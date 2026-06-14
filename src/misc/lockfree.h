// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_LOCKFREE_H
#define SWIFT_MISC_LOCKFREE_H

#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

#include <QString>
#include <QThread>
#include <QtGlobal>

// http://www.drdobbs.com/lock-free-data-structures/184401865
// http://en.cppreference.com/w/cpp/memory/shared_ptr/atomic

namespace swift::misc
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
        //! @{
        //! Return the value that was present when the reader was created.
        const T &get() const { return *m_ptr; }
        const T *operator->() const { return m_ptr.get(); }
        const T &operator*() const { return *m_ptr; }
        operator const T &() const { return *m_ptr; }
        //! @}

        //! Copy constructor.
        LockFreeReader(const LockFreeReader &) = default;

        //! Copy assignment operator.
        LockFreeReader &operator=(const LockFreeReader &) = default;

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
        //! @{
        //! The value can be modified through the returned reference. The modification is applied in the destructor.
        T &get() { return *m_ptr; }
        T *operator->() { return m_ptr.get(); }
        T &operator*() { return *m_ptr; }
        operator T &() { return *m_ptr; }
        //! @}

        //! Replace the stored value by copying from a T. The change is applied in the destructor.
        LockFreeUniqueWriter &operator=(const T &other)
        {
            *m_ptr = other;
            return *this;
        }

        //! Replace the stored value by moving from a T. The change is applied in the destructor.
        LockFreeUniqueWriter &operator=(T &&other) noexcept(std::is_nothrow_move_assignable_v<T>)
        {
            *m_ptr = std::move(other);
            return *this;
        }

        //! @{
        //! LockFreeUniqueWriter cannot be copied.
        LockFreeUniqueWriter(const LockFreeUniqueWriter &) = delete;
        LockFreeUniqueWriter &operator=(const LockFreeUniqueWriter &) = delete;
        //! @}

        //! Move constructor.
        LockFreeUniqueWriter(LockFreeUniqueWriter &&other) noexcept
            : m_old(std::move(other.m_old)), m_now(std::move(other.m_now)), m_ptr(std::move(other.m_ptr))
        {}

        //! Move assignment operator.
        LockFreeUniqueWriter &operator=(LockFreeUniqueWriter &&other) noexcept
        {
            std::tie(m_old, m_now, m_ptr) =
                std::forward_as_tuple(std::move(other.m_old), std::move(other.m_now), std::move(other.m_ptr));
            return *this;
        }

        //! Destructor. The original object will be overwritten by the new one stored in the writer.
        ~LockFreeUniqueWriter()
        {
            if (m_ptr.use_count() == 0) { return; } // *this has been moved from

#ifdef __cpp_lib_atomic_shared_ptr // atomic shared pointer is currently not supported by clang
            bool success = m_now->compare_exchange_strong(m_old, std::shared_ptr<const T>(m_ptr));
#else
            bool success = std::atomic_compare_exchange_strong(m_now, &m_old, std::shared_ptr<const T>(m_ptr));
#endif

            Q_ASSERT_X(success, Q_FUNC_INFO, "UniqueWriter detected simultaneous writes");
            Q_UNUSED(success);
        }

    private:
        friend class LockFree<T>;

#ifdef __cpp_lib_atomic_shared_ptr
        LockFreeUniqueWriter(std::shared_ptr<const T> ptr, std::atomic<std::shared_ptr<const T>> *now)
#else
        LockFreeUniqueWriter(std::shared_ptr<const T> ptr, std::shared_ptr<const T> *now)
#endif
            : m_old(ptr), m_now(now), m_ptr(std::make_shared<T>(*m_old))
        {}
        std::shared_ptr<const T> m_old;
#ifdef __cpp_lib_atomic_shared_ptr
        std::atomic<std::shared_ptr<const T>> *m_now;
#else
        std::shared_ptr<const T> *m_now;
#endif
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
        LockFree(T &&other) noexcept(std::is_nothrow_move_assignable_v<T>)
            : m_ptr(std::make_shared<const T>(std::move(other)))
        {}

        //! @{
        //! LockFree cannot be copied or moved.
        LockFree(const LockFree &) = delete;
        LockFree &operator=(const LockFree &) = delete;
        LockFree(LockFree &&) = delete;
        LockFree &operator=(LockFree &&) = delete;
        //! @}

        //! Return an object which can read the current value.
#ifdef __cpp_lib_atomic_shared_ptr
        LockFreeReader<const T> read() const { return { m_ptr.load() }; }
#else
        LockFreeReader<const T> read() const { return { std::atomic_load(&m_ptr) }; }
#endif

        //! Return an object which can write a new value, as long as there are no other writes.
#ifdef __cpp_lib_atomic_shared_ptr
        LockFreeUniqueWriter<T> uniqueWrite() { return { m_ptr.load(), &m_ptr }; }
#else
        LockFreeUniqueWriter<T> uniqueWrite() { return { std::atomic_load(&m_ptr), &m_ptr }; }
#endif

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

    private:
#ifdef __cpp_lib_atomic_shared_ptr
        std::atomic<std::shared_ptr<const T>> m_ptr = std::make_shared<const T>();
#else
        std::shared_ptr<const T> m_ptr = std::make_shared<const T>();
#endif
    };

    /*!
     * Compose multiple LockFreeReader or LockFreeUniqueWriter instances.
     */
    template <template <typename> class T, typename... Ts>
    class LockFreeMulti
    {
    public:
        //! Construct from a forwarded tuple. Prefer to construct via swift::misc::multiRead or
        //! swift::misc::multiUniqueWrite.
        LockFreeMulti(std::tuple<T<Ts> &&...> &&tup) : m_tup(std::move(tup)) {}

        //! Function call operator.
        //! \param function The LockFree values from which this LockFreeMulti was constructed will be passed as
        //! arguments to this functor. \return The value returned by the functor, if any.
        template <typename F>
        auto operator()(F &&function) &&
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
    LockFreeMulti<LockFreeReader, const Ts...> multiRead(const LockFree<Ts> &...vs)
    {
        return { std::forward_as_tuple(vs.read()...) };
    }

    /*!
     * Return a callable object for writing to multiple LockFree instances simultaneously.
     */
    template <typename... Ts>
    LockFreeMulti<LockFreeUniqueWriter, Ts...> multiUniqueWrite(LockFree<Ts> &...vs)
    {
        return { std::forward_as_tuple(vs.uniqueWrite()...) };
    }

    //! @{
    /*!
     * Non-member begin() and end() for so LockFree containers can be used in ranged for loops.
     */
    template <typename T>
    typename T::const_iterator begin(const LockFreeReader<T> &reader)
    {
        return reader->begin();
    }

    template <typename T>
    typename T::const_iterator end(const LockFreeReader<T> &reader)
    {
        return reader->end();
    }

    template <typename T>
    typename T::iterator begin(const LockFreeUniqueWriter<T> &writer)
    {
        return writer->begin();
    }

    template <typename T>
    typename T::iterator end(const LockFreeUniqueWriter<T> &writer)
    {
        return writer->end();
    }
    //! @}

    //! @{
    /*!
     * Deleted overloads of begin() and end() for rvalue readers and writers.
     *
     * Attempting to call begin() or end() on an rvalue reader or writer would be a common source of mistakes.
     */
    template <typename T>
    typename T::const_iterator begin(const LockFreeReader<T> &&) = delete;

    template <typename T>
    typename T::const_iterator end(const LockFreeReader<T> &&) = delete;

    template <typename T>
    typename T::iterator begin(const LockFreeUniqueWriter<T> &&) = delete;

    template <typename T>
    typename T::iterator end(const LockFreeUniqueWriter<T> &&) = delete;
    //! @}

} // namespace swift::misc

#endif // SWIFT_MISC_LOCKFREE_H
