/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/identifier.h"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QHostInfo>
#include <QStringBuilder>

//! \private
const QString &cachedLocalHostName()
{
    static const QString hostName = QHostInfo::localHostName();
    return hostName;
}

namespace BlackMisc
{
    CIdentifier::CIdentifier(const QString &name)
        : ITimestampBased(QDateTime::currentMSecsSinceEpoch()),
          m_name(name.trimmed()),
          m_machineIdBase64(QDBusConnection::localMachineId().toBase64()),
          m_machineName(cachedLocalHostName()),
          m_processName(QCoreApplication::applicationName()),
          m_processId(QCoreApplication::applicationPid())
    { }

    CIdentifier CIdentifier::anonymous()
    {
        static CIdentifier id;
        if (id.m_processId)
        {
            id.m_processId = 0;
            id.m_processName = "";
            id.m_machineName = "";
            id.m_machineIdBase64 = "";
        }
        return id;
    }

    CIdentifier CIdentifier::fake()
    {
        static CIdentifier id;
        if (id.m_processId)
        {
            id.m_processId = 0;
            id.m_processName = "fake process";
            id.m_machineName = "fake machine";
            id.m_machineIdBase64 = QByteArrayLiteral("0").repeated(32).toBase64();
        }
        return id;
    }

    QUuid CIdentifier::toUuid() const
    {
        static const QUuid ns = QUuid::createUuid();
        QByteArray baseData;
        baseData.append(getMachineId());
        baseData.append(reinterpret_cast<const char *>(&m_processId), sizeof(m_processId));
        baseData.append(reinterpret_cast<const char *>(&m_timestampMSecsSinceEpoch), sizeof(m_timestampMSecsSinceEpoch));
        baseData.append(getName());
        return QUuid::createUuidV5(ns, baseData);
    }

    QString CIdentifier::toUuidString() const
    {
        return toUuid().toString();
    }

    QByteArray CIdentifier::getMachineId() const
    {
        return QByteArray::fromBase64(m_machineIdBase64.toLocal8Bit());
    }

    bool CIdentifier::isFromLocalMachine() const
    {
        return QDBusConnection::localMachineId() == getMachineId();
    }

    bool CIdentifier::isFromSameMachine(const CIdentifier &other) const
    {
        return getMachineIdBase64() == other.getMachineIdBase64();
    }

    bool CIdentifier::isFromSameProcess() const
    {
        return QCoreApplication::applicationPid() == getProcessId() && isFromLocalMachine();
    }

    bool CIdentifier::isFromSameProcessName() const
    {
        return QCoreApplication::applicationName() == getProcessName();
    }

    bool CIdentifier::isAnonymous() const
    {
        return anonymous() == *this;
    }

    QString CIdentifier::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        const QString s = m_name %
                          QLatin1Char(' ') % m_machineIdBase64 %
                          QLatin1Char(' ') % m_machineName %
                          QLatin1Char(' ') % QString::number(m_processId) %
                          QLatin1Char(' ') % m_processName;
        return s;
    }

    CVariant CIdentifier::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
    {
        if (index.isMyself()) { return CVariant::from(*this); }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }

        switch (i)
        {
        case IndexName: return CVariant::fromValue(m_name);
        case IndexMachineIdBase64: return CVariant::fromValue(m_machineIdBase64);
        case IndexMachineName: return CVariant::fromValue(getMachineName());
        case IndexMachineId: return CVariant::fromValue(getMachineId());
        case IndexProcessId: return CVariant::fromValue(m_processId);
        case IndexProcessName: return CVariant::fromValue(m_processName);
        case IndexIsFromLocalMachine: return CVariant::fromValue(isFromLocalMachine());
        case IndexIsFromSameProcess: return CVariant::fromValue(isFromSameProcess());
        case IndexIsFromSameProcessName: return CVariant::fromValue(isFromSameProcessName());
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CIdentifier::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
    {
        CValueObject::setPropertyByIndex(index, variant);
    }
} // ns
