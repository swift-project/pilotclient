/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SLOT_H
#define BLACKMISC_SLOT_H

#include <QPointer>
#include <QObject>
#include <QtGlobal>
#include <functional>

namespace BlackMisc
{
    /*!
     * Callable wrapper for a member function with function signature F. General template, not
     * implemented; the partial specialization for function signatures is what does the actual work
     * (similar to how std::function is defined).
     */
    template <typename F>
    class CSlot;

    /*!
     * Callable wrapper for a member function with function signature R(Args...).
     * Allows for easy manipulation of slots to be used as callbacks.
     */
    template <typename R, typename... Args>
    class CSlot<R(Args...)>
    {
    public:
        //! Construct an empty slot.
        CSlot() {}

        //! Construct a slot from the given member function of the given object.
        template <typename T, typename U>
        CSlot(T *object, R(U::* function)(Args...)) :
            m_object(object),
            m_function([ = ](Args... args) { return (object->*function)(args...); })
        {}

        //! Call the slot. The behaviour is undefined if the slot is empty.
        R operator()(Args... args) const
        {
            Q_ASSERT_X(m_function, Q_FUNC_INFO, "Empty CSlot was called");
            return m_function(args...);
        }

        //! Returns the object which the slot belongs to.
        //! Use this as the third argument to QObject::connect to ensure the slot is called in the correct thread.
        QObject *object() const
        {
            return m_object.data();
        }

        //! True if the slot can be called, false if it is empty.
        operator bool() const
        {
            return !this->isEmpty() && !this->hasNullObject();
        }

        //! True if the slot is empty or object is null, false if it can be called.
        bool operator !() const
        {
            return this->isEmpty() || this->hasNullObject();
        }

        //! True if the slot is empty, false if it can be called.
        bool isEmpty() const
        {
            return ! m_function;
        }

        //! True if the object is null
        bool hasNullObject() const
        {
            return m_object.isNull();
        }

    private:
        QPointer<QObject> m_object;
        std::function<R(Args...)> m_function;
    };
}

#endif
