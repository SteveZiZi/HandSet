//
//=============================================================================
// ʵ���ڴ�����ͷ�̽��ҵ��
//
// ��¼�ڴ����, ��󱨸�����Щû���ͷ�
//=============================================================================
//

#include "lib_leakprobe.h"


//
//-----------------------------------------------------------------------------
// ���뺯��������
//-----------------------------------------------------------------------------
//
#define IMPFN_MALLOC                L"malloc"
#define IMPFN_FREE                  L"free"
#define IMPFN_CALLOC                L"calloc"
#define IMPFN_REALLOC               L"realloc"
#define IMPFN_STRDUP                L"_strdup"
#define IMPFN_WSTRDUP               L"_wcsdup"
#define IMPFN_NEW                   L"??2@YAPAXI@Z"
#define IMPFN_DELETE                L"??3@YAXPAX@Z"
#define IMPFN_NEWARRAY              L"??_U@YAPAXI@Z"
#define IMPFN_DELETEARRAY           L"??_V@YAXPAX@Z"
#define IMPFN_NEWNOTHROW            L"??2@YAPAXIABUnothrow_t@std@@@Z"
#define IMPFN_DELETENOTHROW         L"??3@YAXPAXABUnothrow_t@std@@@Z"
#define IMPFN_NEWARRAYNOTHROW       L"??_U@YAPAXIABUnothrow_t@std@@@Z"
#define IMPFN_DELETEARRAYNOTHROW    L"??_V@YAXPAXABUnothrow_t@std@@@Z"
#define IMPFN_LOCALALLOC            L"LocalAlloc"
#define IMPFN_LOCALFREE             L"LocalFree"
#define IMPFN_LOCALREALLOC          L"LocalReAlloc"



//
//-----------------------------------------------------------------------------
// �ڴ���������
//-----------------------------------------------------------------------------
//
typedef enum
{
    MEMTYPE_MALLOC      = 1,                    // ʹ��malloc�����

    MEMTYPE_NEW         = 2,                    // ʹ��new����� ,��nothrow����

    MEMTYPE_NEWARRAY    = 3,                    // ʹ��new[]�����, ��nothrow����

    MEMTYPE_LOCALALLOC  = 4                     // ʹ��LocalAlloc�����

} __MemAllocType;


//
//-----------------------------------------------------------------------------
// �ڴ������Ϣ
//-----------------------------------------------------------------------------
//
#define MAX_CALLSTACK_FRAME  12
typedef struct
{
    BOOL                alloced;                // �����Ƿ�ռ��

    void*               handle;                 // �ڴ�����ʶ

    size_t              size;                   // �ڴ����Ĵ�С

    __MemAllocType      type;                   // �ڴ���������

    CallSnapshot        callstack[MAX_CALLSTACK_FRAME];  // ��¼�ڴ����ʱ����ջ���, ���ڲ鿴����ջ

	DWORD64				alloccnt;				// �������

} __MemAllocInfo;

//
//-----------------------------------------------------------------------------
// �ڴ������Ϣ��, ���ڼ�¼�����ڴ�����
//-----------------------------------------------------------------------------
//
#define MAX_MEMALLOC_INFO  65536
__MemAllocInfo  g_memAllocList[MAX_MEMALLOC_INFO] = {0};

static DWORD64  g_memAllocCount = 0;
static DWORD64  g_memBreakCount = 0;
static void*  g_memBreakHandle = NULL;

//
//-----------------------------------------------------------------------------
// g_memAllocList������
//-----------------------------------------------------------------------------
//
CRITICAL_SECTION  g_memAllocListLock;
#define INITLOCK_MEMALLOCLIST()  InitializeCriticalSection(&g_memAllocListLock)
#define UNINITLOCK_MEMALLOCLIST()  DeleteCriticalSection(&g_memAllocListLock)
#define LOCK_MEMALLOCLIST()  EnterCriticalSection(&g_memAllocListLock)
#define UNLOCK_MEMALLOCLIST()  LeaveCriticalSection(&g_memAllocListLock)



//=============================================================================
// �ڴ������Ϣ���������
//=============================================================================

//
//-----------------------------------------------------------------------------
// hash�㷨, ���ڿ��ٶ�λ�����, �����ڴ��ʶ, ����hashֵ
//-----------------------------------------------------------------------------
//
static size_t  HashMemAllocList (void*  handle)
{
    return (size_t)handle % MAX_MEMALLOC_INFO;
}

//
//-----------------------------------------------------------------------------
// �ҵ��ڴ�����Ӧ����, �Ҳ�������NULL
//-----------------------------------------------------------------------------
//
static __MemAllocInfo*  FindMemAllocInfo (void*  handle)
{
    size_t  hit = HashMemAllocList(handle);
    if (g_memAllocList[hit].alloced && g_memAllocList[hit].handle == handle)
    {
        return &(g_memAllocList[hit]);
    }

    // hashʧ��, ͨ��Ӳѭ������
    for (size_t  i = 0; i < MAX_MEMALLOC_INFO; i++)
    {
        if (g_memAllocList[i].alloced && g_memAllocList[i].handle == handle)
        {
            return &(g_memAllocList[i]);
        }
    }

    return NULL;
}

//
//-----------------------------------------------------------------------------
// ���һ��δʹ�õ���, �����������, �򷵻�NULL
//
// �����ṩһ����Ҫ����ı�ʶ, ����hash
//-----------------------------------------------------------------------------
//
static __MemAllocInfo*  GetUnusedMemAllocItem (void*  handle)
{
    size_t  hit = HashMemAllocList(handle);
    if (!g_memAllocList[hit].alloced)
    {
        // ���û�б�ʹ��
        return &(g_memAllocList[hit]);
    }

    // hashʧ��, ͨ��Ӳѭ������
    for (size_t  i = 0; i < MAX_MEMALLOC_INFO; i++)
    {
        if (!g_memAllocList[i].alloced)
        {
            return &(g_memAllocList[i]);
        }
    }

    return NULL;
}

//
//-----------------------------------------------------------------------------
// ���ڴ��������б����һ����, ���� new, malloc, �Ȳ���
//-----------------------------------------------------------------------------
//
static __MemAllocInfo*  MemAllocList_Alloc (void*  handle, size_t  size, __MemAllocType  type)
{
    if (!handle)
    {
        LKP_DEBUGLOG(L"LKP_MEM: mem alloc param error\n");
        return NULL;
    }

    LOCK_MEMALLOCLIST();

    __MemAllocInfo*  memInfo = GetUnusedMemAllocItem(handle);
    if (memInfo)
    {
        // ������������
        memInfo->handle = handle;
        memInfo->size = size;
        memInfo->type = type;

        // ��õ���ջ, ������ģ��ĵ��ÿ��, Ŀǰ������ 3
        GetCallStackSnapshot(MAX_CALLSTACK_FRAME, memInfo->callstack, 0, 3);


		// ��¼�������
		memInfo->alloccnt = ++g_memAllocCount;


        // ��Ǳ����Ѿ���ʹ��
        memInfo->alloced = TRUE;

		if (memInfo->alloccnt == g_memBreakCount)
		{
			::DebugBreak();
		}

		if (memInfo->handle == g_memBreakHandle)
		{
			::DebugBreak();
		}

    }
    else
    {
        // �б���, �޷�������¼
        LKP_DEBUGLOG(L"LKP_MEM: mem info list full, max size %d\n", MAX_MEMALLOC_INFO);
    }

    UNLOCK_MEMALLOCLIST();
    return memInfo;
}

//
//-----------------------------------------------------------------------------
// ���ڴ��������б��ͷ�һ����, ���� free, delete �Ȳ���
//-----------------------------------------------------------------------------
//
static void  MemAllocList_Free (void*  handle, __MemAllocType  type)
{
    if (!handle)
    {
        LKP_DEBUGLOG(L"LKP_MEM: mem free param error\n");
        return;
    }


    LOCK_MEMALLOCLIST();

    __MemAllocInfo*  memInfo = FindMemAllocInfo(handle);
    if (memInfo)
    {

        // ��Ǳ���δ��ʹ��, �ͷű���
        memInfo->alloced = FALSE;

        // ����ڴ���������Ƿ�һ��
        if (memInfo->type == type)
        {
        }
        else
        {
            // �ͷŵ��ڴ����������ʱ��һ��
            LKP_DEBUGLOG(L"LKP_MEM: mem free block 0x%p type diff with alloc, alloc type %d, free type %d\n", 
                handle, memInfo->type, type);
        }

    }
    else
    {
        // �ͷŵ��ڴ����û�б����������Ч��
        LKP_DEBUGLOG(L"LKP_MEM: mem free null block 0x%p\n", handle);
    }


    UNLOCK_MEMALLOCLIST();
}

//
//-----------------------------------------------------------------------------
// ���ڴ��������б����һ����, ���� realloc�Ȳ���
//-----------------------------------------------------------------------------
//
static __MemAllocInfo*  MemAllocList_Update (void*  handle, size_t  newSize, __MemAllocType  type)
{
    if (!handle)
    {
        LKP_DEBUGLOG(L"LKP_MEM: mem update param error\n");
        return NULL;
    }


    LOCK_MEMALLOCLIST();

    __MemAllocInfo*  memInfo = FindMemAllocInfo(handle);
    if (memInfo)
    {
        // ����ڴ���������Ƿ�һ��
        if (memInfo->type == type)
        {
            memInfo->size = newSize;
        }
        else
        {
            // ���µ��ڴ����������ʱ��һ��
            LKP_DEBUGLOG(L"LKP_MEM: mem update block 0x%p type diff with alloc, alloc type %d, update type %d\n", 
                handle, memInfo->type, type);
        }

    }
    else
    {
        // ���µ��ڴ����û�б����������Ч��
        LKP_DEBUGLOG(L"LKP_MEM: mem update null block 0x%p\n", handle);
    }


    UNLOCK_MEMALLOCLIST();
    return memInfo;
}


//
//-----------------------------------------------------------------------------
// ��ʾ�ڴ������ϸ����
//-----------------------------------------------------------------------------
//
static void  MemAllocList_DumpDetail (__MemAllocInfo*  memInfo)
{
    // ��ʾ�ڴ�����, ǰ16�ֽ�   
    BYTE*  memEntry = (BYTE*)memInfo->handle;
    
    // ��ʾʮ����������
    LKP_DEBUGLOG(L"\t");
    for (size_t  i = 0; i < 16 && i < memInfo->size; i++)
    {
        LKP_DEBUGLOG(L"%02x ", memEntry[i]);
    }

    // ��ʾ�ַ�
    LKP_DEBUGLOG(L"\t[");
    for (size_t  i = 0; i < 16 && i < memInfo->size; i++)
    {
        LKP_DEBUGLOG(L"%c", memEntry[i]);
    }
    LKP_DEBUGLOG(L"]\n");


    // ��ʾ����ջ
    for (size_t  i = 0; i < MAX_CALLSTACK_FRAME; i++)
    {
		if (!memInfo->callstack[i].dwReturnAddr)
		{
			// û�и���ĵ�����Ϣ
			break;
		}


        LKP_DEBUGLOG(L"\t@return 0x%08p", memInfo->callstack[i].dwReturnAddr);

        // ����ģ������
        LKP_DEBUGLOG(L"\t<%s>\n", Lkp_LookupAddressOwner((void*)memInfo->callstack[i].dwReturnAddr));
    }
    LKP_DEBUGLOG(L"\n");
}


//
//-----------------------------------------------------------------------------
// ��ʾ�ڴ�ʹ�����, ����dump����ڴ�δ�ͷŵ���Ϣ
//-----------------------------------------------------------------------------
//
static void  MemAllocList_Dump (BOOL  showDetail)
{
    LOCK_MEMALLOCLIST();

    size_t  leakItemCount = 0;      // й¶����Ŀ, ����һ���ڴ����һ��
    size_t  leakByteSum = 0;        // й¶�ڴ�����ֽ���

    for (size_t  i = 0; i < MAX_MEMALLOC_INFO; i++)
    {
        if (g_memAllocList[i].alloced)
        {
            LKP_DEBUGLOG(L"LKP_MEM: !!!memory leak!!! ==> cnt %I64u, handle 0x%p, size %d, type %d\n", 
                g_memAllocList[i].alloccnt, g_memAllocList[i].handle, g_memAllocList[i].size, g_memAllocList[i].type);

            if (showDetail)
            {
                MemAllocList_DumpDetail(&(g_memAllocList[i]));
            }

            // ͳ��
            leakItemCount++;
            leakByteSum += g_memAllocList[i].size;
        }
    }

    if (leakItemCount > 0)
    {
        LKP_DEBUGLOG(L"LKP_MEM: leak item %d, leak byte %d\n", leakItemCount, leakByteSum);
    }

    UNLOCK_MEMALLOCLIST();
}



//=============================================================================
// �����ڴ�hook�����Լ��ɺ�������
//=============================================================================

//
//-----------------------------------------------------------------------------
// ���ַ����ͷź���ָ�����Ͷ���
//-----------------------------------------------------------------------------
//
typedef void*   (__cdecl  *FNCommonAlloc) (size_t  _Size);
typedef void    (__cdecl  *FNCommonFree) (void*  _Memory);
typedef void*   (__cdecl  *FNCalloc) (size_t  _NumOfElements, size_t  _SizeOfElements);
typedef void*   (__cdecl  *FNRealloc) (void*  _Memory, size_t  _NewSize);
typedef void*   (__cdecl  *FNCommonDup) (void*  _Memory);
typedef void*   (__cdecl  *FNNewNothrow) (size_t  _Size, void*  x);
typedef void    (__cdecl  *FNDeleteNothrow) (void*  _Memory, void*  x);
typedef HLOCAL  (WINAPI  *FNLocalAlloc) (UINT  fuFlags, UINT  cbBytes);
typedef HLOCAL  (WINAPI  *FNLocalFree) (HLOCAL  hMem);
typedef HLOCAL  (WINAPI  *FNLocalReAlloc) (HLOCAL  hMem, UINT  cbBytes, UINT  fuFlags);

//
//-----------------------------------------------------------------------------
// malloc/free/calloc/realloc
//-----------------------------------------------------------------------------
//
static FNCommonAlloc    g_oldMalloc = NULL;
static FNCommonFree     g_oldFree = NULL;
static FNCalloc         g_oldCalloc = NULL;
static FNRealloc        g_oldRealloc = NULL;
static FNCommonDup      g_oldStrdup = NULL;
static FNCommonDup      g_oldWStrdup = NULL;
static void*  __cdecl  newMalloc (size_t _Size)
{
    assert(g_oldMalloc);
    void*  handle = g_oldMalloc(_Size);

    MemAllocList_Alloc(handle, _Size, MEMTYPE_MALLOC);

    return handle;
}
static void  __cdecl  newFree (void * _Memory)
{
    assert(g_oldFree);
    g_oldFree(_Memory);

    MemAllocList_Free(_Memory, MEMTYPE_MALLOC);
}
static void*  __cdecl  newCalloc (size_t  _NumOfElements, size_t  _SizeOfElements)
{
    assert(g_oldCalloc);
    void*  handle = g_oldCalloc(_NumOfElements, _SizeOfElements);

    MemAllocList_Alloc(handle, _NumOfElements*_SizeOfElements, MEMTYPE_MALLOC);

    return handle;
}
static void*  __cdecl  newRealloc (void*  _Memory, size_t  _NewSize)
{
    //
    // ע��realloc�Ķ��غ���,
    //   _MemoryΪ0ʱ, ����ȫ�·���һ���ڴ���
    //   _NewSizeΪ0ʱ, ����Ҫ�ͷ��ڴ�
    //   _Memory��0, _NewSize��0ʱ, ����Ҫ���·����ڴ�
    //

    if (_Memory == NULL && _NewSize == 0)
    {
        LKP_DEBUGLOG(L"LKP_MEM: realloc param error\n");
    }


    assert(g_oldRealloc);
    void*  newHandle = g_oldRealloc(_Memory, _NewSize);

    if (newHandle == _Memory)
    {
        // �ڴ�û�б����·���, �����ԭ������
        MemAllocList_Update(newHandle, _NewSize, MEMTYPE_MALLOC);
    }
    else
    {
        // �ڴ汻���·���, ��Ҫɾ��ԭ������, ����������µ���
        if (_Memory)
        {
            // _Memory�п�����0, ����ȫ�·���һ���ڴ�
            MemAllocList_Free(_Memory, MEMTYPE_MALLOC);
        }
        if (_NewSize)
        {
            MemAllocList_Alloc(newHandle, _NewSize, MEMTYPE_MALLOC);
        }
    }

    return newHandle;
}
static void*  __cdecl  newStrdup (void*  _Memory)
{
    assert(g_oldStrdup);
    void*  handle = g_oldStrdup(_Memory);

    MemAllocList_Alloc(handle, 1, MEMTYPE_MALLOC);

    return handle;
}
static void*  __cdecl  newWStrdup (void*  _Memory)
{
    assert(g_oldWStrdup);
    void*  handle = g_oldWStrdup(_Memory);

    MemAllocList_Alloc(handle, 1, MEMTYPE_MALLOC);

    return handle;
}



//
//-----------------------------------------------------------------------------
// new/delete
//-----------------------------------------------------------------------------
//
static FNCommonAlloc    g_oldNew = NULL;
static FNCommonFree     g_oldDelete = NULL;
static void*  __cdecl  newNew (size_t  _Size)
{
    assert(g_oldNew);
    void*  handle = g_oldNew(_Size);

    MemAllocList_Alloc(handle, _Size, MEMTYPE_NEW);
    
    return handle;
}
static void  __cdecl  newDelete (void * _Memory)
{
    assert(g_oldDelete);
    g_oldDelete(_Memory);

    MemAllocList_Free(_Memory, MEMTYPE_NEW);
}

//
//-----------------------------------------------------------------------------
// new[]/delete[]
//-----------------------------------------------------------------------------
//
static FNCommonAlloc    g_oldNewArray = NULL;
static FNCommonFree     g_oldDeleteArray = NULL;
static void*  __cdecl  newNewArray (size_t  _Size)
{
    assert(g_oldNewArray);
    void*  handle = g_oldNewArray(_Size);

    MemAllocList_Alloc(handle, _Size, MEMTYPE_NEWARRAY);

    return handle;
}
static void  __cdecl  newDeleteArray (void * _Memory)
{
    assert(g_oldDeleteArray);
    g_oldDeleteArray(_Memory);

    MemAllocList_Free(_Memory, MEMTYPE_NEWARRAY);
}

//
//-----------------------------------------------------------------------------
// new(nothrow)/delete(nothrow)
//-----------------------------------------------------------------------------
//
static FNNewNothrow     g_oldNewNothrow = NULL;
static FNDeleteNothrow  g_oldDeleteNothrow = NULL;
static void*  __cdecl  newNewNothrow (size_t _Size, void* x)
{
    assert(g_oldNewNothrow);
    void*  handle = g_oldNewNothrow(_Size, x);

    MemAllocList_Alloc(handle, _Size, MEMTYPE_NEW);

    return handle;
}
static void  __cdecl  newDeleteNothrow (void*  _Memory, void*  x)
{
    assert(g_oldDeleteNothrow);
    g_oldDeleteNothrow(_Memory, x);

    MemAllocList_Free(_Memory, MEMTYPE_NEW);
}

//
//-----------------------------------------------------------------------------
// new(nothrow)[]/delete(nothrow)[]
//-----------------------------------------------------------------------------
//
static FNNewNothrow     g_oldNewArrayNothrow = NULL;
static FNDeleteNothrow  g_oldDeleteArrayNothrow = NULL;
static void*  __cdecl  newNewArrayNothrow (size_t _Size, void* x)
{
    assert(g_oldNewArrayNothrow);
    void*  handle = g_oldNewArrayNothrow(_Size, x);

    MemAllocList_Alloc(handle, _Size, MEMTYPE_NEWARRAY);

    return handle;
}
static void  __cdecl  newDeleteArrayNothrow (void*  _Memory, void*  x)
{
    assert(g_oldDeleteArrayNothrow);
    g_oldDeleteArrayNothrow(_Memory, x);

    MemAllocList_Free(_Memory, MEMTYPE_NEWARRAY);
}


//
//-----------------------------------------------------------------------------
// LocalAlloc/LocalFree/LocalRealloc
//-----------------------------------------------------------------------------
//
static FNLocalAlloc     g_oldLocalAlloc = NULL;
static FNLocalFree      g_oldLocalFree = NULL;
static FNLocalReAlloc   g_oldLocalReAlloc = NULL;
static HLOCAL  WINAPI  newLocalAlloc (UINT fuFlags, UINT cbBytes)
{
    assert(g_oldLocalAlloc);
    void*  handle = g_oldLocalAlloc(fuFlags, cbBytes);

    MemAllocList_Alloc(handle, cbBytes, MEMTYPE_LOCALALLOC);

    return handle;
}
static HLOCAL  WINAPI  newLocalFree (HLOCAL  hMem)
{
    assert(g_oldLocalFree);
    void*  handle = g_oldLocalFree(hMem);

    MemAllocList_Free(hMem, MEMTYPE_LOCALALLOC);

    return handle;
}
static HLOCAL  WINAPI  newLocalReAlloc (HLOCAL  hMem, UINT  cbBytes, UINT  fuFlags)
{

    if (hMem == NULL && cbBytes == 0)
    {
        LKP_DEBUGLOG(L"LKP_MEM: LocalReAlloc param error\n");
    }

    assert(g_oldLocalReAlloc);
    void*  newHandle = g_oldLocalReAlloc(hMem, cbBytes, fuFlags);

    if (newHandle == hMem)
    {
        // �ڴ�û�б����·���, �����ԭ������
        MemAllocList_Update(newHandle, cbBytes, MEMTYPE_LOCALALLOC);
    }
    else
    {
        // �ڴ汻���·���, ��Ҫɾ��ԭ������, ����������µ���
        if (hMem)
        {
            MemAllocList_Free(hMem, MEMTYPE_LOCALALLOC);
        }
        if (cbBytes)
        {
            MemAllocList_Alloc(newHandle, cbBytes, MEMTYPE_LOCALALLOC);
        }
    }

    return newHandle;
}



//
//-----------------------------------------------------------------------------
// hook, unhook���ֺ�
//-----------------------------------------------------------------------------
//
#define HOOK_API_HELPER(mod, impfunc, Func, functype)  \
    g_old##Func = (functype)Lkp_HookApi(mod, impfunc, (PROC)new##Func)

#define UNHOOK_API_HELPER(mod, impfunc, Func)  \
    Lkp_UnHookApi(mod, impfunc, (PROC)new##Func)


//
//-----------------------------------------------------------------------------
// Hook���е��ڴ������صĺ���
//-----------------------------------------------------------------------------
//
static void  HookAllMemAllocAPI ()
{
    // malloc
    /*
    g_oldMalloc = (FNCommonAlloc)Lkp_HookApi(L"coredll", IMPFN_MALLOC, (PROC)newMalloc);
    g_oldFree   = (FNCommonFree)Lkp_HookApi(L"coredll", IMPFN_FREE, (PROC)newFree);
    g_oldCalloc = (FNCalloc)Lkp_HookApi(L"coredll", IMPFN_CALLOC, (PROC)newCalloc);
    g_oldRealloc= (FNRealloc)Lkp_HookApi(L"coredll", IMPFN_REALLOC, (PROC)newRealloc);
    */
    HOOK_API_HELPER(L"coredll", IMPFN_MALLOC,   Malloc,     FNCommonAlloc);
    HOOK_API_HELPER(L"coredll", IMPFN_FREE,     Free,       FNCommonFree);
    HOOK_API_HELPER(L"coredll", IMPFN_CALLOC,   Calloc,     FNCalloc);
    HOOK_API_HELPER(L"coredll", IMPFN_REALLOC,  Realloc,    FNRealloc);
    HOOK_API_HELPER(L"coredll", IMPFN_STRDUP,   Strdup,     FNCommonDup);
    HOOK_API_HELPER(L"coredll", IMPFN_WSTRDUP,  WStrdup,    FNCommonDup);
    

    // new
    /*
    g_oldNew    = (FNCommonAlloc)Lkp_HookApi(L"coredll", IMPFN_NEW, (PROC)newNew);
    g_oldDelete = (FNCommonFree)Lkp_HookApi(L"coredll", IMPFN_DELETE, (PROC)newDelete);
    */
    HOOK_API_HELPER(L"coredll", IMPFN_NEW,      New,        FNCommonAlloc);
    HOOK_API_HELPER(L"coredll", IMPFN_DELETE,   Delete,     FNCommonFree);


    // new[]
    /*
    g_oldNewArray = (FNCommonAlloc)Lkp_HookApi(L"coredll", IMPFN_NEWARRAY, (PROC)newNewArray);
    g_oldDeleteArray = (FNCommonFree)Lkp_HookApi(L"coredll", IMPFN_DELETEARRAY, (PROC)newDeleteArray);
    */
    HOOK_API_HELPER(L"coredll", IMPFN_NEWARRAY,     NewArray,       FNCommonAlloc);
    HOOK_API_HELPER(L"coredll", IMPFN_DELETEARRAY,  DeleteArray,    FNCommonFree);


    // new nothrow
    /*
    g_oldNewNothrow = (FNNewNothrow)Lkp_HookApi(L"coredll", IMPFN_NEWNOTHROW, (PROC)newNewNothrow);
    g_oldDeleteNothrow = (FNDeleteNothrow)Lkp_HookApi(L"coredll", IMPFN_DELETENOTHROW, (PROC)newDeleteNothrow);
    */
    HOOK_API_HELPER(L"coredll", IMPFN_NEWNOTHROW,       NewNothrow,       FNNewNothrow);
    HOOK_API_HELPER(L"coredll", IMPFN_DELETENOTHROW,    DeleteNothrow,    FNDeleteNothrow);


    // new[] nothrow
    /*
    g_oldNewArrayNothrow = (FNNewNothrow)Lkp_HookApi(L"coredll", IMPFN_NEWARRAYNOTHROW, (PROC)newNewArrayNothrow);
    g_oldDeleteArrayNothrow = (FNDeleteNothrow)Lkp_HookApi(L"coredll", IMPFN_DELETEARRAYNOTHROW, (PROC)newDeleteArrayNothrow);
    */
    HOOK_API_HELPER(L"coredll", IMPFN_NEWARRAYNOTHROW,      NewArrayNothrow,        FNNewNothrow);
    HOOK_API_HELPER(L"coredll", IMPFN_DELETEARRAYNOTHROW,   DeleteArrayNothrow,     FNDeleteNothrow);


    // LocalAlloc
    /*
    g_oldLocalAlloc = (FNLocalAlloc)Lkp_HookApi(L"coredll", IMPFN_LOCALALLOC, (PROC)newLocalAlloc);
    g_oldLocalFree = (FNLocalFree)Lkp_HookApi(L"coredll", IMPFN_LOCALFREE, (PROC)newLocalFree);
    g_oldLocalReAlloc = (FNLocalReAlloc)Lkp_HookApi(L"coredll", IMPFN_LOCALREALLOC, (PROC)newLocalReAlloc);
    */
    HOOK_API_HELPER(L"coredll", IMPFN_LOCALALLOC,   LocalAlloc,     FNLocalAlloc);
    HOOK_API_HELPER(L"coredll", IMPFN_LOCALFREE,    LocalFree,      FNLocalFree);
    HOOK_API_HELPER(L"coredll", IMPFN_LOCALREALLOC, LocalReAlloc,   FNLocalReAlloc);
}


//
//-----------------------------------------------------------------------------
// UnHook���е��ڴ������صĺ���
//-----------------------------------------------------------------------------
//
static void  UnHookAllMemAllocAPI ()
{
    // malloc
    /*
    Lkp_HookApi(L"coredll", IMPFN_MALLOC, (PROC)g_oldMalloc);
    Lkp_HookApi(L"coredll", IMPFN_FREE, (PROC)g_oldFree);
    Lkp_HookApi(L"coredll", IMPFN_CALLOC, (PROC)g_oldCalloc);
    Lkp_HookApi(L"coredll", IMPFN_REALLOC, (PROC)g_oldRealloc);
    */
    UNHOOK_API_HELPER(L"coredll", IMPFN_MALLOC,   Malloc);
    UNHOOK_API_HELPER(L"coredll", IMPFN_FREE,     Free);
    UNHOOK_API_HELPER(L"coredll", IMPFN_CALLOC,   Calloc);
    UNHOOK_API_HELPER(L"coredll", IMPFN_REALLOC,  Realloc);
    UNHOOK_API_HELPER(L"coredll", IMPFN_STRDUP,   Strdup);
    UNHOOK_API_HELPER(L"coredll", IMPFN_WSTRDUP,  WStrdup);
    

    // new
    /*
    Lkp_HookApi(L"coredll", IMPFN_NEW, (PROC)g_oldNew);
    Lkp_HookApi(L"coredll", IMPFN_DELETE, (PROC)g_oldDelete);
    */
    UNHOOK_API_HELPER(L"coredll", IMPFN_NEW,      New);
    UNHOOK_API_HELPER(L"coredll", IMPFN_DELETE,   Delete);


    // new[]
    /*
    Lkp_HookApi(L"coredll", IMPFN_NEWARRAY, (PROC)g_oldNewArray);
    Lkp_HookApi(L"coredll", IMPFN_DELETEARRAY, (PROC)g_oldDeleteArray);
    */
    UNHOOK_API_HELPER(L"coredll", IMPFN_NEWARRAY,     NewArray);
    UNHOOK_API_HELPER(L"coredll", IMPFN_DELETEARRAY,  DeleteArray);


    // new nothrow
    /*
    Lkp_HookApi(L"coredll", IMPFN_NEWNOTHROW, (PROC)g_oldNewNothrow);
    Lkp_HookApi(L"coredll", IMPFN_DELETENOTHROW, (PROC)g_oldDeleteNothrow);
    */
    UNHOOK_API_HELPER(L"coredll", IMPFN_NEWNOTHROW,       NewNothrow);
    UNHOOK_API_HELPER(L"coredll", IMPFN_DELETENOTHROW,    DeleteNothrow);


    // new[] nothrow
    /*
    Lkp_HookApi(L"coredll", IMPFN_NEWARRAYNOTHROW, (PROC)g_oldNewArrayNothrow);
    Lkp_HookApi(L"coredll", IMPFN_DELETEARRAYNOTHROW, (PROC)g_oldDeleteArrayNothrow);
    */
    UNHOOK_API_HELPER(L"coredll", IMPFN_NEWARRAYNOTHROW,      NewArrayNothrow);
    UNHOOK_API_HELPER(L"coredll", IMPFN_DELETEARRAYNOTHROW,   DeleteArrayNothrow);


    // LocalAlloc
    /*
    Lkp_HookApi(L"coredll", IMPFN_LOCALALLOC, (PROC)g_oldLocalAlloc);
    Lkp_HookApi(L"coredll", IMPFN_LOCALFREE, (PROC)g_oldLocalFree);
    Lkp_HookApi(L"coredll", IMPFN_LOCALREALLOC, (PROC)g_oldLocalReAlloc);
    */
    UNHOOK_API_HELPER(L"coredll", IMPFN_LOCALALLOC,   LocalAlloc);
    UNHOOK_API_HELPER(L"coredll", IMPFN_LOCALFREE,    LocalFree);
    UNHOOK_API_HELPER(L"coredll", IMPFN_LOCALREALLOC, LocalReAlloc);
}

//
//-----------------------------------------------------------------------------
// ��ʼ̽���ڴ�й¶
//-----------------------------------------------------------------------------
//
void  Lkp_StartProbeMemLeak ()
{
    INITLOCK_MEMALLOCLIST();


    HookAllMemAllocAPI();

}

//
//-----------------------------------------------------------------------------
// ����̽���ڴ�й¶, ����ӡ���ӿ�ʼ������ʱ���ڴ�����ͷŲ�ƽ�����Ŀ
//-----------------------------------------------------------------------------
//
void  Lkp_EndProbeMemLeak ()
{

    UnHookAllMemAllocAPI();


    // ��ӡ�ڴ�й¶��Ϣ
    MemAllocList_Dump(TRUE);

    UNINITLOCK_MEMALLOCLIST();
}


void  Lkp_BreakAtAllocCount (DWORD64  cnt)
{
	g_memBreakCount = cnt;
}

void  Lkp_BreakAtAllocHandle (void*  handle)
{
	g_memBreakHandle = handle;
}