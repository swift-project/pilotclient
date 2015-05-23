/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "originator.h"
#include <QCoreApplication>
#include <QDBusConnection>

namespace BlackMisc
{
    COriginator::COriginator(const QString &name)
        : m_name(name),
          m_machineIdBase64(QDBusConnection::localMachineId().toBase64()),
          m_processName(QCoreApplication::applicationName()),
          m_processId(QCoreApplication::applicationPid()),
          m_timestampMsEpoch(QDateTime::currentMSecsSinceEpoch())
    { }

    COriginator::COriginator(const QObject *object) : COriginator(object->objectName())
    {
        Q_ASSERT_X(!object->objectName().isEmpty(), Q_FUNC_INFO, "Missing name");
    }

    QByteArray COriginator::getMachineId() const
    {
        return QByteArray::fromBase64(m_machineIdBase64.toLocal8Bit());
    }

    bool COriginator::isFromLocalMachine() const
    {
        return QDBusConnection::localMachineId() == getMachineId();
    }

    bool COriginator::isFromSameProcess() const
    {
        return QCoreApplication::applicationPid() == getProcessId() && isFromLocalMachine();
    }

    bool COriginator::isFromSameProcessName() const
    {
        return QCoreApplication::applicationName() == getProcessName();
    }

    QString COriginator::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        QString s;
        s.append(m_name);
        s.append(" ").append(m_machineIdBase64);
        s.append(" ").append(QString::number(m_processId));
        s.append(" ").append(m_processName);
        return s;
    }

    CVariant COriginator::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
    {
        if (index.isMyself()) { return this->toCVariant(); }

        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexName:
            return CVariant::fromValue(m_name);
        case IndexMachineIdBase64:
            return CVariant::fromValue(m_machineIdBase64);
        case IndexMachineId:
            return CVariant::fromValue(getMachineId());
        case IndexProcessId:
            return CVariant::fromValue(m_processId);
        case IndexProcessName:
            return CVariant::fromValue(m_processName);
        case IndexIsFromLocalMachine:
            return CVariant::fromValue(isFromLocalMachine());
        case IndexIsFromSameProcess:
            return CVariant::fromValue(isFromSameProcess());
        case IndexIsFromSameProcessName:
            return CVariant::fromValue(isFromSameProcessName());
        case IndexUtcTimestamp:
            return CVariant::fromValue(getTimestamp());
        default:
            return CValueObject::propertyByIndex(index);
        }
    }

    void COriginator::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
    {
        CValueObject::setPropertyByIndex(variant, index);
    }

} // ns
