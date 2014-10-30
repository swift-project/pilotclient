/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

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
            m_mappingId(CAircraftMapping::InvalidId), m_proposalId(CAircraftMapping::InvalidId),
            m_lastChanged(-1), m_simulatorInfo(BlackSim::CSimulatorInfo::UnspecifiedSim()),
            m_changed(false)
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
        QVariant CAircraftMapping::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toQVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexMappingId:
                return QVariant(this->m_mappingId);
            case IndexProposalId:
                return QVariant(m_proposalId);
            case IndexAircraftKey:
                return QVariant(m_fsAircraftKey);
            case IndexAircraftDesignator:
                return QVariant(m_aircraftDesignator);
            case IndexAirlineDesignator:
                return QVariant(m_airlineDesignator);
            case IndexAircraftCombinedType:
                return QVariant(m_aircraftCombinedType);
            case IndexWakeTurbulenceCategory:
                return QVariant(m_wakeTurbulenceCategory);
            case IndexAirlineColor:
                return QVariant(this->m_aircraftColor);
            case IndexLastChanged:
                return QVariant(this->getLastChangedFormatted());
            case IndexSimulatorInfo:
                return this->m_simulatorInfo.propertyByIndex(index.copyFrontRemoved());
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        /*
         * Set column's value
         */
        void CAircraftMapping::setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->setPropertyByIndex(variant, index);
                return;
            }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexMappingId:
                {
                    bool ok = false;
                    qint32 id = variant.toInt(&ok);
                    this->m_mappingId = ok ? id : CAircraftMapping::InvalidId;
                }
                break;
            case IndexProposalId:
                {
                    bool ok = false;
                    qint32 id = variant.toInt(&ok);
                    this->m_proposalId = ok ? id : CAircraftMapping::InvalidId;
                }
                break;
            case IndexAircraftKey:
                m_fsAircraftKey = variant.toString();
                break;
            case IndexAircraftDesignator:
                this->setAircraftDesignator(variant.toString());
                break;
            case IndexAirlineDesignator:
                this->setAirlineDesignator(variant.toString());
                break;
            case IndexAircraftCombinedType:
                this->setAircraftCombinedType(variant.toString());
                break;
            case IndexWakeTurbulenceCategory:
                this->setWakeTurbulenceCategory(variant.toString());
                break;
            case IndexAirlineColor:
                this->m_aircraftColor = variant.toString();
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

    } // namespace
} // namespace
