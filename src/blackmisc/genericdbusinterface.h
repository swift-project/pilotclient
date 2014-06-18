#ifndef BLACKMISC_GENERICDBUSINTERFACE_H
#define BLACKMISC_GENERICDBUSINTERFACE_H

#include <QDBusAbstractInterface>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QObject>

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
        CGenericDBusInterface(const QString &serverName, const QString &path, const QString &interfaceName, const QDBusConnection &connection, QObject *parent = 0) :
            QDBusAbstractInterface(serverName, path, interfaceName.toUtf8().constData(), connection, parent)
        {
        }

        //! Call DBus, no return value
        template <typename... Args>
        void callDBus(const QLatin1String &method, Args&&... args)
        {
            QList<QVariant> argumentList { QVariant::fromValue(std::forward<Args>(args))... };
            this->asyncCallWithArgumentList(method, argumentList);
        }

        //! Call DBus with synchronous return value
        template <typename Ret, typename... Args>
        Ret callDBusRet(const QLatin1String &method, Args&&... args)
        {
            QList<QVariant> argumentList { QVariant::fromValue(std::forward<Args>(args))... };
            QDBusPendingReply<Ret> pr = this->asyncCallWithArgumentList(method, argumentList);
            return pr;
        }

        //! Call DBus with asynchronous return value
        //! Callback can be any callable object taking a single argument of type QDBusPendingCallWatcher*.
        template <typename Func, typename... Args>
        void callDBusAsync(const QLatin1String &method, Func callback, Args&&... args)
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
}

#endif // guard
