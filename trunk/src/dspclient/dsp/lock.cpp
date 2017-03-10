#include "stdafx.h"
#include "lock.h"

//////////////////////////////////////////////////////////////////////////

LockImpl::LockImpl() {
    InitializeCriticalSection(&os_lock_);
}

LockImpl::~LockImpl() {
    DeleteCriticalSection(&os_lock_);
}

bool LockImpl::Try() {
    if (TryEnterCriticalSection(&os_lock_) != FALSE) {
        return true;
    }
    return false;
}

void LockImpl::Lock() {
    EnterCriticalSection(&os_lock_);
}

void LockImpl::Unlock() {
    LeaveCriticalSection(&os_lock_);
}

//////////////////////////////////////////////////////////////////////////