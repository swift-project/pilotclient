/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
        Optional(T value) : m_isValid(true) { new (m_bytes) T(std::move(value)); }

        //! Construct from a nullptr, equivalent to default constructor.
        Optional(std::nullptr_t) : m_isValid(false) {}

        //! Copy constructor.
        Optional(const Optional &other) : m_isValid(other.m_isValid)
        {
            if (other.m_isValid) { new (m_bytes) T(*other); }
        }

        //! Move constructor.
        Optional(Optional &&other) : m_isValid(other.m_isValid)
        {
            if (other.m_isValid) { new (m_bytes) T(std::move(*other)); }
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
            if (other.m_isValid) { new (m_bytes) T(*other); }
            m_isValid = other.m_isValid;
            return *this;
        }

        //! Move assignment.
        Optional &operator =(Optional &&other)
        {
            if (m_isValid) { (*this)->~T(); }
            if (other.m_isValid) { new (m_bytes) T(std::move(*other)); }
            m_isValid = other.m_isValid;
            return *this;
        }

        //! Destructor.
        ~Optional() { if (m_isValid) { (*this)->~T(); } }

        //! Explicit cast to bool, true if this Optional contains a value.
        explicit operator bool() const { return m_isValid; }

        //! Dereference operator, returns reference to contained value, undefined if there is no value contained.
        T &operator *() { Q_ASSERT(m_isValid); return *reinterpret_cast<T *>(m_bytes); }

        //! Dereference operator, returns reference to contained value, undefined if there is no value contained.
        const T &operator *() const { Q_ASSERT(m_isValid); return *reinterpret_cast<const T *>(m_bytes); }

        //! Indirection operator, returns pointer to contained value, undefined if there is no value contained.
        T *operator ->() { Q_ASSERT(m_isValid); return reinterpret_cast<T *>(m_bytes); }

        //! Indirection operator, returns pointer to contained value, undefined if there is no value contained.
        const T *operator ->() const { Q_ASSERT(m_isValid); return reinterpret_cast<const T *>(m_bytes); }

    private:
        bool m_isValid;
        char m_bytes[sizeof(T)];
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
