/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISCKTEST_TESTSERVICEPQAV_H
#define BLACKMISCKTEST_TESTSERVICEPQAV_H

// clash with struct interface in objbase.h used to happen
#pragma push_macro("interface")
#undef interface

#include "blackmisc/avallclasses.h"
#include "blackmisc/pqallquantities.h"
#include "blackmisc/mathallclasses.h"
#include "servicetool.h"
#include <QObject>
#include <QDebug>
#include <QDBusVariant>
#include <QDBusConnection>
#include <QDBusObjectPath>

#define BLACKMISCKTEST_TESTSERVICE_INTERFACENAME "blackmisctest.testservice"
#define BLACKMISCKTEST_TESTSERVICE_SERVICEPATH "/ts"

namespace BlackMiscTest
{

    /*!
     * Testservice for PQ DBus tests. Needs to re-generate the introspection xml file
     * (qdbuscpp2xml) when new slots have been added.
     */
    class Testservice : public QObject
    {
        // see the readme.txt on how to qdbuscpp2xml
        // remember to recompile the plugin when new class have been createdand registered
        // http://techbase.kde.org/Development/Tutorials/D-Bus/CustomTypes#Write_a_class
        // https://dev.vatsim-germany.org/projects/vatpilotclient/wiki/DBusExample
        // http://qt-project.org/doc/qt-4.8/examples-dbus.html
        // http://dbus.freedesktop.org/doc/dbus-tutorial.html#meta

        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKMISCKTEST_TESTSERVICE_INTERFACENAME)

        // For some reasons the interface name in the XML is not set correctly
        // to the above name

    signals:
        /*!
         * \brief Send string message
         * \param message
         */
        void sendStringMessage(const QString &message);

    public slots:

        /*!
         * \brief Receive string message
         * \param message
         */
        void receiveStringMessage(const QString &message);

        /*!
         * \brief Receive a QVariant
         * \param variant
         * \param localMetyType, works only in the same binary
         */
        void receiveVariant(const QDBusVariant &variant, int localMetaType);

        /*!
         * \brief Receive speed
         * \param speed
         */
        void receiveSpeed(const BlackMisc::PhysicalQuantities::CSpeed &speed);

        /*!
         * \brief Receive speed
         * \param speed
         */
        BlackMisc::PhysicalQuantities::CSpeed pingSpeed(const BlackMisc::PhysicalQuantities::CSpeed &speed);

        /*!
         * \brief Receive com unit
         * \param comUnit
         */
        void receiveComUnit(const BlackMisc::Aviation::CComSystem &comUnit);

        /*!
         * \brief Receive altitude
         * \param comUnit
         */
        void receiveAltitude(const BlackMisc::Aviation::CAltitude &altitude);

        /*!
         * \brief Receive altitude
         * \param comUnit
         */
        BlackMisc::Aviation::CAltitude pingAltitude(const BlackMisc::Aviation::CAltitude &altitude);

        /*!
         * \brief Receive matrix
         * \param matrix
         */
        void receiveMatrix(const BlackMisc::Math::CMatrix3x3 &matrix);

        /*!
         * \brief Receive list
         * \param list
         */
        void receiveList(const QList<double> &list);

        /*!
         * \brief Receive a geo position
         * \param geo
         */
        void receiveGeoPosition(const BlackMisc::Geo::CCoordinateGeodetic &geo) const;

        /*!
         * \brief Receive transponder
         * \param transponder
         */
        void receiveTransponder(const BlackMisc::Aviation::CTransponder &transponder) const;

        /*!
         * \brief Receive track
         * \param track
         */
        void receiveTrack(const BlackMisc::Aviation::CTrack &track) const;

        /*!
         * \brief Receive a length (not working)
         * \param length
         */
        void receiveLength(const BlackMisc::PhysicalQuantities::CLength &length) const;

        /*!
         * \brief Receive lengths
         * \param length
         */
        void receiveLengthsQl(const QList<QVariant> &lengthsList) const;

        /*!
         * \brief Receive lengths
         * \param length
         */
        void receiveLengthsQvl(const QVariantList &lengthsVariantList) const;

        /*!
         * \brief Receive ATC station
         * \param station
         */
        void receiveAtcStation(const BlackMisc::Aviation::CAtcStation &station) const;

        /*!
         * \brief Ping ATC station
         * \param station
         */
        BlackMisc::Aviation::CAtcStation pingAtcStation(const BlackMisc::Aviation::CAtcStation &station);

        /*!
         * \brief Ping aircraft
         * \param aircraft
         * \return
         */
        BlackMisc::Aviation::CAircraft pingAircraft(const BlackMisc::Aviation::CAircraft &aircraft);

        /*!
         * \brief Receive callsign
         * \param callsign
         */
        void receiveCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

        /*!
         * \brief return n ATC stations
         * \param number
         * \return
         */
        const BlackMisc::Aviation::CAtcStationList getAtcStationList(const qint32 number) const;

        /*!
         * \brief Get object paths
         * \param number
         * \return
         */
        const QList<QDBusObjectPath> getObjectPaths(const qint32 number) const;

        /*!
         * \brief Receive ATC list
         * \param AtcStationList
         */
        void receiveAtcStationList(const BlackMisc::Aviation::CAtcStationList &AtcStationList) const;

        /*!
         * \brief Receive an value map
         * \param valueMap
         */
        void receiveValueMap(const BlackMisc::CIndexVariantMap &valueMap) const;

        /*!
         * \brief Ping atc list
         * \param AtcStationList
         * \return
         */
        BlackMisc::Aviation::CAtcStationList pingAtcStationList(const BlackMisc::Aviation::CAtcStationList &AtcStationList) const;

        /*!
         * \brief Get speed
         * \return
         */
        BlackMisc::PhysicalQuantities::CSpeed getSpeed() const;

        /*!
         * \brief Get station
         * \return
         */
        BlackMisc::Aviation::CAtcStation getAtcStation() const;

    public:
        static const QString ServiceName;
        static const QString ServicePath;

        /*!
         * \brief Constructor
         * \param parent
         */
        explicit Testservice(QObject *parent = nullptr);

    private:
        BlackMisc::Aviation::CAtcStationList m_someAtcStations;

    };

} // namespace

#pragma pop_macro("interface")

#endif // guard
