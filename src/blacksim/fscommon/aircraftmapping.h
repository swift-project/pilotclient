/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIM_FSCOMMON_AIRCRAFTMAPPING_H
#define BLACKSIM_FSCOMMON_AIRCRAFTMAPPING_H

#include "aircraftcfgentries.h"
#include "../simulatorinfo.h"
#include "blackmisc/valueobject.h"
#include <QVariant>
#include <QDateTime>

namespace BlackSim
{
    namespace FsCommon
    {

        //! \brief Aircraft mapping class, represents one particular mapping
        class CAircraftMapping : public BlackMisc::CValueObject
        {

        public:
            static const qint32 InvalidId = -1; //!< Invalid mapping id

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const;

            //! \copydoc CValueObject::marshallToDbus
            virtual void marshallToDbus(QDBusArgument &) const override;

            //! \copydoc CValueObject::unmarshallFromDbus
            virtual void unmarshallFromDbus(const QDBusArgument &) override;

            //! \copydoc CValueObject::getMetaTypeId()
            int getMetaTypeId() const;

            //! \copydoc CValueObject::compareImpl
            int compareImpl(const CValueObject &otherBase) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAircraftMapping)
            qint32 m_mappingId; //!< Kind of primary key for this particular mapping
            qint32 m_proposalId; //!< If proposal id of the proposal
            QString m_fsAircraftKey; //!< Id by which the simulator can create the aircraft
            QString m_aircraftDesignator; //!< Aircraft designator such as B737
            QString m_airlineDesignator; //!< Airline designator such as DLH
            QString m_aircraftCombinedType; //!< Engine, type, number of engines L2J, L1P
            QString m_wakeTurbulenceCategory; //!< Wake turbulence category H, L, M
            QString m_aircraftColor; //!< Aircrafts painting designator, could be same as airline or specific
            QString m_lastChanged; //!< Simple timestamp as YYYYMMDDhhmmss
            BlackSim::CSimulatorInfo m_simulatorInfo; //!< Mapping is for simulator
            bool m_changed; //! Changed flag

        public:
            //! \brief Columns
            enum ColumnIndex
            {
                IndexMappingId = 0,
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

            //! \brief Default mapping
            CAircraftMapping();

            /*!
             * \brief Complete constructor
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

            //! \brief Virtual destructor
            virtual ~CAircraftMapping()
            {}

            //! \brief operator ==
            bool operator ==(const CAircraftMapping &otherMapping) const;

            //! \brief operator !=
            bool operator !=(const CAircraftMapping &otherMapping) const;

            //! \brief Mapping id
            qint32 getMappingId() const { return this->m_mappingId; }

            //! \brief Proposal id (if proposal, otherwise <0)
            qint32 getProposalId() const { return this->m_proposalId; }

            //! \brief Aircraft key
            QString getFsAircraftKey() const { return this->m_fsAircraftKey; }

            //! \brief ICAO designator (B737)
            QString getAircraftDesignator() const { return this->m_aircraftDesignator; }

            //! \brief ICAO airline (DLH)
            QString getAirlineDesignator() const { return this->m_airlineDesignator; }

            //! \brief ICAO aircraft type (L2J)
            QString getAircraftCombinedType() const { return this->m_aircraftCombinedType; }

            //! \brief ICAO wake turbulence category (L,M,H)
            QString getWakeTurbulenceCategory() const { return this->m_wakeTurbulenceCategory; }

            //! \brief Painting, basically the airline code for GA planes
            QString getAircraftColor() const { return this->m_aircraftColor; }

            //! \brief Last changed timestamp YYYYMMDDhhmmss
            QString getLastChanged() const { return this->m_lastChanged; }

            //! \brief Last changed timestamp YYYYMMDDhhmmss
            QString getLastChangedFormatted() const;

            //! \brief Simulator
            BlackSim::CSimulatorInfo getSimulatorInfo() const { return this->m_simulatorInfo; }

            //! \brief Simulator
            QString getSimulatorText() const;

            //! \brief Get changed flag
            bool isChanged() const { return this->m_changed; }

            //! \brief Valid data?
            bool isValid() const
            {
                QString v = this->validate();
                return v.isEmpty();
            }

            //! \brief Validate and return error messages, empty string means all OK.
            QString validate() const;

            //! \brief Set mapping id
            void setMappingId(qint32 mappingId) { this->m_mappingId = mappingId; }

            //! \brief Proposal id
            void setProposalId(qint32 proposalId) { this->m_proposalId = proposalId; }

            //! \brief Aircraft key
            void setFsAircraftKey(const QString &aircraftKey) { this->m_fsAircraftKey = aircraftKey; }

            //! \brief ICAO designator (B737)
            void setAircraftDesignator(const QString &icaoDesignator) { this->m_aircraftDesignator = icaoDesignator.toUpper(); }

            //! \brief ICAO airline (DLH)
            void setAirlineDesignator(const QString &airline) { this->m_airlineDesignator = airline.toUpper(); }

            //! \brief ICAO aircraft type (L2J)
            void setAircraftCombinedType(const QString &aircraftType) { this->m_aircraftCombinedType = aircraftType.toUpper(); }

            //! \brief ICAO wake turbulence category
            void setWakeTurbulenceCategory(const QString &wtc) { this->m_wakeTurbulenceCategory = wtc.toUpper(); }

            //! \brief Painting, basically the airline code for GA planes
            void setAircraftColor(const QString &painting) { this->m_aircraftColor = painting; }

            //! \brief Last changed timestamp YYYYMMDDhhmmss
            void setLastChanged(qint32 lastChanged) { this->m_lastChanged = lastChanged; }

            //! \brief Simulator
            void setSimulator(BlackSim::CSimulatorInfo simulator) { this->m_simulatorInfo = simulator; }

            //! \brief Set simulator text
            void setSimulatorText(const QString &simulator);

            //! \brief Set changed flag
            void setChanged(bool changed) { this->m_changed = changed; }

            //! \copydoc CValueObject::propertyByIndex
            QVariant propertyByIndex(int index) const override;

            //! \copydoc CValueObject::setPropertyByIndex()
            void setPropertyByIndex(const QVariant &value, int index) override;

            //! \copydoc CValueObject::toQVariant()
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! \copydoc CValueObject::getValueHash()
            virtual uint getValueHash() const override;

            //! \brief Current UTC timestamp
            static QString currentUtcTimestamp()
            {
                QDateTime dateTime = QDateTime::currentDateTimeUtc();
                QString dateTimeString = dateTime.toString("yyyyMMddhhmmss");
                return dateTimeString;
            }

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::fromJson
            virtual void fromJson(const QJsonObject &json) override;

            //! \brief Register metadata
            static void registerMetadata();

            //! \brief Members
            static const QStringList &jsonMembers();
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackSim::FsCommon::CAircraftMapping, (o.m_mappingId, o.m_proposalId, o.m_fsAircraftKey, o.m_aircraftDesignator, o.m_airlineDesignator, o.m_aircraftCombinedType, o.m_wakeTurbulenceCategory, o.m_aircraftColor, o.m_lastChanged, o.m_simulatorInfo, o.m_changed))
Q_DECLARE_METATYPE(BlackSim::FsCommon::CAircraftMapping)

#endif // guard
