//
//=============================================================================
// 实现API Hook业务
//=============================================================================
//

#include "lib_leakprobe.h"

#include <Psapi.h>

#include "Tlhelp32.h"
#pragma comment(lib,"Toolhelp.lib")


//
//-----------------------------------------------------------------------------
// 模块PE文件结构
//-----------------------------------------------------------------------------
//
typedef struct
{
    BOOL                    loaded;             // 该项已经被载入(有效性)

    HMODULE                 moduleHandle;       // 模块句柄

    WCHAR                   modulePath[MAX_PATH];  // 模块地址

    LPVOID                  baseOfDll;          // 模块加载到内存的基地址

    DWORD                   sizeOfImage;        // 模块镜像大小

    IMAGE_DATA_DIRECTORY    entryOfIAT;         // IAT表入口信息

    BOOL                    isIATWriteable;     // IAT表是否可以改写

}__ModulePEInfo;


//
//-----------------------------------------------------------------------------
// 模块信息缓存数组, 用于快速查找模块信息
//-----------------------------------------------------------------------------
//
#define MAX_MODULE_INFO_CACHE  64
__ModulePEInfo  g_moduleInfoCache[MAX_MODULE_INFO_CACHE] = {0};


//
//-----------------------------------------------------------------------------
// 查找给定的虚拟地址属于哪个模块
//-----------------------------------------------------------------------------
//
static __ModulePEInfo*  LookupAddress (void*  addr)
{
    for (size_t  i = 0; i < MAX_MODULE_INFO_CACHE; i++)
    {
        __ModulePEInfo*  modInfo = &(g_moduleInfoCache[i]);
        if (modInfo->loaded)
        {
            // 判断给定的地址是否在模块镜像地址内
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
// 从模块缓冲中获取已经缓冲的模块信息, 如果找不到则返回NULL
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
// 获取一个没有用过的cache项
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
// 从实体文件中获取模块的IAT目录信息
//-----------------------------------------------------------------------------
//
static IMAGE_DATA_DIRECTORY  GetModuleIATFromFileObject (FILE*  fp)
{
    LONG  e_lfanew = 0;
    IMAGE_NT_HEADERS  ntHeader;
    IMAGE_DATA_DIRECTORY  dirInfo = {0};


    // 例行检查
    assert(fp);

    // 定位到dosheader的e_lfanew位置
    if (fseek(fp, offsetof(IMAGE_DOS_HEADER, e_lfanew), SEEK_SET) != 0)
    {
        LKP_DEBUGLOG(L"LKP_HOOK: file format e_lfanew seek error\n");
        return dirInfo;
    }

    // 获得ntheader位置
    if (!fread(&e_lfanew, sizeof(e_lfanew), 1, fp))
    {
        LKP_DEBUGLOG(L"LKP_HOOK: file format e_lfanew read error\n");
        return dirInfo;
    }

    // 定位到ntheader位置
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
// 从实体文件中获取模块的IAT目录信息
//-----------------------------------------------------------------------------
//
static IMAGE_DATA_DIRECTORY  GetModuleIATFromFile (const WCHAR*  modulePath)
{
    FILE*  fp = NULL;
    IMAGE_DATA_DIRECTORY  dirInfo = {0};


    // 例行检查
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
// 加载模块信息, 把分析结果放入moduleInfo, 并返回该项对象
//-----------------------------------------------------------------------------
//
static __ModulePEInfo*  LoadModuleInfoToItem (HMODULE  module, __ModulePEInfo*  retModuleInfo)
{

    MODULEINFO  theModInfo;


    // 例行检查
    assert(retModuleInfo);
    assert(!retModuleInfo->loaded);

    // 获得模块基地址信息
    if (!GetModuleInformation(GetCurrentProcess(), module, &theModInfo, sizeof(theModInfo)))
    {
        LKP_DEBUGLOG(L"LKP_HOOK: get module 0x%x info fail\n", module);
    }

    // 填充返回的模块性信息
    retModuleInfo->moduleHandle = module;
    GetModuleFileName(module, retModuleInfo->modulePath, MAX_PATH);
    retModuleInfo->baseOfDll = theModInfo.lpBaseOfDll;
    retModuleInfo->sizeOfImage = theModInfo.SizeOfImage;
    retModuleInfo->entryOfIAT = GetModuleIATFromFile(retModuleInfo->modulePath);

    if (retModuleInfo->entryOfIAT.VirtualAddress)
    {
        // 测试IAT表是否可以改写
        LPVOID  addrOfIAT = (LPVOID)((DWORD)retModuleInfo->baseOfDll + retModuleInfo->entryOfIAT.VirtualAddress);
        MEMORY_BASIC_INFORMATION  mbi = {0};
        VirtualQuery(addrOfIAT, &mbi, sizeof(mbi));

        if (mbi.Protect == PAGE_READWRITE)
        {
            // 标志该表可改写
            retModuleInfo->isIATWriteable = TRUE;

        }
        else
        {
            // 标志该表不可改写
            retModuleInfo->isIATWriteable = FALSE;

            LKP_DEBUGLOG(L"LKP_HOOK: module %s IAT entry readonly\n", retModuleInfo->modulePath);
        }
    }
    else
    {
        LKP_DEBUGLOG(L"LKP_HOOK: get module %s IAT entry fail\n", retModuleInfo->modulePath);
    }




    // 标记该项已经被cache
    retModuleInfo->loaded = TRUE;
    return retModuleInfo;
}


//
//-----------------------------------------------------------------------------
// 缓冲指定模块的信息, 并返回获得的信息
// 
// 同一个模块不会被重复缓冲, 如果缓冲失败则返回NULL
//-----------------------------------------------------------------------------
//
static __ModulePEInfo*  CacheModuleInfo (HMODULE  module)
{
    __ModulePEInfo*  moduleInfo = NULL;

    // 获得已经缓冲的对象
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
// 替换一个模块中的函数调用
//-----------------------------------------------------------------------------
//
static BOOL  ReplaceInOneModule (__ModulePEInfo*  calleeModuleInfo, 
                                 PROC  currentFunction, PROC  newFunction, HMODULE  callerModule)
{
    __ModulePEInfo*  callerModuleInfo = NULL;
    PROC*  iterFunction = NULL;
    PROC*  beginFunction = NULL;
    PROC*  endFunction = NULL;


    // 获得函数调用模块的信息
    callerModuleInfo = CacheModuleInfo(callerModule);
    if (!callerModuleInfo)
    {
        LKP_DEBUGLOG(L"LKP_HOOK: load caller module 0x%x info fail\n", callerModule);
        return FALSE;
    }

    // 遍历IAT表, 替换函数入口
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
// Hook指定模块中实现的API, 用新函数替换当前的API
//
// 返回被HOOK的函数, Hook失败返回NULL   (非线程安全)
//-----------------------------------------------------------------------------
//
PROC  Lkp_HookApiDirect (HMODULE  calleeModule, PROC  currentFunction, PROC  newFunction)
{
    __ModulePEInfo*  calleeModuleInfo = NULL;
    HANDLE  snapShot = INVALID_HANDLE_VALUE;


    // 缓冲并获得被调用模块的信息
    calleeModuleInfo = CacheModuleInfo(calleeModule);
    if (!calleeModuleInfo)
    {
        // 获得被调用模块信息失败
        LKP_DEBUGLOG(L"LKP_HOOK: load callee module 0x%x info fail\n", calleeModule);
        return NULL;
    }

    // 遍历本进程载入的所有模块, Hook这些模块的跳转表
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


    // Hook主模块的跳转表
    if (calleeModule != NULL)
    {
        ReplaceInOneModule(calleeModuleInfo, currentFunction, newFunction, NULL);
    }


    return currentFunction;
}

//
//-----------------------------------------------------------------------------
// Hook指定模块中实现的API, 用新函数替换当前的API 
//
// 返回被HOOK的函数, Hook失败返回NULL   (非线程安全)
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
// 卸载指定模块中实现的API的HOOK, 将newFunction换回原始的currentFunctionName
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
// 查找给定的地址属于哪模块, 返回模块名称, 查找失败返回NULL
//
// 返回值不用释放
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