/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIM_FSCOMMON_AIRCRAFTMAPPING_H
#define BLACKSIM_FSCOMMON_AIRCRAFTMAPPING_H

#include "aircraftcfgentries.h"
#include "../simulator.h"
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
            qint32 m_mappingId; //!< Kind of primary key for this particular mapping
            qint32 m_proposalId; //!< If proposal id of the proposal
            QString m_fsAircraftKey; //!< Id by which the simulator can create the aircraft
            QString m_icaoAircraftDesignator; //!< Aircraft designator such as B737
            QString m_icaoAirlineDesignator; //!< Airline designator such as DLH
            QString m_icaoAircraftType; //!< Engine, type, number of engines L2J, L1P
            QString m_icaoWakeTurbulenceCategory; //!< Wake turbulence category H, L, M
            QString m_painting; //!< Aircrafts painting designator, could be same as airline or specific
            QString m_lastChanged; //!< Simple timestamp as YYYYMMDDhhmmss
            BlackSim::CSimulator m_simulator; //!< Mapping is for simulator
            bool m_changed; //! Changed flag

        public:
            //! \brief Columns
            enum ColumnIndex
            {
                IndexMappingId = 0,
                IndexProposalId,
                IndexAircraftKey,
                IndexIcaoAircraftDesignator,
                IndexIcaoAirlineDesignator,
                IndexAircraftType,
                IndexWakeTurbulenceCategory,
                IndexPainting,
                IndexLastChanged,
                IndexSimulator
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
            CAircraftMapping(qint32 mappingId, qint32 proposalId, const QString &fsAircraftKey, const QString &icaoAircraftDesignator, const QString &icaoAirline, const QString &icaoAircraftType, const QString &icaoWakeTurbulenceCategory, const QString &painting, const QString &lastChanged, CSimulator simulator);

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
            QString getIcaoAircraftDesignator() const { return this->m_icaoAircraftDesignator; }

            //! \brief ICAO airline (DLH)
            QString getIcaoAirline() const { return this->m_icaoAirlineDesignator; }

            //! \brief ICAO aircraft type (L2J)
            QString getIcaoAircraftType() const { return this->m_icaoAircraftType; }

            //! \brief ICAO wake turbulence category
            QString getIcaoWakeTurbulenceCategory() const { return this->m_icaoWakeTurbulenceCategory; }

            //! \brief Painting, basically the airline code for GA planes
            QString getPainting() const { return this->m_painting; }

            //! \brief Last changed timestamp YYYYMMDDhhmmss
            QString getLastChanged() const { return this->m_lastChanged; }

            //! \brief Last changed timestamp YYYYMMDDhhmmss
            QString getLastChangedFormatted() const;

            //! \brief Simulator
            BlackSim::CSimulator getSimulator() const { return this->m_simulator; }

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
            void setIcaoAircraftDesignator(const QString &icaoDesignator) { this->m_icaoAircraftDesignator = icaoDesignator.toUpper(); }

            //! \brief ICAO airline (DLH)
            void setIcaoAirline(const QString &airline) { this->m_icaoAirlineDesignator = airline.toUpper(); }

            //! \brief ICAO aircraft type (L2J)
            void setIcaoAircraftType(const QString &aircraftType) { this->m_icaoAircraftType = aircraftType.toUpper(); }

            //! \brief ICAO wake turbulence category
            void setIcaoWakeTurbulenceCategory(const QString &wtc) { this->m_icaoWakeTurbulenceCategory = wtc.toUpper(); }

            //! \brief Painting, basically the airline code for GA planes
            void setPainting(const QString &painting) { this->m_painting = painting; }

            //! \brief Last changed timestamp YYYYMMDDhhmmss
            void setLastChanged(qint32 lastChanged) { this->m_lastChanged = lastChanged; }

            //! \brief Simulator
            void setSimulator(BlackSim::CSimulator simulator) { this->m_simulator = simulator; }

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

            //! \brief Register metadata
            static void registerMetadata();
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackSim::FsCommon::CAircraftMapping)

#endif // guard
