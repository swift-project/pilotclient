/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_GENERICDBUSINTERFACE_H
#define BLACKMISC_GENERICDBUSINTERFACE_H

#include <QDBusAbstractInterface>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QObject>
#include <QMetaMethod>

#ifndef Q_MOC_RUN
/*!
 * Any signals tagged with this macro will be ignored by BlackMisc::CGenericDBusInterface::relayParentSignals().
 * \see QMetaMethod::tag
 */
#define BLACK_NO_RELAY
#endif

namespace BlackMisc
{
    /*!
     * Used for hand written interface based on virtual methods.
     * Allows to relay a message to DBus in a single code line
     */
    class CGenericDBusInterface : public QDBusAbstractInterface
    {
    public:
        //! Constructor
        CGenericDBusInterface(const QString &serviceName, const QString &path, const QString &interfaceName, const QDBusConnection &connection, QObject *parent = 0) :
            QDBusAbstractInterface(serviceName, path, interfaceName.toUtf8().constData(), connection, parent)
        {  }

        //! For each signal in parent, attempt to connect to it an interface signal of the same name.
        //! \see BLACK_NO_RELAY
        void relayParentSignals()
        {
            const QMetaObject *metaObject = this->parent()->metaObject();
            const QMetaObject *superMetaObject = metaObject;
            while (strcmp(superMetaObject->superClass()->className(), "QObject") != 0) { superMetaObject = superMetaObject->superClass(); }

            for (int i = superMetaObject->methodOffset(), count = metaObject->methodCount(); i < count; ++i)
            {
                QMetaMethod method = metaObject->method(i);
                if (method.methodType() != QMetaMethod::Signal) { continue; }
                if (method.tag() && strcmp(method.tag(), "BLACK_NO_RELAY") == 0) { continue; }

                this->connection().connect(this->service(), this->path(), this->interface(), method.name(), this->parent(),
                    method.methodSignature().prepend("2")); // the reason for this "2" can be found in the definition of SIGNAL() macro
            }
        }

        //! Call DBus, no return value
        template <typename... Args>
        void callDBus(QLatin1String method, Args &&... args)
        {
            const QList<QVariant> argumentList { QVariant::fromValue(std::forward<Args>(args))... };
            this->asyncCallWithArgumentList(method, argumentList);
        }

        //! Call DBus with synchronous return value
        template <typename Ret, typename... Args>
        Ret callDBusRet(QLatin1String method, Args &&... args)
        {
            QList<QVariant> argumentList { QVariant::fromValue(std::forward<Args>(args))... };
            QDBusPendingReply<Ret> pr = this->asyncCallWithArgumentList(method, argumentList);
            return pr;
        }

        //! Call DBus with asynchronous return value
        //! Callback can be any callable object taking a single argument of type QDBusPendingCallWatcher*.
        template <typename Func, typename... Args>
        void callDBusAsync(QLatin1String method, Func callback, Args &&... args)
        {
            QList<QVariant> argumentList { QVariant::fromValue(std::forward<Args>(args))... };
            QDBusPendingCall pc = this->asyncCallWithArgumentList(method, argumentList);
            auto pcw = new QDBusPendingCallWatcher(pc, this);
            connect(pcw, &QDBusPendingCallWatcher::finished, callback);
        }

        //! Cancel all asynchronous DBus calls which are currently pending
        //! \warning Don't call this from inside an async callback!
        void cancelAllPendingAsyncCalls()
        {
            auto watchers = this->findChildren<QDBusPendingCallWatcher *>(QString(), Qt::FindDirectChildrenOnly);
            for (auto w : watchers)
            {
                delete w;
            }
        }
    };
} // ns

#endif // guard
