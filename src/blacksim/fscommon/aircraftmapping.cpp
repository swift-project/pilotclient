/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "aircraftmapping.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackSim
{
    namespace FsCommon
    {

        /*
         * Constructor
         */
        CAircraftMapping::CAircraftMapping() :
            m_mappingId(CAircraftMapping::InvalidId), m_proposalId(CAircraftMapping::InvalidId), m_lastChanged(-1), m_simulator(BlackSim::CSimulator::UnspecifiedSim())
        {
            // void
        }

        /*
         * Constructor
         */
        CAircraftMapping::CAircraftMapping(
            qint32 mappingId, qint32 proposalId, const QString &fsAircraftKey, const QString &icaoAircraftDesignator,
            const QString &icaoAirline, const QString &icaoAircraftType, const QString &icaoWakeTurbulenceCategory, const QString &painting,
            const QString &lastChanged, BlackSim::CSimulator simulator) :
            m_mappingId(mappingId), m_proposalId(proposalId), m_fsAircraftKey(fsAircraftKey), m_icaoAircraftDesignator(icaoAircraftDesignator),
            m_icaoAirlineDesignator(icaoAirline), m_icaoAircraftType(icaoAircraftType), m_icaoWakeTurbulenceCategory(icaoWakeTurbulenceCategory), m_painting(painting),
            m_lastChanged(lastChanged), m_simulator(simulator), m_changed(false)
        {
            // void
        }

        /*
         * Operator ==
         */
        bool CAircraftMapping::operator ==(const CAircraftMapping &otherMapping) const
        {
            if (this == &otherMapping) return true;
            return
                this->m_mappingId == otherMapping.m_mappingId &&
                this->m_proposalId == otherMapping.m_proposalId &&
                this->m_changed == otherMapping.m_changed &&
                this->m_icaoAircraftDesignator == otherMapping.m_icaoAircraftDesignator &&
                this->m_icaoAircraftType == otherMapping.m_icaoAircraftType &&
                this->m_icaoAirlineDesignator == otherMapping.m_icaoAirlineDesignator &&
                this->m_icaoWakeTurbulenceCategory == otherMapping.m_icaoWakeTurbulenceCategory;
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
                arg(this->m_icaoAircraftDesignator).arg(this->m_simulator.toQString(i18n));
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
            if (this->m_icaoAircraftDesignator.isEmpty())
                msg.append("Missing designator. ");
            if (this->m_simulator.isUnspecified())
                msg.append("Unknown simulator. ");

            if (this->m_icaoAircraftType.isEmpty())
                msg.append("Missing type. ");
            else if (this->m_icaoAircraftType.length() != 3)
                msg.append("Wrong type length (req.3). ");

            if (this->m_icaoWakeTurbulenceCategory.isEmpty() || this->m_icaoWakeTurbulenceCategory.length() != 1)
                msg.append("Invalid WTC. ");
            else if (this->m_icaoWakeTurbulenceCategory != "L" && this->m_icaoWakeTurbulenceCategory != "M" && this->m_icaoWakeTurbulenceCategory != "H")
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
            QList<uint> hashs;
            hashs << qHash(this->m_changed);
            hashs << qHash(this->m_fsAircraftKey);
            hashs << qHash(this->m_icaoAircraftDesignator);
            hashs << qHash(this->m_icaoAircraftType);
            hashs << qHash(this->m_icaoAirlineDesignator);
            hashs << qHash(this->m_icaoWakeTurbulenceCategory);
            hashs << qHash(this->m_lastChanged);
            hashs << qHash(this->m_mappingId);
            hashs << qHash(this->m_painting);
            hashs << qHash(this->m_proposalId);
            hashs << qHash(this->m_simulator);
            return BlackMisc::calculateHash(hashs, "CAircraftMapping");
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
            case IndexIcaoAircraftDesignator:
                return m_icaoAircraftDesignator;
            case IndexIcaoAirlineDesignator:
                return m_icaoAirlineDesignator;
            case IndexAircraftType:
                return m_icaoAircraftType;
            case IndexWakeTurbulenceCategory:
                return m_icaoWakeTurbulenceCategory;
            case IndexPainting:
                return this->m_painting;
            case IndexLastChanged:
                return this->getLastChangedFormatted();
            case IndexSimulator:
                return this->m_simulator.toQVariant();
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
            case IndexIcaoAircraftDesignator:
                this->setIcaoAircraftDesignator(value.toString());
                changed = true;
                break;
            case IndexIcaoAirlineDesignator:
                this->setIcaoAirline(value.toString());
                changed = true;
                break;
            case IndexAircraftType:
                this->setIcaoAircraftType(value.toString());
                changed = true;
                break;
            case IndexWakeTurbulenceCategory:
                this->setIcaoWakeTurbulenceCategory(value.toString());
                changed = true;
                break;
            case IndexPainting:
                this->m_painting = value.toString();
                changed = true;
                break;
            default:
                changed = false;
                break;
            }
            if (changed) this->setChanged(changed);
        }

        void CAircraftMapping::marshallToDbus(QDBusArgument &argument) const
        {
            argument << this->m_changed;
            argument << this->m_fsAircraftKey;
            argument << this->m_icaoAircraftDesignator;
            argument << this->m_icaoAircraftType;
            argument << this->m_icaoAirlineDesignator;
            argument << this->m_icaoWakeTurbulenceCategory;
            argument << this->m_lastChanged;
            argument << this->m_mappingId;
            argument << this->m_painting;
            argument << this->m_proposalId;
            argument << this->m_simulator;
        }

        /*
         * Unmarshall from DBus
         */
        void CAircraftMapping::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> this->m_changed;
            argument >> this->m_fsAircraftKey;
            argument >> this->m_icaoAircraftDesignator;
            argument >> this->m_icaoAircraftType;
            argument >> this->m_icaoAirlineDesignator;
            argument >> this->m_icaoWakeTurbulenceCategory;
            argument >> this->m_lastChanged;
            argument >> this->m_mappingId;
            argument >> this->m_painting;
            argument >> this->m_proposalId;
            argument >> this->m_simulator;
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
            const CAircraftMapping &other = static_cast<const CAircraftMapping &>(otherBase);
            int result;
            if ((result = compare(this->m_simulator, other.m_simulator))) return result;
            if ((result = this->m_icaoAircraftDesignator.compare(other.m_icaoAircraftDesignator, Qt::CaseInsensitive))) return result;
            if ((result = this->m_icaoAirlineDesignator.compare(other.m_icaoAirlineDesignator, Qt::CaseInsensitive))) return result;
            if ((result = this->m_icaoAircraftType.compare(other.m_icaoAircraftType, Qt::CaseInsensitive))) return result;;
            if ((result = this->m_icaoWakeTurbulenceCategory.compare(other.m_icaoWakeTurbulenceCategory, Qt::CaseInsensitive))) return result;;
            return this->m_fsAircraftKey.compare(other.m_fsAircraftKey, Qt::CaseInsensitive);
        }

        /*
         * Register metadata
         */
        void CAircraftMapping::registerMetadata()
        {
            qRegisterMetaType<CAircraftMapping>();
            qDBusRegisterMetaType<CAircraftMapping>();
        }
    } // namespace
} // namespace
