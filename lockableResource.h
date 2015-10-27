#ifndef CLOCKABLERESOURSE_H
#define CLOCKABLERESOURCE_H


#include <QMutex>
#include <QAtomicInt>

#include <iostream>

/******************************************************************************
 * CMutexLocker:
 * auto-locker, helps to lock the class scoped.
 *
 * COwnedLockableResource:
 * Owned lockable resource idiom.
 * The class is a container for the resource AND
 * provides interface to lock and unlock the mutex.
 * Also assertion is implemented on wrong mutex usage, and scoped lock idiom.
 *
 */

#ifndef NDEBUG
inline void DEBUG_P (std::string s) {
    std::cout << s << std::endl;
}
#else
#define DEBUG_P
#endif


class CCountingMutex {
public:
    explicit CCountingMutex( QMutex::RecursionMode mode = QMutex::NonRecursive ) : m_mutex(mode) {};
    ~CCountingMutex(){};

    void lock()                     QT_MUTEX_LOCK_NOEXCEPT;
    bool tryLock(int timeout = 0)   QT_MUTEX_LOCK_NOEXCEPT;
    void unlock()                   Q_DECL_NOTHROW;

    // new methods
    bool isLocked() const { return m_lockCount.loadAcquire() > 0; };
private:
    QMutex m_mutex;
    QAtomicInt m_lockCount;
};



template<class T>
class CMutexLockerHelper {
public:
    explicit CMutexLockerHelper (T& res) : m_mutex(res) { DEBUG_P (__PRETTY_FUNCTION__); m_mutex.lock(); };
    ~CMutexLockerHelper() { DEBUG_P (__PRETTY_FUNCTION__); m_mutex.unlock(); };
private:
    T& m_mutex;
};



template<class T>
class COwnedLockableResource {
public:
    enum RecursionMode { NonRecursive, Recursive };

    COwnedLockableResource( T* resource, RecursionMode mode = Recursive );
    ~COwnedLockableResource();

    void lock() QT_MUTEX_LOCK_NOEXCEPT { m_mutex.lock(); };
    bool tryLock(int timeout = 0) QT_MUTEX_LOCK_NOEXCEPT { return m_mutex.tryLock(); };
    void unlock() Q_DECL_NOTHROW { m_mutex.unlock(); };

    CMutexLockerHelper< COwnedLockableResource<T> > lockScoped () {
        return CMutexLockerHelper< COwnedLockableResource<T> >(*this);
    }

    T* get();
    T* operator ->() { return get(); }
private:
    T* m_res;
    CCountingMutex m_mutex;

    QMutex::RecursionMode convertEnum ( RecursionMode m ) {
        switch (m) {
            case Recursive:
                return QMutex::Recursive;
            case NonRecursive:
                return QMutex::NonRecursive;
        }

        // not reached
        return QMutex::Recursive;
    }
};



template <class T>
CMutexLockerHelper<T> CMutexLocker (T& mutex){
	return CMutexLockerHelper<T>(mutex);
}



template <class T>
COwnedLockableResource<T>::~COwnedLockableResource() {
    Q_ASSERT(!m_mutex.isLocked());
    delete m_res;
}

template <class T>
COwnedLockableResource<T>::COwnedLockableResource(T* resource, RecursionMode mode) :
    m_res(resource),
    m_mutex(convertEnum(mode)) {

}

template <class T>
T* COwnedLockableResource< T >::get() {
        Q_ASSERT(m_mutex.isLocked());
        return m_res;
}

#endif
