#ifndef BLACKMISC_GENERICDBUSINTERFACE_H
#define BLACKMISC_GENERICDBUSINTERFACE_H

#include <QDBusAbstractInterface>
#include <QDBusPendingCall>
#include <QDBusPendingReply>


namespace BlackMisc
{

#include <QObject>

    /*!
     * Used for hand written interface based on virtual methods.
     * Allows to relay a message to DBus in a single code line
     */
    class CGenericDBusInterface : public QDBusAbstractInterface
    {

    public:

        /*!
         * \brief Generic DBus interface
         * \param serverName
         * \param path
         * \param interfaceName
         * \param connection
         * \param parent
         */
        CGenericDBusInterface(const QString &serverName, const QString &path, const QString &interfaceName, const QDBusConnection &connection, QObject *parent = 0) :
            QDBusAbstractInterface(serverName, path, interfaceName.toUtf8().constData(), connection, parent)
        {
            // void
        }

        /*!
         * \brief Call DBus
         * \param method
         */
        void callDBus(const QLatin1String &method)
        {
            QList<QVariant> argumentList;
            this->asyncCallWithArgumentList(method, argumentList);
        }

        /*!
          * \brief Call DBus, no return value
          * \param method
          * \param p1
          * \return
          */
        template<typename P1> void callDBus(const QLatin1String &method, P1 p1)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(p1);
            this->asyncCallWithArgumentList(method, argumentList);
        }

        /*!
          * \brief Call DBus, no return value
          * \param method
          * \param p1
          * \param p2
          * \return
          */
        template<typename P1, typename P2> void callDBus(const QLatin1String &method, P1 p1, P2 p2)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(p1) << QVariant::fromValue(p2);
            this->asyncCallWithArgumentList(method, argumentList);
        }

        /*!
          * \brief Call DBus, no return value
          * \param method
          * \param p1
          * \param p2
          * \param p3
          * \return
          */
        template<typename P1, typename P2, typename P3> void callDBus(const QLatin1String &method, P1 p1, P2 p2, P3 p3)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(p1) << QVariant::fromValue(p2) << QVariant::fromValue(p3);
            this->asyncCallWithArgumentList(method, argumentList);
        }

        /*!
          * \brief Call DBus, no return value
          * \param method
          * \param p1
          * \param p2
          * \param p3
          * \param p4
          * \return
          */
        template<typename P1, typename P2, typename P3, typename P4> void callDBus(const QLatin1String &method, P1 p1, P2 p2, P3 p3, P4 p4)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(p1) << QVariant::fromValue(p2) << QVariant::fromValue(p3) << QVariant::fromValue(p4);
            this->asyncCallWithArgumentList(method, argumentList);
        }

        /*!
          * \brief Call DBus with return value
          * \param method
          * \return
          */
        template<typename Ret> Ret callDBusRet(const QLatin1String &method)
        {
            QList<QVariant> argumentList;
            QDBusPendingReply<Ret> pr = this->asyncCallWithArgumentList(method, argumentList);
            return pr;
        }

        /*!
          * \brief Call DBus with return value
          * \param method
          * \param p1
          * \return
          */
        template<typename Ret, typename P1> Ret callDBusRet(const QLatin1String &method, P1 p1)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(p1);
            QDBusPendingReply<Ret> pr = this->asyncCallWithArgumentList(method, argumentList);
            return pr;
        }

        /*!
          * \brief Call DBus with return value
          * \param method
          * \param p1
          * \param p2
          * \return
          */
        template<typename Ret, typename P1, typename P2> Ret callDBusRet(const QLatin1String &method, P1 p1, P2 p2)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(p1) << QVariant::fromValue(p2);
            QDBusPendingReply<Ret> pr = this->asyncCallWithArgumentList(method, argumentList);
            return pr;
        }

        /*!
          * \brief Call DBus with return value
          * \param method
          * \param p1
          * \param p2
          * \param p3
          * \return
          */
        template<typename Ret, typename P1, typename P2, typename P3> Ret callDBusRet(const QLatin1String &method, P1 p1, P2 p2, P3 p3)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(p1) << QVariant::fromValue(p2) << QVariant::fromValue(p3);
            QDBusPendingReply<Ret> pr = this->asyncCallWithArgumentList(method, argumentList);
            return pr;
        }

        /*!
          * \brief Call DBus with return value
          * \param method
          * \param p1
          * \param p2
          * \param p3
          * \param p4
          * \return
          */
        template<typename Ret, typename P1, typename P2, typename P3, typename P4> Ret callDBusRet(const QLatin1String &method, P1 p1, P2 p2, P3 p3, P4 p4)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(p1) << QVariant::fromValue(p2) << QVariant::fromValue(p3) << QVariant::fromValue(p4);
            QDBusPendingReply<Ret> pr = this->asyncCallWithArgumentList(method, argumentList);
            return pr;
        }
    };
}

#endif // guard
