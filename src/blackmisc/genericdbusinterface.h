// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_GENERICDBUSINTERFACE_H
#define BLACKMISC_GENERICDBUSINTERFACE_H

#include "blackmisc/logmessage.h"
#include "blackmisc/promise.h"
#include <QDBusAbstractInterface>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QDBusError>
#include <QObject>
#include <QMetaMethod>
#include <QSharedPointer>

#ifndef Q_MOC_RUN
/*!
 * Any signals tagged with this macro will be ignored by BlackMisc::CGenericDBusInterface::relayParentSignals().
 * \see QMetaMethod::tag
 */
#    define BLACK_NO_RELAY
#endif

namespace BlackMisc
{
    /*!
     * Used for hand written interface based on virtual methods.
     * Allows to relay a message to DBus in a single code line
     */
    class BLACKMISC_EXPORT CGenericDBusInterface : public QDBusAbstractInterface
    {
        Q_OBJECT

    public:
        //! Constructor
        CGenericDBusInterface(const QString &serviceName, const QString &path, const QString &interfaceName, const QDBusConnection &connection, QObject *parent = nullptr) : QDBusAbstractInterface(serviceName, path, interfaceName.toUtf8().constData(), connection, parent)
        {}

        //! For each signal in parent, attempt to connect to it an interface signal of the same name.
        //! \see BLACK_NO_RELAY
        void relayParentSignals()
        {
            const QMetaObject *metaObject = this->parent()->metaObject();
            const QMetaObject *superMetaObject = metaObject;
            while (strcmp(superMetaObject->superClass()->className(), "QObject") != 0) { superMetaObject = superMetaObject->superClass(); }

            for (int i = superMetaObject->methodOffset(), count = metaObject->methodCount(); i < count; ++i)
            {
                const QMetaMethod method = metaObject->method(i);
                if (method.methodType() != QMetaMethod::Signal) { continue; }
                if (method.tag() && strcmp(method.tag(), "BLACK_NO_RELAY") == 0) { continue; }

                const QByteArray signature = method.methodSignature().prepend("2"); // the reason for this "2" can be found in the definition of SIGNAL() macro
                const bool c = this->connection().connect(this->service(), this->path(), this->interface(), method.name(), this->parent(), signature);
                if (!c)
                {
                    CLogMessage(this).error(u"Cannot connect signal: %1") << QString(signature);
                }
            }
        }

        //! Call DBus, no return value
        template <typename... Args>
        void callDBus(QLatin1String method, Args &&...args)
        {
            const QList<QVariant> argumentList { QVariant::fromValue(std::forward<Args>(args))... };
            this->callWithArgumentList(QDBus::NoBlock, method, argumentList);
        }

        //! Call DBus with synchronous return value
        template <typename Ret, typename... Args>
        Ret callDBusRet(QLatin1String method, Args &&...args)
        {
            QList<QVariant> argumentList { QVariant::fromValue(std::forward<Args>(args))... };
            QDBusPendingReply<Ret> pr = this->asyncCallWithArgumentList(method, argumentList);
            pr.waitForFinished();
            if (pr.isError())
            {
                CLogMessage(this).debug(u"CGenericDBusInterface::callDBusRet(%1) returned: %2") << method << pr.error().message();
            }
            return pr;
        }

        //! Call DBus with asynchronous return value
        //! Callback can be any callable object taking a single argument of type QDBusPendingCallWatcher*.
        template <typename Func, typename... Args>
        QDBusPendingCallWatcher *callDBusAsync(QLatin1String method, Func callback, Args &&...args)
        {
            QList<QVariant> argumentList { QVariant::fromValue(std::forward<Args>(args))... };
            QDBusPendingCall pc = this->asyncCallWithArgumentList(method, argumentList);
            auto pcw = new QDBusPendingCallWatcher(pc, this);
            connect(pcw, &QDBusPendingCallWatcher::finished, callback);
            return pcw;
        }

        //! Call DBus with asynchronous return as a future
        template <typename Ret, typename... Args>
        QFuture<Ret> callDBusFuture(QLatin1String method, Args &&...args)
        {
            auto sharedPromise = QSharedPointer<CPromise<Ret>>::create();
            this->callDBusAsync(
                method, [=](auto pcw) { sharedPromise->setResult(QDBusPendingReply<Ret>(*pcw)); }, std::forward<Args>(args)...);
            return sharedPromise->future();
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
