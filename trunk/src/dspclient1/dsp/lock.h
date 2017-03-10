#ifndef _LOCK_H_
#define _LOCK_H_
#include <WinBase.h>
/**
*  File:     lock
*  Date:     2012/03/01
*  Author:   chao
*  Describe: 主要用于自动化加锁解锁，增加程序异常安全,减少操作
*/

// This class implements the underlying platform-specific spin-lock mechanism
// used for the Lock class.  Most users should not use LockImpl directly, but
// should instead use Lock.
class LockImpl {
public:

    LockImpl();
    ~LockImpl();

    // If the lock is not held, take it and return true.  If the lock is already
    // held by something else, immediately return false.
    bool Try();

    // Take the lock, blocking until it is available if necessary.
    void Lock();

    // Release the lock.  This must only be called by the lock's holder: after
    // a successful call to Try, or a call to Lock.
    void Unlock();

private:
    CRITICAL_SECTION os_lock_;

    LockImpl(LockImpl&);
    void operator=(LockImpl&);
};


// manage lockimpl
class  Lock {
public:
    Lock() : lock_() {}
    ~Lock() {}
    void Acquire() { lock_.Lock(); }
    void Release() { lock_.Unlock(); }

    // If the lock is not held, take it and return true. If the lock is already
    // held by another thread, immediately return false. This must not be called
    // by a thread already holding the lock (what happens is undefined and an
    // assertion may fail).
    bool Try() { return lock_.Try(); }
private:
    Lock(Lock&);
    void operator=(Lock&);

    // lock implementation.
    LockImpl lock_;
};

// A helper class that acquires the given Lock while the AutoLock is in scope.
class AutoLock {
public:
    explicit AutoLock(Lock& lock) : lock_(lock) {
        lock_.Acquire();
    }

    ~AutoLock() {
        lock_.Release();
    }

private:
    AutoLock(AutoLock&);
    void operator=(AutoLock&);
    Lock& lock_;
};

// AutoUnlock is a helper that will Release() the |lock| argument in the
// constructor, 
class AutoUnlock {
public:
    explicit AutoUnlock(Lock& lock) : lock_(lock) {
        lock_.Release();
    }

    ~AutoUnlock() {
        lock_.Acquire();
    }

private:
    AutoUnlock(AutoUnlock&);
    void operator=(AutoUnlock&);
    Lock& lock_;
};

#endif
