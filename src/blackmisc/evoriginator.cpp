/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
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

        // Copy Constructor
        COriginator::COriginator(const COriginator &other)
            : CValueObject(other),
              m_originatorName(other.m_originatorName),
              m_machineId(other.m_machineId),
              m_primaryIpAddress(other.m_primaryIpAddress),
              m_objectId(other.m_objectId),
              m_processId(other.m_processId),
              m_processName(other.m_processName)
        {
        }

        // Hash
        uint COriginator::getValueHash() const
        {
            return qHash(TupleConverter<COriginator>::toTuple(*this));
        }

        // Register metadata
        void COriginator::registerMetadata()
        {
            qRegisterMetaType<COriginator>();
            qDBusRegisterMetaType<COriginator>();
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

        /*
         * metaTypeId
         */
        int COriginator::getMetaTypeId() const
        {
            return qMetaTypeId<COriginator>();
        }

        /*
         * is a
         */
        bool COriginator::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<COriginator>()) { return true; }

            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Compare
         */
        int COriginator::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const COriginator &>(otherBase);
            return compare(TupleConverter<COriginator>::toTuple(*this), TupleConverter<COriginator>::toTuple(other));
        }

        /*
         * Marshall to DBus
         */
        void COriginator::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<COriginator>::toTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void COriginator::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<COriginator>::toTuple(*this);
        }
    } // namespace Event
}
