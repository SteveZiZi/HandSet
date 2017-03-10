//
//=============================================================================
// ʵ��API Hookҵ��
//=============================================================================
//

#include "lib_leakprobe.h"

#include <Psapi.h>

#include "Tlhelp32.h"
#pragma comment(lib,"Toolhelp.lib")


//
//-----------------------------------------------------------------------------
// ģ��PE�ļ��ṹ
//-----------------------------------------------------------------------------
//
typedef struct
{
    BOOL                    loaded;             // �����Ѿ�������(��Ч��)

    HMODULE                 moduleHandle;       // ģ����

    WCHAR                   modulePath[MAX_PATH];  // ģ���ַ

    LPVOID                  baseOfDll;          // ģ����ص��ڴ�Ļ���ַ

    DWORD                   sizeOfImage;        // ģ�龵���С

    IMAGE_DATA_DIRECTORY    entryOfIAT;         // IAT�������Ϣ

    BOOL                    isIATWriteable;     // IAT���Ƿ���Ը�д

}__ModulePEInfo;


//
//-----------------------------------------------------------------------------
// ģ����Ϣ��������, ���ڿ��ٲ���ģ����Ϣ
//-----------------------------------------------------------------------------
//
#define MAX_MODULE_INFO_CACHE  64
__ModulePEInfo  g_moduleInfoCache[MAX_MODULE_INFO_CACHE] = {0};


//
//-----------------------------------------------------------------------------
// ���Ҹ����������ַ�����ĸ�ģ��
//-----------------------------------------------------------------------------
//
static __ModulePEInfo*  LookupAddress (void*  addr)
{
    for (size_t  i = 0; i < MAX_MODULE_INFO_CACHE; i++)
    {
        __ModulePEInfo*  modInfo = &(g_moduleInfoCache[i]);
        if (modInfo->loaded)
        {
            // �жϸ����ĵ�ַ�Ƿ���ģ�龵���ַ��
            if (addr > modInfo->baseOfDll && 
                addr < ((BYTE*)modInfo->baseOfDll + modInfo->sizeOfImage))
            {
                return modInfo;
            }
        }
    }

    return NULL;
}

//
//-----------------------------------------------------------------------------
// ��ģ�黺���л�ȡ�Ѿ������ģ����Ϣ, ����Ҳ����򷵻�NULL
//-----------------------------------------------------------------------------
//
static __ModulePEInfo*  FindModuleInfo (HMODULE  module)
{
    size_t  i = 0;

    for (i = 0; i < MAX_MODULE_INFO_CACHE; i++)
    {
        if (g_moduleInfoCache[i].loaded && g_moduleInfoCache[i].moduleHandle == module)
        {
            return &(g_moduleInfoCache[i]);
        }
    }

    return NULL;
}

//
//-----------------------------------------------------------------------------
// ��ȡһ��û���ù���cache��
//-----------------------------------------------------------------------------
//
static __ModulePEInfo*  GetUnusedModuleInfoItem ()
{
    size_t  i = 0;

    for (i = 0; i < MAX_MODULE_INFO_CACHE; i++)
    {
        if (!g_moduleInfoCache[i].loaded)
        {
            return &(g_moduleInfoCache[i]);
        }
    }

    return NULL;
}

//
//-----------------------------------------------------------------------------
// ��ʵ���ļ��л�ȡģ���IATĿ¼��Ϣ
//-----------------------------------------------------------------------------
//
static IMAGE_DATA_DIRECTORY  GetModuleIATFromFileObject (FILE*  fp)
{
    LONG  e_lfanew = 0;
    IMAGE_NT_HEADERS  ntHeader;
    IMAGE_DATA_DIRECTORY  dirInfo = {0};


    // ���м��
    assert(fp);

    // ��λ��dosheader��e_lfanewλ��
    if (fseek(fp, offsetof(IMAGE_DOS_HEADER, e_lfanew), SEEK_SET) != 0)
    {
        LKP_DEBUGLOG(L"LKP_HOOK: file format e_lfanew seek error\n");
        return dirInfo;
    }

    // ���ntheaderλ��
    if (!fread(&e_lfanew, sizeof(e_lfanew), 1, fp))
    {
        LKP_DEBUGLOG(L"LKP_HOOK: file format e_lfanew read error\n");
        return dirInfo;
    }

    // ��λ��ntheaderλ��
    if (fseek(fp, e_lfanew, SEEK_SET) != 0)
    {
        LKP_DEBUGLOG(L"LKP_HOOK: file format ntheader seek error\n");
        return dirInfo;
    }

    if (!fread(&ntHeader, sizeof(ntHeader), 1, fp))
    {
        LKP_DEBUGLOG(L"LKP_HOOK: file format ntheader read error\n");
        return dirInfo;
    }

    dirInfo = ntHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT];

    return dirInfo;
}


//
//-----------------------------------------------------------------------------
// ��ʵ���ļ��л�ȡģ���IATĿ¼��Ϣ
//-----------------------------------------------------------------------------
//
static IMAGE_DATA_DIRECTORY  GetModuleIATFromFile (const WCHAR*  modulePath)
{
    FILE*  fp = NULL;
    IMAGE_DATA_DIRECTORY  dirInfo = {0};


    // ���м��
    assert(modulePath);


    fp = _wfopen(modulePath, L"rb");
    if (fp)
    {

        dirInfo = GetModuleIATFromFileObject(fp);

        fclose(fp);
    }
    else
    {
        LKP_DEBUGLOG(L"LKP_HOOK: open file %s fail\n", modulePath);
    }

    return dirInfo;
}

//
//-----------------------------------------------------------------------------
// ����ģ����Ϣ, �ѷ����������moduleInfo, �����ظ������
//-----------------------------------------------------------------------------
//
static __ModulePEInfo*  LoadModuleInfoToItem (HMODULE  module, __ModulePEInfo*  retModuleInfo)
{

    MODULEINFO  theModInfo;


    // ���м��
    assert(retModuleInfo);
    assert(!retModuleInfo->loaded);

    // ���ģ�����ַ��Ϣ
    if (!GetModuleInformation(GetCurrentProcess(), module, &theModInfo, sizeof(theModInfo)))
    {
        LKP_DEBUGLOG(L"LKP_HOOK: get module 0x%x info fail\n", module);
    }

    // ��䷵�ص�ģ������Ϣ
    retModuleInfo->moduleHandle = module;
    GetModuleFileName(module, retModuleInfo->modulePath, MAX_PATH);
    retModuleInfo->baseOfDll = theModInfo.lpBaseOfDll;
    retModuleInfo->sizeOfImage = theModInfo.SizeOfImage;
    retModuleInfo->entryOfIAT = GetModuleIATFromFile(retModuleInfo->modulePath);

    if (retModuleInfo->entryOfIAT.VirtualAddress)
    {
        // ����IAT���Ƿ���Ը�д
        LPVOID  addrOfIAT = (LPVOID)((DWORD)retModuleInfo->baseOfDll + retModuleInfo->entryOfIAT.VirtualAddress);
        MEMORY_BASIC_INFORMATION  mbi = {0};
        VirtualQuery(addrOfIAT, &mbi, sizeof(mbi));

        if (mbi.Protect == PAGE_READWRITE)
        {
            // ��־�ñ�ɸ�д
            retModuleInfo->isIATWriteable = TRUE;

        }
        else
        {
            // ��־�ñ��ɸ�д
            retModuleInfo->isIATWriteable = FALSE;

            LKP_DEBUGLOG(L"LKP_HOOK: module %s IAT entry readonly\n", retModuleInfo->modulePath);
        }
    }
    else
    {
        LKP_DEBUGLOG(L"LKP_HOOK: get module %s IAT entry fail\n", retModuleInfo->modulePath);
    }




    // ��Ǹ����Ѿ���cache
    retModuleInfo->loaded = TRUE;
    return retModuleInfo;
}


//
//-----------------------------------------------------------------------------
// ����ָ��ģ�����Ϣ, �����ػ�õ���Ϣ
// 
// ͬһ��ģ�鲻�ᱻ�ظ�����, �������ʧ���򷵻�NULL
//-----------------------------------------------------------------------------
//
static __ModulePEInfo*  CacheModuleInfo (HMODULE  module)
{
    __ModulePEInfo*  moduleInfo = NULL;

    // ����Ѿ�����Ķ���
    moduleInfo = FindModuleInfo(module);
    if (moduleInfo)
    {
        return moduleInfo;
    }
    else
    {
        moduleInfo = GetUnusedModuleInfoItem();
        if (moduleInfo)
        {
            return LoadModuleInfoToItem(module, moduleInfo);
        }
        else
        {
            LKP_DEBUGLOG(L"LKP_HOOK: module info cache full, max size %d\n", 
                MAX_MODULE_INFO_CACHE);
            return NULL;
        }
    }
}


//
//-----------------------------------------------------------------------------
// �滻һ��ģ���еĺ�������
//-----------------------------------------------------------------------------
//
static BOOL  ReplaceInOneModule (__ModulePEInfo*  calleeModuleInfo, 
                                 PROC  currentFunction, PROC  newFunction, HMODULE  callerModule)
{
    __ModulePEInfo*  callerModuleInfo = NULL;
    PROC*  iterFunction = NULL;
    PROC*  beginFunction = NULL;
    PROC*  endFunction = NULL;


    // ��ú�������ģ�����Ϣ
    callerModuleInfo = CacheModuleInfo(callerModule);
    if (!callerModuleInfo)
    {
        LKP_DEBUGLOG(L"LKP_HOOK: load caller module 0x%x info fail\n", callerModule);
        return FALSE;
    }

    // ����IAT��, �滻�������
    beginFunction = (PROC*)((BYTE*)callerModuleInfo->baseOfDll + callerModuleInfo->entryOfIAT.VirtualAddress);
    endFunction = (PROC*)((BYTE*)beginFunction + callerModuleInfo->entryOfIAT.Size);
    for (iterFunction = beginFunction; iterFunction != endFunction; iterFunction++)
    {
        if (*iterFunction == currentFunction && callerModuleInfo->isIATWriteable)
        {
            *iterFunction = newFunction;
        }
    }

    return TRUE;
}



//
//-----------------------------------------------------------------------------
// Hookָ��ģ����ʵ�ֵ�API, ���º����滻��ǰ��API
//
// ���ر�HOOK�ĺ���, Hookʧ�ܷ���NULL   (���̰߳�ȫ)
//-----------------------------------------------------------------------------
//
PROC  Lkp_HookApiDirect (HMODULE  calleeModule, PROC  currentFunction, PROC  newFunction)
{
    __ModulePEInfo*  calleeModuleInfo = NULL;
    HANDLE  snapShot = INVALID_HANDLE_VALUE;


    // ���岢��ñ�����ģ�����Ϣ
    calleeModuleInfo = CacheModuleInfo(calleeModule);
    if (!calleeModuleInfo)
    {
        // ��ñ�����ģ����Ϣʧ��
        LKP_DEBUGLOG(L"LKP_HOOK: load callee module 0x%x info fail\n", calleeModule);
        return NULL;
    }

    // �������������������ģ��, Hook��Щģ�����ת��
    snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (snapShot != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32  callerModinfo = {sizeof(callerModinfo)};
        BOOL  findMod = Module32First(snapShot, &callerModinfo);
        while (findMod == TRUE)
        {
            assert(callerModinfo.hModule);
            if (callerModinfo.hModule != calleeModule)
            {
                ReplaceInOneModule(calleeModuleInfo, currentFunction, newFunction, callerModinfo.hModule);
            }

            findMod = Module32Next(snapShot, &callerModinfo);
        }

        CloseToolhelp32Snapshot(snapShot);
    }


    // Hook��ģ�����ת��
    if (calleeModule != NULL)
    {
        ReplaceInOneModule(calleeModuleInfo, currentFunction, newFunction, NULL);
    }


    return currentFunction;
}

//
//-----------------------------------------------------------------------------
// Hookָ��ģ����ʵ�ֵ�API, ���º����滻��ǰ��API 
//
// ���ر�HOOK�ĺ���, Hookʧ�ܷ���NULL   (���̰߳�ȫ)
//-----------------------------------------------------------------------------
//
PROC  Lkp_HookApi (LPCWSTR  calleeModuleName, LPCWSTR  currentFunctionName, PROC  newFunction)
{
    HMODULE  calleeModule = GetModuleHandle(calleeModuleName);
    if (calleeModule)
    {
        PROC  oldFunction = GetProcAddress(calleeModule, currentFunctionName);
        if (oldFunction)
        {
            return Lkp_HookApiDirect(calleeModule, oldFunction, newFunction);
        }
        else
        {
            LKP_DEBUGLOG(L"LKP_HOOK: get callee function %s fail\n", currentFunctionName);
            return NULL;
        }
    }
    else
    {
        LKP_DEBUGLOG(L"LKP_HOOK: get callee module %s handle fail\n", calleeModuleName);
        return NULL;
    }
}

//
//-----------------------------------------------------------------------------
// ж��ָ��ģ����ʵ�ֵ�API��HOOK, ��newFunction����ԭʼ��currentFunctionName
//-----------------------------------------------------------------------------
//
PROC  Lkp_UnHookApi (LPCWSTR  calleeModuleName, LPCWSTR  currentFunctionName, PROC  newFunction)
{
    HMODULE  calleeModule = GetModuleHandle(calleeModuleName);
    if (calleeModule)
    {
        PROC  oldFunction = GetProcAddress(calleeModule, currentFunctionName);
        if (oldFunction)
        {
            return Lkp_HookApiDirect(calleeModule, newFunction, oldFunction);
        }
        else
        {
            LKP_DEBUGLOG(L"LKP_HOOK: get callee function %s fail\n", currentFunctionName);
            return NULL;
        }
    }
    else
    {
        LKP_DEBUGLOG(L"LKP_HOOK: get callee module %s handle fail\n", calleeModuleName);
        return NULL;
    }
}

//
//-----------------------------------------------------------------------------
// ���Ҹ����ĵ�ַ������ģ��, ����ģ������, ����ʧ�ܷ���NULL
//
// ����ֵ�����ͷ�
//-----------------------------------------------------------------------------
//
LPCWSTR  Lkp_LookupAddressOwner (void*  addr)
{
    __ModulePEInfo*  memInfo = LookupAddress(addr);
    if (memInfo)
    {
        return memInfo->modulePath;
    }
    else
    {
        return NULL;
    }
}