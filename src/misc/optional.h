// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_OPTIONAL_H
#define SWIFT_MISC_OPTIONAL_H

#include <utility>

#include <QtGlobal>

namespace swift::misc
{

    /*!
     * Own implementation of std::optional.
     *
     * Needed to work around lack of C++20 copyable lambda functions.
     */
    template <typename T>
    class Optional
    {
    public:
        //! Default constructor.
        Optional() noexcept = default;

        //! Construct from a value.
        Optional(T value) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            new (m_data.bytes) T(std::move(value));
            m_isValid = true;
        }

        //! Construct from a nullptr, equivalent to default constructor.
        Optional(std::nullptr_t) noexcept {}

        //! Copy constructor.
        Optional(const Optional &other) noexcept(std::is_nothrow_copy_constructible_v<T>)
        {
            if (other.m_isValid) { new (m_data.bytes) T(*other); }
            m_isValid = other.m_isValid;
        }

        //! Move constructor.
        Optional(Optional &&other) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            if (other.m_isValid) { new (m_data.bytes) T(std::move(*other)); }
            m_isValid = other.m_isValid;
        }

        //! Assign a nullptr.
        Optional &operator=(std::nullptr_t) noexcept
        {
            reset();
            return *this;
        }

        //! Copy assignment.
        Optional &operator=(const Optional &other) noexcept(std::is_nothrow_copy_constructible_v<T>)
        {
            reset();
            if (other.m_isValid) { new (m_data.bytes) T(*other); }
            m_isValid = other.m_isValid;
            return *this;
        }

        //! Move assignment.
        Optional &operator=(Optional &&other) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            reset();
            if (other.m_isValid) { new (m_data.bytes) T(std::move(*other)); }
            m_isValid = other.m_isValid;
            return *this;
        }

        //! Destructor.
        ~Optional()
        {
            if (m_isValid) { (*this)->~T(); }
        }

        //! Explicit cast to bool, true if this Optional contains a value.
        explicit operator bool() const noexcept { return m_isValid; }

        //! If object is valid, destroy to make it invalid.
        void reset() noexcept
        {
            if (m_isValid) { (*this)->~T(); }
            m_isValid = false;
        }

        //! Dereference operator, returns reference to contained value, undefined if there is no value contained.
        T &operator*() { return dereference(); }

        //! Dereference operator, returns reference to contained value, undefined if there is no value contained.
        const T &operator*() const { return dereference(); }

        //! Indirection operator, returns pointer to contained value, undefined if there is no value contained.
        T *operator->() { return &dereference(); }

        //! Indirection operator, returns pointer to contained value, undefined if there is no value contained.
        const T *operator->() const { return &dereference(); }

    private:
        bool m_isValid = false;

        T &dereference()
        {
            Q_ASSERT(m_isValid);
            return m_data.object;
        }
        const T &dereference() const
        {
            Q_ASSERT(m_isValid);
            return m_data.object;
        }
        union Data
        {
            Data() {}
            ~Data() {}
            char bytes[sizeof(T)];
            T object;
        };
        Data m_data;
    };

    /*!
     * Efficient swap for two Optional objects.
     */
    template <typename T>
    void swap(Optional<T> &a, Optional<T> &b) noexcept(std::is_nothrow_swappable_v<T>)
    {
        if (a)
        {
            if (b)
            {
                using std::swap;
                swap(*a, *b);
            }
            else
            {
                b = std::move(a);
                a = nullptr;
            }
        }
        else if (b)
        {
            a = std::move(b);
            b = nullptr;
        }
    }

} // namespace swift::misc

#endif // SWIFT_MISC_OPTIONAL_H
