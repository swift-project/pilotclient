#ifndef BLACKMISCTEST_DATACONTEXTCORE_H
#define BLACKMISCTEST_DATACONTEXTCORE_H

#include "blackcore/dbus_server.h"
#include "blackmisc/avallclasses.h"
#include <QObject>

#define BLACKMISCKTEST_DATACONTEXT_INTERFACENAME "blackmisctest.datacontext"
#define BLACKMISCKTEST_DATACONTEXT_SERVICEPATH "/datacontext"

namespace BlackMiscTest
{

    /*!
     * \brief A poor man's Data context running in the core
     */
    class CDataContext : public QObject
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKMISCKTEST_DATACONTEXT_INTERFACENAME)
        Q_PROPERTY(BlackMisc::Aviation::CAtcStationList qpAtcOnlineList READ onlineControllers WRITE setOnlineControllers)
        Q_PROPERTY(BlackMisc::Aviation::CAtcStationList qpAtcBookedList READ bookedControllers WRITE setBookedControllers)
        Q_PROPERTY(QStringList qpFooStrings READ fooStrings)

        // no property for Foo, which is just meant to be local only

    Q_SIGNALS:
        void fooSignal(const QString &message);

    public:
        static const QString ServiceName;
        static const QString ServicePath;

    private:
        BlackMisc::Aviation::CAtcStationList m_atcOnline;
        BlackMisc::Aviation::CAtcStationList m_atcBooked;
        QString m_foo;
        QStringList m_fooStrings;

    public slots:

        /*!
         * \brief FooSlot
         * \param baz
         */
        void fooSlot(const QString &baz); // generated interface methods are "const &QString", so this must match

        /*!
         * \brief FooSlotRet, with return value
         * \param baz
         * \return
         */
        QString fooSlotRet(const QString &baz);

        /*!
         * \brief Update booked controllers
         * \param condition
         * \param values
         */
        void updateBookedControllers(const BlackMisc::CValueMap &condition, const BlackMisc::CValueMap &values)
        {
            qDebug() << Q_FUNC_INFO;
            qDebug() << "  condition" << condition;
            qDebug() << "  values" << values;
            m_atcBooked.applyIf(condition, values);
        }

        /*!
         * \brief Update online controllers
         * \param condition
         * \param values
         */
        void updateOnlineControllers(const BlackMisc::CValueMap &condition, const BlackMisc::CValueMap &values)
        {
            qDebug() << Q_FUNC_INFO;
            qDebug() << "  condition" << condition;
            qDebug() << "  values" << values;
            m_atcOnline.applyIf(condition, values);
        }

    public:
        /*!
         * \brief Default constructor
         */
        CDataContext(QObject *parent = 0) : QObject(parent) {}

        /*!
         * \brief ctor for Qt meta system
         * \param other
         */
        CDataContext(const CDataContext &other) :
            QObject(), m_atcOnline(other.m_atcOnline), m_atcBooked(other.m_atcBooked), m_foo(other.m_foo), m_fooStrings(other.fooStrings())
        {
            // void
        }

        /*!
         * \brief With link to server
         * \param server
         */
        CDataContext(BlackCore::CDBusServer *server);

        /*!
         * \brief The "central" ATC list with online ATC controllers
         * \return
         */
        BlackMisc::Aviation::CAtcStationList &onlineControllers()
        {
            qDebug() << Q_FUNC_INFO;
            return m_atcOnline;
        }

        /*!
         * \brief The "central" ATC list with online ATC controllers
         * \return
         */
        const BlackMisc::Aviation::CAtcStationList &onlineControllers() const
        {
            return m_atcOnline;
        }

        /*!
         * \brief Another ATC list, maybe all booked controllers
         * \return
         */
        const BlackMisc::Aviation::CAtcStationList &bookedControllers() const
        {
            qDebug() << Q_FUNC_INFO;
            return m_atcBooked;
        }

        /*!
         * \brief Replace value by new values, but keep object itself intact
         * \param newValues
         */
        void setBookedControllers(const BlackMisc::Aviation::CAtcStationList &newValues)
        {
            // problem concurrent updates
            qDebug() << Q_FUNC_INFO;
            this->m_atcBooked = newValues;
        }

        /*!
         * \brief Replace value by new values, but keep object itself intact
         * \param newValues
         */
        void setOnlineControllers(const BlackMisc::Aviation::CAtcStationList &newValues)
        {
            // problem concurrent updates
            qDebug() << Q_FUNC_INFO;
            this->m_atcOnline = newValues;
        }

        /*!
         * \brief Some property which would be local only
         * \return
         */
        const QString &foo() const
        {
            qDebug() << Q_FUNC_INFO;
            return m_foo;
        }

        /*!
         * \brief Some foo strings
         * \return
         */
        const QStringList &fooStrings() const
        {
            qDebug() << Q_FUNC_INFO;
            return m_fooStrings;
        }

        /*!
         * \brief Set n foo strings
         * \param number
         */
        void setFooStrings(int number)
        {
            this->m_fooStrings.clear();
            for (int i = 0; i < number; i++)
            {
                QString foo("I am foo ");
                foo.append(QString::number(i));
                this->m_fooStrings.append(foo);
            }
        }

    };
}
Q_DECLARE_METATYPE(BlackMiscTest::CDataContext)

#endif // guard
