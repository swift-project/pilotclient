/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testservice.h"

namespace BlackMiscTest
{

const QString Testservice::ServiceName = QString(BLACKMISCKTEST_SERVICENAME);
const QString Testservice::ServicePath = QString(BLACKMISCKTEST_SERVICEPATH);

/*
 * Constructor
 */
Testservice::Testservice(QObject *parent) : QObject(parent)
{
    // void
}

/*
 * Slot to receive messages
 */
void Testservice::receiveStringMessage(const QString &message)
{
    qDebug() << "Pid:" << TestserviceTool::getPid() << "Received message:" << message;
}

/*
 * Receive variant
 */
void Testservice::receiveVariant(const QDBusVariant &variant)
{
    QVariant qv = variant.variant();
    BlackMisc::Aviation::CAltitude altitude = qv.value<BlackMisc::Aviation::CAltitude>();
    qDebug() << "Pid:" << TestserviceTool::getPid() << "Received variant:" << altitude;
}

/*
 * Receive speed
 */
void Testservice::receiveSpeed(const BlackMisc::PhysicalQuantities::CSpeed &speed)
{
    qDebug() << "Pid:" << TestserviceTool::getPid() << "Received speed:" << speed;
}

/*
 * Receive COM unit
 */
void Testservice::receiveComUnit(const BlackMisc::Aviation::CComSystem &comUnit)
{
    qDebug() << "Pid:" << TestserviceTool::getPid() << "Received COM:" << comUnit;
}

/*
 * Receive altitude
 */
void Testservice::receiveAltitude(const BlackMisc::Aviation::CAltitude &altitude)
{
    qDebug() << "Pid:" << TestserviceTool::getPid() << "Received altitude:" << altitude;
}

/*
 * Receive matrix
 */
void Testservice::receiveMatrix(const BlackMisc::Math::CMatrix3x3 &matrix)
{
    qDebug() << "Pid:" << TestserviceTool::getPid() << "Received matrix:" << matrix;
}

/*
 *
 */
void Testservice::receiveList(const QList<double> &list)
{
    qDebug() << "Pid:" << TestserviceTool::getPid() << "Received list:" << list;
}

} // namespace
