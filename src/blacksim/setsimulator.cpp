/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "setsimulator.h"

using namespace BlackMisc;
using namespace BlackMisc::Settings;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackSim
{
    namespace Settings
    {
        /*
         * Constructor
         */
        CSettingsSimulator::CSettingsSimulator()
        {
            this->initDefaultValues();
        }

        /*
         * Convert to string
         */
        QString CSettingsSimulator::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            QString s("Sel.driver:");
            s.append(" ").append(m_selectedPlugin.toQString(i18n));
            return s;
        }

        /*
         * metaTypeId
         */
        int CSettingsSimulator::getMetaTypeId() const
        {
            return qMetaTypeId<CSettingsSimulator>();
        }

        /*
         * is a
         */
        bool CSettingsSimulator::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CSettingsSimulator>()) { return true; }
            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Compare
         */
        int CSettingsSimulator::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CSettingsSimulator &>(otherBase);
            return compare(TupleConverter<CSettingsSimulator>::toTuple(*this), TupleConverter<CSettingsSimulator>::toTuple(other));
        }

        /*
         * Marshall
         */
        void CSettingsSimulator::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CSettingsSimulator>::toTuple(*this);
        }

        /*
         * Unmarshall
         */
        void CSettingsSimulator::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CSettingsSimulator>::toTuple(*this);
        }

        /*
         * Equal?
         */
        bool CSettingsSimulator::operator ==(const CSettingsSimulator &other) const
        {
            if (this == &other) return true;
            return compare(*this, other) == 0;
        }

        /*
         * Unequal?
         */
        bool CSettingsSimulator::operator !=(const CSettingsSimulator &other) const
        {
            return !((*this) == other);
        }

        /*
         * Hash
         */
        uint CSettingsSimulator::getValueHash() const
        {
            return qHash(TupleConverter<CSettingsSimulator>::toTuple(*this));
        }

        /*
         * To JSON
         */
        QJsonObject CSettingsSimulator::toJson() const
        {
            return BlackMisc::serializeJson(TupleConverter<CSettingsSimulator>::toMetaTuple(*this));
        }

        /*
         * From JSON
         */
        void CSettingsSimulator::fromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, TupleConverter<CSettingsSimulator>::toMetaTuple(*this));
        }

        /*
         * Members
         */
        const QStringList &CSettingsSimulator::jsonMembers()
        {
            return TupleConverter<CSettingsSimulator>::jsonMembers();
        }

        /*
         * Default values
         */
        void CSettingsSimulator::initDefaultValues()
        {
            this->m_selectedPlugin = CSimulatorInfo::FSX();
            this->m_timeSyncOffset = CTime(0, CTimeUnit::hrmin());
            this->m_timeSync = false;
        }

        /*
         * Register metadata
         */
        void CSettingsSimulator::registerMetadata()
        {
            qRegisterMetaType<CSettingsSimulator>();
            qDBusRegisterMetaType<CSettingsSimulator>();
        }

        /*
         * Value
         */
        BlackMisc::CStatusMessageList CSettingsSimulator::value(const QString &path, const QString &command, const CVariant &value, bool &changedFlag)
        {
            // TODO: This needs to be refactored to a smarter way to delegate commands
            changedFlag = false;
            CStatusMessageList msgs;
            if (path == CSettingsSimulator::ValueSelectedDriver())
            {
                if (command == CSettingUtilities::CmdAdd() || command == CSettingUtilities::CmdUpdate())
                {
                    CSimulatorInfo v = value.value<CSimulatorInfo>();
                    changedFlag = (v != this->m_selectedPlugin);
                    msgs.push_back(CSettingUtilities::valueChangedMessage(changedFlag, "selected driver"));
                    this->m_selectedPlugin = v;
                    return msgs;
                }
                return CSettingUtilities::wrongCommandMessages(command);
            }
            else if (path == CSettingsSimulator::ValueSyncTime())
            {
                if (command == CSettingUtilities::CmdAdd() || command == CSettingUtilities::CmdUpdate())
                {
                    bool v = value.value<bool>();
                    changedFlag = (v != this->m_timeSync);
                    msgs.push_back(CSettingUtilities::valueChangedMessage(changedFlag, "time synchronization"));
                    this->m_timeSync = v;
                    return msgs;
                }
                return CSettingUtilities::wrongCommandMessages(command);
            }
            else if (path == CSettingsSimulator::ValueSyncTimeOffset())
            {
                if (command == CSettingUtilities::CmdAdd() || command == CSettingUtilities::CmdUpdate())
                {
                    CTime v = value.value<CTime>();
                    changedFlag = (v != this->m_timeSyncOffset);
                    msgs.push_back(CSettingUtilities::valueChangedMessage(changedFlag, "time synchronization offset"));
                    this->m_timeSyncOffset = v;
                    return msgs;
                }
                return CSettingUtilities::wrongCommandMessages(command);
            }
            else
            {
                return CSettingUtilities::wrongPathMessages(path);
            }
        }
    } // namespace
} // namespace
