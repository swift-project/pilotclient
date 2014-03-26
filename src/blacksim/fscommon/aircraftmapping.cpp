/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "aircraftmapping.h"
#include "blackmisc/blackmiscfreefunctions.h"

using namespace BlackMisc;

namespace BlackSim
{
    namespace FsCommon
    {

        /*
         * Constructor
         */
        CAircraftMapping::CAircraftMapping() :
            m_mappingId(CAircraftMapping::InvalidId), m_proposalId(CAircraftMapping::InvalidId), m_lastChanged(-1), m_simulatorInfo(BlackSim::CSimulatorInfo::UnspecifiedSim())
        {
            // void
        }

        /*
         * Constructor
         */
        CAircraftMapping::CAircraftMapping(
            qint32 mappingId, qint32 proposalId, const QString &fsAircraftKey, const QString &icaoAircraftDesignator,
            const QString &icaoAirline, const QString &icaoAircraftType, const QString &icaoWakeTurbulenceCategory, const QString &painting,
            const QString &lastChanged, BlackSim::CSimulatorInfo simulator) :
            m_mappingId(mappingId), m_proposalId(proposalId), m_fsAircraftKey(fsAircraftKey), m_aircraftDesignator(icaoAircraftDesignator),
            m_airlineDesignator(icaoAirline), m_aircraftCombinedType(icaoAircraftType), m_wakeTurbulenceCategory(icaoWakeTurbulenceCategory), m_aircraftColor(painting),
            m_lastChanged(lastChanged), m_simulatorInfo(simulator), m_changed(false)
        {
            // void
        }

        /*
         * Operator ==
         */
        bool CAircraftMapping::operator ==(const CAircraftMapping &otherMapping) const
        {
            if (this == &otherMapping) return true;
            return TupleConverter<CAircraftMapping>::toTuple(*this) == TupleConverter<CAircraftMapping>::toTuple(otherMapping);
        }

        /*
         * Operator !=
         */
        bool CAircraftMapping::operator !=(const CAircraftMapping &otherMapping) const
        {
            if (this == &otherMapping) return false;
            return !((*this) == otherMapping);
        }

        /*
         * String for converter
         */
        QString CAircraftMapping::convertToQString(bool i18n) const
        {
            QString s("{%1, %2, %3, %4, %5}");
            s = s.arg(this->m_fsAircraftKey).
                arg(this->m_mappingId).arg(this->m_proposalId).
                arg(this->m_aircraftDesignator).arg(this->m_simulatorInfo.toQString(i18n));
            return s;
        }

        /*
         * Validate data
         */
        QString CAircraftMapping::validate() const
        {
            QString msg;

            if (this->m_fsAircraftKey.isEmpty())
                msg.append("Missing sim key. ");
            if (this->m_aircraftDesignator.isEmpty())
                msg.append("Missing designator. ");
            if (this->m_simulatorInfo.isUnspecified())
                msg.append("Unknown simulator. ");

            if (this->m_aircraftCombinedType.isEmpty())
                msg.append("Missing type. ");
            else if (this->m_aircraftCombinedType.length() != 3)
                msg.append("Wrong type length (req.3). ");

            if (this->m_wakeTurbulenceCategory.isEmpty() || this->m_wakeTurbulenceCategory.length() != 1)
                msg.append("Invalid WTC. ");
            else if (this->m_wakeTurbulenceCategory != "L" && this->m_wakeTurbulenceCategory != "M" && this->m_wakeTurbulenceCategory != "H")
                msg.append("Invalid WTC code. ");

            return msg.trimmed();
        }

        /*
         * Last changed formatted
         */
        QString CAircraftMapping::getLastChangedFormatted() const
        {
            QString ts = this->m_lastChanged;
            QString f("%1-%2-%3 %4:%5:%6");
            return f.arg(ts.left(4)).arg(ts.mid(4, 2)).arg(ts.mid(6, 2))
                   .arg(ts.mid(8, 2)).arg(ts.mid(10, 2)).arg(ts.right(2));
        }

        /*
         * Hash
         */
        uint CAircraftMapping::getValueHash() const
        {
            return qHash(TupleConverter<CAircraftMapping>::toTuple(*this));
        }


        /*
         * Get column
         */
        QVariant CAircraftMapping::propertyByIndex(int index) const
        {
            // non throwing implementation
            switch (index)
            {
            case IndexMappingId:
                return this->m_mappingId;
            case IndexProposalId:
                return m_proposalId;
            case IndexAircraftKey:
                return m_fsAircraftKey;
            case IndexAircraftDesignator:
                return m_aircraftDesignator;
            case IndexAirlineDesignator:
                return m_airlineDesignator;
            case IndexAircraftCombinedType:
                return m_aircraftCombinedType;
            case IndexWakeTurbulenceCategory:
                return m_wakeTurbulenceCategory;
            case IndexAirlineColor:
                return this->m_aircraftColor;
            case IndexLastChanged:
                return this->getLastChangedFormatted();
            case IndexSimulatorInfo:
                return this->m_simulatorInfo.toQVariant();
            default:
                break;
            }

            Q_ASSERT_X(false, "CAircraftMapping", "index unknown");
            QString m = QString("no property, index ").append(QString::number(index));
            return QVariant::fromValue(m);
        }

        /*
         * Set column's value
         */
        void CAircraftMapping::setPropertyByIndex(const QVariant &value, int index)
        {
            // non throwing implementation
            bool changed;

            switch (index)
            {
            case IndexMappingId:
                {
                    bool ok = false;
                    qint32 id = value.toInt(&ok);
                    this->m_mappingId = ok ? id : CAircraftMapping::InvalidId;
                }
                changed = true;
                break;
            case IndexProposalId:
                {
                    bool ok = false;
                    qint32 id = value.toInt(&ok);
                    this->m_proposalId = ok ? id : CAircraftMapping::InvalidId;
                }
                changed = true;
                break;
            case IndexAircraftKey:
                m_fsAircraftKey = value.toString();
                changed = true;
                break;
            case IndexAircraftDesignator:
                this->setAircraftDesignator(value.toString());
                changed = true;
                break;
            case IndexAirlineDesignator:
                this->setAirlineDesignator(value.toString());
                changed = true;
                break;
            case IndexAircraftCombinedType:
                this->setAircraftCombinedType(value.toString());
                changed = true;
                break;
            case IndexWakeTurbulenceCategory:
                this->setWakeTurbulenceCategory(value.toString());
                changed = true;
                break;
            case IndexAirlineColor:
                this->m_aircraftColor = value.toString();
                changed = true;
                break;
            default:
                changed = false;
                break;
            }
            if (changed) this->setChanged(changed);
        }

        /*
         * Marshall to Dbus
         */
        void CAircraftMapping::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CAircraftMapping>::toTuple(*this);
        }

        /*
         * Unmarshall from Dbus
         */
        void CAircraftMapping::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CAircraftMapping>::toTuple(*this);
        }

        /*
         *  Metatype id
         */
        int CAircraftMapping::getMetaTypeId() const
        {
            return qMetaTypeId<CAircraftCfgEntries>();
        }

        /*
         * Compare
         */
        int CAircraftMapping::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CAircraftMapping &>(otherBase);
            return compare(TupleConverter<CAircraftMapping>::toTuple(*this), TupleConverter<CAircraftMapping>::toTuple(other));
        }

        /*
         * Register metadata
         */
        void CAircraftMapping::registerMetadata()
        {
            qRegisterMetaType<CAircraftMapping>();
            qDBusRegisterMetaType<CAircraftMapping>();
        }

        /*
         * To JSON
         */
        QJsonObject CAircraftMapping::toJson() const
        {
            return BlackMisc::serializeJson(CAircraftMapping::jsonMembers(), TupleConverter<CAircraftMapping>::toTuple(*this));
        }

        /*
         * To JSON
         */
        void CAircraftMapping::fromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, CAircraftMapping::jsonMembers(), TupleConverter<CAircraftMapping>::toTuple(*this));
        }

        /*
         * Members
         */
        const QStringList &CAircraftMapping::jsonMembers()
        {
            return TupleConverter<CAircraftMapping>::jsonMembers();
        }

    } // namespace
} // namespace
