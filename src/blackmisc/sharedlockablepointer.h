/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_SHAREDLOCKABKEPTR_H
#define BLACKMISC_SHAREDLOCKABKEPTR_H

#include <QSharedPointer>
#include <QMutex>

//! \file

namespace BlackMisc
{
    /*! Wrapper container for shared lockable pointer.
     *  Provides BasicLockable characteristics to be used
     *  with std::lock_guard
     */
    template<class T>
    class SharedLockablePtr
    {
    public:

        //! Constructor
        SharedLockablePtr(T *ptr)
            : m_ptr(ptr), m_mutex(new QMutex(QMutex::Recursive))
        {
        }

        //! Constructor with custom deleter
        template <typename Deleter>
        SharedLockablePtr(T *ptr, Deleter deleter)
            : m_ptr(ptr, deleter), m_mutex(new QMutex(QMutex::Recursive))
        {
        }

        //! Copy constructor
        SharedLockablePtr(SharedLockablePtr const &other)
            : m_ptr(other.m_ptr), m_mutex(other.m_mutex)
        {
        }

        //! Move constructor
        SharedLockablePtr(SharedLockablePtr &&other)
            : m_ptr(std::move(other.m_ptr)), m_mutex(std::move(other.m_mutex))
        {
        }

        //! Destructor
        ~SharedLockablePtr()
        {
        }

        //! Assignment operator
        SharedLockablePtr &operator =(const SharedLockablePtr &other)
        {
            m_ptr = other.m_ptr;
            m_mutex = other.m_mutex;
            return *this;
        }

        //! Move assignment operator
        SharedLockablePtr &operator =(SharedLockablePtr &&other)
        {
            // QSharedPointer doesn't provide a non-member swap
            m_ptr.swap(other.m_ptr);
            m_mutex.swap(other.m_mutex);
            return *this;
        }

        //! Provides access to shared pointer member
        T *operator ->()
        {
            return m_ptr.operator ->();
        }

        //! Provides const access to shared pointer member
        T const *operator ->() const
        {
            return m_ptr.operator ->();
        }

        //! Unary ! operator
        bool operator !()
        {
            return m_ptr.operator !();
        }

        //! Implicit conversion to bool
        operator bool() const
        {
            return m_ptr.operator bool();
        }

        //! Locks the internal mutex. Compatible with std::lock_guard
        void lock()
        {
            m_mutex->lock();
        }

        //! Unlocks the internal mutex. Compatible with std::lock_guard
        void unlock()
        {
            m_mutex->unlock();
        }

    private:

        QSharedPointer<T> m_ptr;
        QSharedPointer<QMutex> m_mutex;
    };

} // namespace BlackMisc

#endif // guard
