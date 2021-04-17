/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SLOT_H
#define BLACKMISC_SLOT_H

#include "blackmisc/invoke.h"

#include <QPointer>
#include <QObject>
#include <QtGlobal>

#include <atomic>
#include <functional>
#include <future>
#include <memory>

namespace BlackMisc
{
    /*!
     * Wrapper around QObject::connect which disconnects after the signal has been emitted once.
     */
    template <typename T, typename U, typename F, typename G>
    QMetaObject::Connection connectOnce(T *sender, F signal, U *receiver, G &&slot, Qt::ConnectionType type = Qt::AutoConnection)
    {
        std::promise<QMetaObject::Connection> promise;
        auto called = std::make_shared<std::atomic_flag>();
        called->clear();
        auto wrapper = [receiver, called, connection = promise.get_future().share(), slot = std::forward<G>(slot)](auto &&... args)
        {
            if (called->test_and_set()) { return; }
            QObject::disconnect(connection.get());
            Private::invokeSlot(slot, receiver, std::forward<decltype(args)>(args)...);
        };
        auto connection = QObject::connect(sender, signal, receiver, std::move(wrapper), type);
        promise.set_value(connection);
        return connection;
    }

    /*!
     * Wrapper around QObject::connect which disconnects after the signal has been emitted once.
     * \note Slot has no context, so will always be a direct connection, slot will be called in sender's thread.
     */
    template <typename T, typename F, typename G>
    QMetaObject::Connection connectOnce(T *sender, F signal, G &&slot)
    {
        static_assert(! std::is_member_pointer_v<std::decay_t<G>>, "If slot is a pointer to member, a receiver must be supplied");
        return connectOnce(sender, signal, sender, std::forward<G>(slot));
    }

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
        {
            Q_ASSERT_X(object, Q_FUNC_INFO, "Need object");
        }

        //! Construct a slot from the given object passing a function
        CSlot(std::function<R(Args...)> function) :
            m_function(function)
        {}

        //! Construct a slot from the given object passing a function and a object
        template <typename T>
        CSlot(T *object, std::function<R(Args...)> function) :
            m_object(object),
            m_function(function)
        {}

        //! Call the slot. The behaviour is undefined if the slot is empty.
        R operator()(Args... args) const
        {
            Q_ASSERT_X(m_function, Q_FUNC_INFO, "Empty CSlot was called");
            return m_function(args...);
        }

        //! Call function "de-coupled" in original thread
        bool singleShot(Args... args) const
        {
            // does NOT return the values of m_function!
            if (!m_object || !m_function) { return false; }
            QTimer::singleShot(0, m_object.data(), std::bind(*this, std::forward<Args>(args)...));
            return true;
        }

        //! Returns the object which the slot belongs to.
        //! Use this as the third argument to QObject::connect to ensure the slot is called in the correct thread.
        QObject *object() const
        {
            return m_object.data();
        }

        //! Set the object which the slot belongs to.
        //! Use this as the third argument to QObject::connect to ensure the slot is called in the correct thread.
        void setObject(QObject *object)
        {
            Q_ASSERT_X(hasNullObject(), Q_FUNC_INFO, "Can only set, not change the object");
            m_object = object;
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
