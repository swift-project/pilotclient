/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISCKTEST_TESTSERVICEPQAV_H
#define BLACKMISCKTEST_TESTSERVICEPQAV_H

// clash with struct interace in objbase.h used to happen
#pragma push_macro("interface")
#undef interface

#define BLACKMISCKTEST_SERVICENAME "blackmisctest.testservice"
#define BLACKMISCKTEST_SERVICEPATH "/blackbus"

#include <QObject>
#include <QDebug>
#include <QDBusVariant>
#include "blackmisc/blackmiscfreefunctions.h"
#include "testservicetool.h"

namespace BlackMiscTest {

/*!
 * \brief Testservice for PQ DBus tests
 */
class Testservice : public QObject
{
    // http://techbase.kde.org/Development/Tutorials/D-Bus/CustomTypes#Write_a_class
    // https://dev.vatsim-germany.org/projects/vatpilotclient/wiki/DBusExample
    // http://qt-project.org/doc/qt-4.8/examples-dbus.html
    // http://dbus.freedesktop.org/doc/dbus-tutorial.html#meta

    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", BLACKMISCKTEST_SERVICENAME)

    // For some reasons the interface name in the XML is not set correctly
    // to the above name


signals:
    /*!
     * \brief Send string message
     * \param message
     */
    void sendStringMessage(const QString& message);

public slots:
    /*!
     * \brief Receive string message
     * \param message
     */
    void receiveStringMessage(const QString &message);

    /*!
     * \brief Receive a QVariant
     * \param variant
     */
    void receiveVariant(const QDBusVariant &variant);

    /*!
     * \brief Receive speed
     * \param speed
     */
    void receiveSpeed(const BlackMisc::PhysicalQuantities::CSpeed &speed);

    /*!
     * \brief receiveComUnit
     * \param comUnit
     */
    void receiveComUnit(const BlackMisc::Aviation::CComSystem &comUnit);

public:
    static const QString ServiceName;
    static const QString ServicePath;

    /*!
     * \brief Constructor
     * \param parent
     */
    explicit Testservice(QObject *parent = 0);

};

} // namespace

#pragma pop_macro("interface")

#endif // BLACKMISCKTEST_TESTSERVICEPQAV_H
