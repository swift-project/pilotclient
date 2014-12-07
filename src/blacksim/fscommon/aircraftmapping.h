/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIM_FSCOMMON_AIRCRAFTMAPPING_H
#define BLACKSIM_FSCOMMON_AIRCRAFTMAPPING_H

#include "aircraftcfgentries.h"
#include "blacksim/simulatorinfo.h"
#include "blackmisc/valueobject.h"
#include <QDateTime>

namespace BlackSim
{
    namespace FsCommon
    {
        //! Aircraft mapping class, represents one particular mapping
        class CAircraftMapping : public BlackMisc::CValueObjectStdTuple<CAircraftMapping>
        {
        public:
            static const qint32 InvalidId = -1; //!< Invalid mapping id

            //! Columns
            enum ColumnIndex
            {
                IndexMappingId = BlackMisc::CPropertyIndex::GlobalIndexCAircraftMapping,
                IndexProposalId,
                IndexAircraftKey,
                IndexAircraftDesignator,
                IndexAirlineDesignator,
                IndexAircraftCombinedType,
                IndexWakeTurbulenceCategory,
                IndexAirlineColor,
                IndexLastChanged,
                IndexSimulatorInfo
            };

            //! Default mapping
            CAircraftMapping();

            /*!
             * Complete constructor
             * \param mappingId
             * \param proposalId
             * \param fsAircraftKey
             * \param icaoAircraftDesignator
             * \param icaoAirline
             * \param icaoAircraftType
             * \param icaoWakeTurbulenceCategory
             * \param painting
             * \param lastChanged
             * \param simulator
             */
            CAircraftMapping(qint32 mappingId, qint32 proposalId, const QString &fsAircraftKey, const QString &icaoAircraftDesignator, const QString &icaoAirline, const QString &icaoAircraftType, const QString &icaoWakeTurbulenceCategory, const QString &painting, const QString &lastChanged, CSimulatorInfo simulator);

            //! Mapping id
            qint32 getMappingId() const { return this->m_mappingId; }

            //! Proposal id (if proposal, otherwise <0)
            qint32 getProposalId() const { return this->m_proposalId; }

            //! Aircraft key
            QString getFsAircraftKey() const { return this->m_fsAircraftKey; }

            //! ICAO designator (B737)
            QString getAircraftDesignator() const { return this->m_aircraftDesignator; }

            //! ICAO airline (DLH)
            QString getAirlineDesignator() const { return this->m_airlineDesignator; }

            //! ICAO aircraft type (L2J)
            QString getAircraftCombinedType() const { return this->m_aircraftCombinedType; }

            //! ICAO wake turbulence category (L,M,H)
            QString getWakeTurbulenceCategory() const { return this->m_wakeTurbulenceCategory; }

            //! Painting, basically the airline code for GA planes
            QString getAircraftColor() const { return this->m_aircraftColor; }

            //! Last changed timestamp YYYYMMDDhhmmss
            QString getLastChanged() const { return this->m_lastChanged; }

            //! Last changed timestamp YYYYMMDDhhmmss
            QString getLastChangedFormatted() const;

            //! Simulator
            BlackSim::CSimulatorInfo getSimulatorInfo() const { return this->m_simulatorInfo; }

            //! Simulator
            QString getSimulatorText() const;

            //! Valid data?
            bool isValid() const
            {
                QString v = this->validate();
                return v.isEmpty();
            }

            //! Validate and return error messages, empty string means all OK.
            QString validate() const;

            //! Set mapping id
            void setMappingId(qint32 mappingId) { this->m_mappingId = mappingId; }

            //! Proposal id
            void setProposalId(qint32 proposalId) { this->m_proposalId = proposalId; }

            //! Aircraft key
            void setFsAircraftKey(const QString &aircraftKey) { this->m_fsAircraftKey = aircraftKey; }

            //! ICAO designator (B737)
            void setAircraftDesignator(const QString &icaoDesignator) { this->m_aircraftDesignator = icaoDesignator.toUpper(); }

            //! ICAO airline (DLH)
            void setAirlineDesignator(const QString &airline) { this->m_airlineDesignator = airline.toUpper(); }

            //! ICAO aircraft type (L2J)
            void setAircraftCombinedType(const QString &aircraftType) { this->m_aircraftCombinedType = aircraftType.toUpper(); }

            //! ICAO wake turbulence category
            void setWakeTurbulenceCategory(const QString &wtc) { this->m_wakeTurbulenceCategory = wtc.toUpper(); }

            //! Painting, basically the airline code for GA planes
            void setAircraftColor(const QString &painting) { this->m_aircraftColor = painting; }

            //! Last changed timestamp YYYYMMDDhhmmss
            void setLastChanged(qint32 lastChanged) { this->m_lastChanged = lastChanged; }

            //! Simulator
            void setSimulator(BlackSim::CSimulatorInfo simulator) { this->m_simulatorInfo = simulator; }

            //! Set simulator text
            void setSimulatorText(const QString &simulator);

            //! \copydoc CValueObject::propertyByIndex
            BlackMisc::CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex()
            void setPropertyByIndex(const BlackMisc::CVariant &value, const BlackMisc::CPropertyIndex &index) override;

            //! \copydoc CValueObject::getValueHash()
            virtual uint getValueHash() const override;

            //! Current UTC timestamp
            static QString currentUtcTimestamp()
            {
                QDateTime dateTime = QDateTime::currentDateTimeUtc();
                QString dateTimeString = dateTime.toString("yyyyMMddhhmmss");
                return dateTimeString;
            }

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAircraftMapping)
            qint32 m_mappingId;  //!< Kind of primary key for this particular mapping
            qint32 m_proposalId; //!< If proposal id of the proposal
            QString m_fsAircraftKey;          //!< Id by which the simulator can create the aircraft
            QString m_aircraftDesignator;     //!< Aircraft designator such as B737
            QString m_airlineDesignator;      //!< Airline designator such as DLH
            QString m_aircraftCombinedType;   //!< Engine, type, number of engines L2J, L1P
            QString m_wakeTurbulenceCategory; //!< Wake turbulence category H, L, M
            QString m_aircraftColor; //!< Aircrafts painting designator, could be same as airline or specific
            QString m_lastChanged;   //!< Simple timestamp as YYYYMMDDhhmmss
            BlackSim::CSimulatorInfo m_simulatorInfo; //!< Mapping is for simulator
            bool m_changed; //! Changed flag
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackSim::FsCommon::CAircraftMapping, (o.m_mappingId, o.m_proposalId, o.m_fsAircraftKey, o.m_aircraftDesignator, o.m_airlineDesignator, o.m_aircraftCombinedType, o.m_wakeTurbulenceCategory, o.m_aircraftColor, o.m_lastChanged, o.m_simulatorInfo, o.m_changed))
Q_DECLARE_METATYPE(BlackSim::FsCommon::CAircraftMapping)

#endif // guard
