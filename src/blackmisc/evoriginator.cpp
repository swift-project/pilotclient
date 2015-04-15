/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "evoriginator.h"
#include <QCoreApplication>
#include <QDBusConnection>

namespace BlackMisc
{
    namespace Event
    {
        // Default constructor
        COriginator::COriginator()
            : m_machineId(QDBusConnection::localMachineId()),
              m_processId(QCoreApplication::applicationPid()),
              m_processName(QCoreApplication::applicationName())
        {
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

        /*
         * Convert to string
         */
        QString COriginator::convertToQString(bool /* i18n */) const
        {
            QString s;
            s.append(m_originatorName);
            s.append(" ").append(m_machineId);
            s.append(" ").append(m_primaryIpAddress);
            s.append(" ").append(m_objectId);
            s.append(" ").append(m_processId);
            s.append(" ").append(m_processName);
            return s;
        }

    }
}
