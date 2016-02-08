/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_OPTIONAL_H
#define BLACKMISC_OPTIONAL_H

#include <QtGlobal>
#include <utility>

namespace BlackMisc
{

    /*!
     * Class which can directly contain zero or one object of type T, with pointer semantics.
     */
    template <typename T>
    class Optional
    {
    public:
        //! Default constructor.
        Optional() : m_isValid(false) {}

        //! Construct from a value.
        Optional(T value) : m_isValid(true) { new (m_data.bytes) T(std::move(value)); }

        //! Construct from a nullptr, equivalent to default constructor.
        Optional(std::nullptr_t) : m_isValid(false) {}

        //! Copy constructor.
        Optional(const Optional &other) : m_isValid(other.m_isValid)
        {
            if (other.m_isValid) { new (m_data.bytes) T(*other); }
        }

        //! Move constructor.
        Optional(Optional &&other) : m_isValid(other.m_isValid)
        {
            if (other.m_isValid) { new (m_data.bytes) T(std::move(*other)); }
        }

        //! Assign a nullptr.
        Optional &operator =(std::nullptr_t)
        {
            if (m_isValid) { (*this)->~T(); }
            m_isValid = false;
            return *this;
        }

        //! Copy assignment.
        Optional &operator =(const Optional &other)
        {
            if (m_isValid) { (*this)->~T(); }
            if (other.m_isValid) { new (m_data.bytes) T(*other); }
            m_isValid = other.m_isValid;
            return *this;
        }

        //! Move assignment.
        Optional &operator =(Optional &&other)
        {
            if (m_isValid) { (*this)->~T(); }
            if (other.m_isValid) { new (m_data.bytes) T(std::move(*other)); }
            m_isValid = other.m_isValid;
            return *this;
        }

        //! Destructor.
        ~Optional() { if (m_isValid) { (*this)->~T(); } }

        //! Explicit cast to bool, true if this Optional contains a value.
        explicit operator bool() const { return m_isValid; }

        //! Dereference operator, returns reference to contained value, undefined if there is no value contained.
        T &operator *() { return dereference(); }

        //! Dereference operator, returns reference to contained value, undefined if there is no value contained.
        const T &operator *() const { return dereference(); }

        //! Indirection operator, returns pointer to contained value, undefined if there is no value contained.
        T *operator ->() { return &dereference(); }

        //! Indirection operator, returns pointer to contained value, undefined if there is no value contained.
        const T *operator ->() const { return &dereference(); }

    private:
        bool m_isValid;

#if defined(Q_COMPILER_UNRESTRICTED_UNIONS)
        T &dereference() { Q_ASSERT(m_isValid); return m_data.object; }
        const T &dereference() const { Q_ASSERT(m_isValid); return m_data.object; }
        union Data
        {
            Data() {}
            ~Data() {}
            char bytes[sizeof(T)];
            T object;
        };
        Data m_data;
#else
        T &dereference() { Q_ASSERT(m_isValid); return *reinterpret_cast<T *>(m_data.bytes); }
        const T &dereference() const { Q_ASSERT(m_isValid); return *reinterpret_cast<const T *>(m_data.bytes); }
        struct { typename std::aligned_storage<sizeof(T)>::type bytes[1]; } m_data;
#endif
    };

    /*!
     * Efficient swap for two Optional objects.
     */
    template <typename T>
    void swap(Optional<T> &a, Optional<T> &b)
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

} //namespace BlackMisc

#endif // guard
