/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_LOCKFREE_H
#define BLACKMISC_LOCKFREE_H

#include "blackmisc/blackmiscexport.h"

#include <stddef.h>
#include <QMetaType>
#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include <QThread>
#include <QtGlobal>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

// http://www.drdobbs.com/lock-free-data-structures/184401865
// http://en.cppreference.com/w/cpp/memory/shared_ptr/atomic

namespace BlackMisc
{

    template <typename>
    class LockFree;

    /*!
     * Return value of LockFree::read(). Allows any one thread to safely read from the lock-free object.
     */
    template <typename T>
    class LockFreeReader
    {
    public:
        //! Return the value that was present when the reader was created.
        //! @{
        const T &get() const { return *m_ptr; }
        const T *operator ->() const { return m_ptr.get(); }
        const T &operator *() const { return *m_ptr; }
        operator const T &() const { return *m_ptr; }
        //! @}

        //! Copy constructor.
        LockFreeReader(const LockFreeReader &) = default;

        //! Copy assignment operator.
        LockFreeReader &operator =(const LockFreeReader &) = default;

    private:
        friend class LockFree<std::remove_const_t<T>>;
    
        LockFreeReader(std::shared_ptr<const T> ptr) : m_ptr(ptr) {}
        std::shared_ptr<const T> m_ptr;
    };

    /*!
     * Return value of LockFree::uniqueWrite(). Allows any one thread to safely write to the lock-free object,
     * as long as no other thread write to it.
     */
    template <typename T>
    class LockFreeUniqueWriter
    {
    public:
        //! The value can be modified through the returned reference. The modification is applied in the destructor.
        //! @{
        T &get() { return *m_ptr; }
        T *operator ->() { return m_ptr.get(); }
        T &operator *() { return *m_ptr; }
        operator T &() { return *m_ptr; }
        //! @}

        //! Replace the stored value by copying from a T. The change is applied in the destructor.
        LockFreeUniqueWriter &operator =(const T &other) { *m_ptr = other; return *this; }

        //! Replace the stored value by moving from a T. The change is applied in the destructor.
        LockFreeUniqueWriter &operator =(T &&other) noexcept(std::is_nothrow_move_assignable_v<T>) { *m_ptr = std::move(other); return *this; }

        //! LockFreeUniqueWriter cannot be copied.
        //! @{
        LockFreeUniqueWriter(const LockFreeUniqueWriter &) = delete;
        LockFreeUniqueWriter &operator =(const LockFreeUniqueWriter &) = delete;
        //! @}

        //! Move constructor.
        LockFreeUniqueWriter(LockFreeUniqueWriter &&other) noexcept : m_old(std::move(other.m_old)), m_now(std::move(other.m_now)), m_ptr(std::move(other.m_ptr)) {}

        //! Move assignment operator.
        LockFreeUniqueWriter &operator =(LockFreeUniqueWriter &&other) noexcept
        {
            std::tie(m_old, m_now, m_ptr) = std::forward_as_tuple(std::move(other.m_old), std::move(other.m_now), std::move(other.m_ptr));
            return *this;
        }

        //! Destructor. The original object will be overwritten by the new one stored in the writer.
        ~LockFreeUniqueWriter()
        {
            if (m_ptr.use_count() == 0) { return; } // *this has been moved from
            bool success = std::atomic_compare_exchange_strong(m_now, &m_old, std::shared_ptr<const T>(m_ptr));
            Q_ASSERT_X(success, Q_FUNC_INFO, "UniqueWriter detected simultaneous writes");
            Q_UNUSED(success);
        }

    private:
        friend class LockFree<T>;
    
        LockFreeUniqueWriter(std::shared_ptr<const T> ptr, std::shared_ptr<const T> *now) : m_old(ptr), m_now(now), m_ptr(std::make_shared<T>(*m_old)) {}
        std::shared_ptr<const T> m_old;
        std::shared_ptr<const T> *m_now;
        std::shared_ptr<T> m_ptr;
    };

    /*!
     * Return value of LockFree::sharedWrite(). Allows any one thread to safely write to the lock-free object.
     */
    template <typename T>
    class LockFreeSharedWriter
    {
    public:
        //! The value can be modified through the returned reference. The modification is applied by evaluating in a bool context.
        //! @{
        T &get() { return *m_ptr; }
        T *operator ->() { return m_ptr.get(); }
        T &operator *() { return *m_ptr; }
        operator T &() { return *m_ptr; }
        //! @}

        //! Replace the stored value by copying from a T. The change is applied by evaluating in a bool context.
        LockFreeSharedWriter &operator =(const T &other) { *m_ptr = other; return *this; }

        //! Replace the stored value by moving from a T. The change is applied by evaluating in a bool context.
        LockFreeSharedWriter &operator =(T &&other) noexcept(std::is_nothrow_move_assignable_v<T>) { *m_ptr = std::move(other); return *this; }

        //! Try to overwrite the original object with the new one stored in the writer, and return false on success.
        //! If true is returned, then the caller must try again. This would happen if another simultaneous write had occurred.
        bool operator !() { return ! operator bool(); }

        //! Try to overwrite the original object with the new one stored in the writer, and return true on success.
        //! If false is returned, then the caller must try again. This would happen if another simultaneous write had occurred.
        operator bool()
        {
            Q_ASSERT_X(m_ptr.use_count() > 0, Q_FUNC_INFO, "SharedWriter tried to commit changes twice");
            if (std::atomic_compare_exchange_strong(m_now, &m_old, std::shared_ptr<const T>(m_ptr)))
            {
                m_ptr.reset();
                return true;
            }
            QThread::msleep(1);
            m_old = std::atomic_load(m_now);
            m_ptr = std::make_shared<T>(*m_old);
            return false;
        }

        //! Destructor. The writer's changes must be committed before this is called.
        ~LockFreeSharedWriter()
        {
            Q_ASSERT_X(m_ptr.use_count() == 0, Q_FUNC_INFO, "SharedWriter destroyed without committing changes");
        }

        //! LockFreeSharedWriter cannot be copied.
        //! @{
        LockFreeSharedWriter(const LockFreeSharedWriter &) = delete;
        LockFreeSharedWriter &operator =(const LockFreeSharedWriter &) = delete;
        //! @}

        //! Move constructor.
        LockFreeSharedWriter(LockFreeSharedWriter &&other) noexcept : m_old(std::move(other.m_old)), m_now(std::move(other.m_now)), m_ptr(std::move(other.m_ptr)) {}

        //! Move assignment operator.
        LockFreeSharedWriter &operator =(LockFreeSharedWriter &&other) noexcept
        {
            std::tie(m_old, m_now, m_ptr) = std::forward_as_tuple(std::move(other.m_old), std::move(other.m_now), std::move(other.m_ptr));
            return *this;
        }

    private:
        friend class LockFree<T>;
    
        LockFreeSharedWriter(std::shared_ptr<const T> ptr, std::shared_ptr<const T> *now) : m_old(ptr), m_now(now), m_ptr(std::make_shared<T>(*m_old)) {}
        std::shared_ptr<const T> m_old;
        std::shared_ptr<const T> *m_now;
        std::shared_ptr<T> m_ptr;
    };

    /*!
     * Lock-free wrapper for synchronizing multi-threaded access to an object.
     *
     * Implemented using atomic operations of std::shared_ptr.
     */
    template <typename T>
    class LockFree
    {
    public:
        //! Default constructor. Object will contain a default-constructed T.
        LockFree() = default;

        //! Construct by copying from a T.
        LockFree(const T &other) : m_ptr(std::make_shared<const T>(other)) {}

        //! Construct by moving from a T.
        LockFree(T &&other) noexcept(std::is_nothrow_move_assignable_v<T>) : m_ptr(std::make_shared<const T>(std::move(other))) {}

        //! LockFree cannot be copied or moved.
        //! @{
        LockFree(const LockFree &) = delete;
        LockFree &operator =(const LockFree &) = delete;
        LockFree(LockFree &&) = delete;
        LockFree &operator =(LockFree &&) = delete;
        //! @}

        //! Return an object which can read the current value.
        LockFreeReader<const T> read() const
        {
            return { std::atomic_load(&m_ptr) };
        }

        //! Return an object which can write a new value, as long as there are no other writes.
        LockFreeUniqueWriter<T> uniqueWrite()
        {
            return { std::atomic_load(&m_ptr), &m_ptr };
        }

        //! Return an object which can write a new value, even if there are other writes.
        LockFreeSharedWriter<T> sharedWrite()
        {
            return { std::atomic_load(&m_ptr), &m_ptr };
        }

        //! Pass the current value to the functor inspector, and return whatever inspector returns.
        template <typename F>
        auto read(F &&inspector)
        {
            return std::forward<F>(inspector)(read().get());
        }

        //! Pass a modifiable reference to the functor mutator. Unsafe if there are multiple writers.
        template <typename F>
        void uniqueWrite(F &&mutator)
        {
            std::forward<F>(mutator)(uniqueWrite().get());
        }

        //! Pass a modifiable reference to the functor mutator. Safe if there are multiple writers.
        //! The mutator may be called multiple times.
        template <typename F>
        void sharedWrite(F &&mutator)
        {
            auto writer = sharedWrite();
            do { std::forward<F>(mutator)(writer.get()); } while (! writer);
        }

    private:
        std::shared_ptr<const T> m_ptr = std::make_shared<const T>();
    };

    /*!
     * Compose multiple LockFreeReader or LockFreeUniqueWriter instances.
     */
    template <template <typename> class T, typename... Ts>
    class LockFreeMulti
    {
    public:
        //! Construct from a forwarded tuple. Prefer to construct via BlackMisc::multiRead or BlackMisc::multiUniqueWrite.
        LockFreeMulti(std::tuple<T<Ts> &&...> &&tup) : m_tup(std::move(tup)) {}

        //! Function call operator.
        //! \param function The LockFree values from which this LockFreeMulti was constructed will be passed as arguments to this functor.
        //! \return The value returned by the functor, if any.
        template <typename F>
        auto operator ()(F &&function) &&
        {
            return call(std::forward<F>(function), std::make_index_sequence<sizeof...(Ts)>());
        }

    private:
        template <typename F, size_t... Is>
        auto call(F &&function, std::index_sequence<Is...>)
        {
            return std::forward<F>(function)(std::get<Is>(m_tup).get()...);
        }

        const std::tuple<T<Ts>...> m_tup;
    };

    /*!
     * Return a callable object for reading from multiple LockFree instances simultaneously.
     */
    template <typename... Ts>
    LockFreeMulti<LockFreeReader, const Ts...> multiRead(const LockFree<Ts> &... vs)
    {
        return { std::forward_as_tuple(vs.read()...) };
    }

    /*!
     * Return a callable object for writing to multiple LockFree instances simultaneously.
     */
    template <typename... Ts>
    LockFreeMulti<LockFreeUniqueWriter, Ts...> multiUniqueWrite(LockFree<Ts> &... vs)
    {
        return { std::forward_as_tuple(vs.uniqueWrite()...) };
    }

    /*!
     * Non-member begin() and end() for so LockFree containers can be used in ranged for loops.
     */
    //! @{
    template <typename T>
    typename T::const_iterator begin(const LockFreeReader<T> &reader) { return reader->begin();}

    template <typename T>
    typename T::const_iterator end(const LockFreeReader<T> &reader) { return reader->end(); }

    template <typename T>
    typename T::iterator begin(const LockFreeUniqueWriter<T> &writer) { return writer->begin(); }

    template <typename T>
    typename T::iterator end(const LockFreeUniqueWriter<T> &writer) { return writer->end(); }

    template <typename T>
    typename T::iterator begin(const LockFreeSharedWriter<T> &writer) { return writer->begin(); }

    template <typename T>
    typename T::iterator end(const LockFreeSharedWriter<T> &writer) { return writer->end(); }
    //! @}

    /*!
     * Deleted overloads of begin() and end() for rvalue readers and writers.
     *
     * Attempting to call begin() or end() on an rvalue reader or writer would be a common source of mistakes.
     */
    //! @{
    template <typename T>
    typename T::const_iterator begin(const LockFreeReader<T> &&) = delete;

    template <typename T>
    typename T::const_iterator end(const LockFreeReader<T> &&) = delete;

    template <typename T>
    typename T::iterator begin(const LockFreeUniqueWriter<T> &&) = delete;

    template <typename T>
    typename T::iterator end(const LockFreeUniqueWriter<T> &&) = delete;

    template <typename T>
    typename T::iterator begin(const LockFreeSharedWriter<T> &&) = delete;

    template <typename T>
    typename T::iterator end(const LockFreeSharedWriter<T> &&) = delete;
    //! @}

}

#endif
