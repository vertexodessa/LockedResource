#include "lockableResource.h"

bool CCountingMutex::tryLock (int timeout) QT_MUTEX_LOCK_NOEXCEPT {
    bool locked = m_mutex.tryLock(timeout);
    if (locked) {
        m_lockCount.fetchAndAddRelease(1);
    }
    return locked;
}

void CCountingMutex::unlock() QT_MUTEX_LOCK_NOEXCEPT {
    m_mutex.unlock();
    m_lockCount.fetchAndAddRelease(-1);
}

void CCountingMutex::lock() QT_MUTEX_LOCK_NOEXCEPT {
    m_mutex.lock();
    m_lockCount.fetchAndAddRelease(1);
}


