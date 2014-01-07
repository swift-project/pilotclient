/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_AIRCRAFT_H
#define BLACKMISC_AIRCRAFT_H
#include "nwuser.h"
#include "avaircraftsituation.h"
#include "avaircrafticao.h"
#include "avcallsign.h"
#include "aviotransponder.h"
#include "aviocomsystem.h"
#include "valueobject.h"

namespace BlackMisc
{
    namespace Aviation
    {
        /*!
         * Value object encapsulating information of an aircraft
         */
        class CAircraft : public BlackMisc::CValueObject, public BlackMisc::Geo::ICoordinateGeodetic
        {
        public:
            /*!
             * Default constructor.
             */
            CAircraft() : m_distanceToPlane(-1.0, BlackMisc::PhysicalQuantities::CLengthUnit::NM()) {}

            /*!
             * Constructor.
             */
            CAircraft(const QString &callsign, const BlackMisc::Network::CUser &user, const CAircraftSituation &situation)
                : m_callsign(callsign), m_pilot(user), m_situation(situation), m_distanceToPlane(-1.0, BlackMisc::PhysicalQuantities::CLengthUnit::NM()) {}

            /*!
             * \brief QVariant, required for DBus QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * Get callsign.
             * \return
             */
            const CCallsign &getCallsign() const { return m_callsign; }

            /*!
             * Get callsign.
             * \return
             */
            QString getCallsignAsString() const { return m_callsign.asString(); }

            /*!
             * Set callsign
             * \param callsign
             */
            void setCallsign(const CCallsign &callsign) { this->m_callsign = callsign; }

            /*!
             * Get situation.
             * \return
             */
            const CAircraftSituation &getSituation() const { return m_situation; }

            /*!
             * Set situation.
             * \param
             */
            void setSituation(const CAircraftSituation &situation) { m_situation = situation; }

            /*!
             * Get user
             * \return
             */
            const BlackMisc::Network::CUser &getPilot() const { return m_pilot; }

            /*!
             * \brief Set user
             * \param password
             */
            void setPilot(const BlackMisc::Network::CUser &user) { m_pilot = user; }

            /*!
             * Get ICAO info
             * \return
             */
            const CAircraftIcao &getIcaoInfo() const { return m_icao; }

            /*!
             * \brief Set ICAO info
             * \param icao
             */
            void setIcaoInfo(const CAircraftIcao &icao) { m_icao = icao; }

            /*!
             * Get the distance to own plane
             * \return
             */
            const BlackMisc::PhysicalQuantities::CLength &getDistanceToPlane() const { return m_distanceToPlane; }

            /*!
             * Set distance to own plane
             * \param range
             */
            void setDistanceToPlane(const BlackMisc::PhysicalQuantities::CLength &distance) { this->m_distanceToPlane = distance; }

            /*!
             * \brief Valid distance?
             * \return
             */
            bool hasValidDistance() const { return !this->m_distanceToPlane.isNegativeWithEpsilonConsidered();}

            /*!
             * \brief Calculcate distance to plane, set it, and also return it
             * \param position
             * \return
             */
            const BlackMisc::PhysicalQuantities::CLength &calculcateDistanceToPlane(const BlackMisc::Geo::CCoordinateGeodetic &position);

            /*!
             * \brief Get position
             * \return
             */
            BlackMisc::Geo::CCoordinateGeodetic getPosition() const { return this->m_situation.getPosition(); }

            /*!
             * \brief Set position
             * \param position
             */
            void setPosition(const BlackMisc::Geo::CCoordinateGeodetic &position) { this->m_situation.setPosition(position); }

            /*!
             * \brief Get altitude
             * \return
             */
            const BlackMisc::Aviation::CAltitude &getAltitude() const { return this->m_situation.getAltitude(); }

            /*!
             * \brief Set altitude
             * \param altitude
             */
            void setAltitude(const BlackMisc::Aviation::CAltitude &altitude) { this->m_situation.setAltitude(altitude); }

            /*!
             * \brief Get groundspeed
             * \return
             */
            const BlackMisc::PhysicalQuantities::CSpeed &getGroundSpeed() const { return this->m_situation.getGroundSpeed(); }

            /*!
             * \brief Get latitude
             * \return
             */
            virtual const BlackMisc::Geo::CLatitude &latitude() const { return this->m_situation.latitude(); }

            /*!
             * \brief Get longitude
             * \return
             */
            virtual const BlackMisc::Geo::CLongitude &longitude() const { return this->m_situation.longitude(); }

            /*!
             * \brief Get height
             * \return
             */
            const BlackMisc::PhysicalQuantities::CLength &getHeight() const { return this->m_situation.getHeight(); }

            /*!
             * \brief Get heading
             * \return
             */
            const BlackMisc::Aviation::CHeading &getHeading() const { return this->m_situation.getHeading(); }

            /*!
             * \brief Get pitch
             * \return
             */
            const BlackMisc::PhysicalQuantities::CAngle &getPitch() const { return this->m_situation.getPitch(); }

            /*!
             * \brief Get bank
             * \return
             */
            const BlackMisc::PhysicalQuantities::CAngle &getBank() const { return this->m_situation.getBank(); }

            /*!
             * \brief Get COM1 system
             * \return
             */
            const BlackMisc::Aviation::CComSystem &getCom1System() const { return this->m_com1system; }

            /*!
             * \brief Get COM2 system
             * \return
             */
            const BlackMisc::Aviation::CComSystem &getCom2System() const { return this->m_com2system; }

            /*!
             * \brief Set COM1 system
             */
            void setCom1System(const CComSystem &comSystem) { this->m_com1system = comSystem; }

            /*!
             * \brief Set COM2 system
             */
            void setCom2System(const CComSystem &comSystem) { this->m_com2system = comSystem; }

            /*!
             * \brief Get transponder
             * \return
             */
            const BlackMisc::Aviation::CTransponder &getTransponder() const { return this->m_transponder; }

            /*!
             * \brief Set transponder
             */
            void setTransponder(const CTransponder &transponder) { this->m_transponder = transponder; }

            /*!
             * \brief Get transponder code
             * \return
             */
            QString getTransponderCodeFormatted() const { return this->m_transponder.getTransponderCodeFormatted(); }

            /*!
             * \brief Get transponder code
             * \return
             */
            qint32 getTransponderCode() const { return this->m_transponder.getTransponderCode(); }

            /*!
             * \brief Get transponder mode
             * \return
             */
            BlackMisc::Aviation::CTransponder::TransponderMode getTransponderMode() const { return this->m_transponder.getTransponderMode(); }

            /*!
             * \brief Is valid for login
             * \return
             */
            bool isValidForLogin() const;

            /*!
             * \brief Meaningful default settings for COM Systems
             */
            void initComSystems();

            /*!
             * \brief Meaningful default settings for Transponder
             */
            void initTransponder();

            /*!
             * \brief Equal operator ==
             * \param other
             * \return
             */
            bool operator ==(const CAircraft &other) const;

            /*!
             * \brief Unequal operator ==
             * \param other
             * \return
             */
            bool operator !=(const CAircraft &other) const;

            /*!
             * \brief Value hash
             */
            virtual uint getValueHash() const;

            /*!
             * \copydoc BlackObject::compare
             */
            virtual int compare(const QVariant &qv) const;

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();

            /*!
             * \brief Properties by index
             */
            enum ColumnIndex
            {
                IndexCallsign = 0,
                IndexCallsignAsString,
                IndexCallsignAsStringAsSet,
                IndexPilotId,
                IndexPilotRealName,
                IndexDistance,
                IndexCom1System,
                IndexFrequencyCom1,
                IndexTransponder,
                IndexTansponderFormatted,
                IndexSituation,
                IndexIcao
            };

            /*!
             * \brief Property by index
             * \param index
             * \return
             */
            virtual QVariant propertyByIndex(int index) const;

            /*!
             * \brief Property by index as string
             * \param index
             * \param i18n
             * \return
             */
            virtual QString propertyByIndexAsString(int index, bool i18n) const;

            /*!
             * \brief Property by index (setter)
             * \param variant
             * \param index
             */
            virtual void setPropertyByIndex(const QVariant &variant, int index);

        protected:
            /*!
             * \brief Rounded value as string
             * \param i18n
             * \return
             */
            virtual QString convertToQString(bool i18n = false) const;

            /*!
             * \brief Stream to DBus <<
             * \param argument
             */
            virtual void marshallToDbus(QDBusArgument &argument) const;

            /*!
             * \brief Stream from DBus >>
             * \param argument
             */
            virtual void unmarshallFromDbus(const QDBusArgument &argument);

        private:
            CCallsign m_callsign;
            BlackMisc::Network::CUser m_pilot;
            CAircraftSituation m_situation;
            BlackMisc::Aviation::CComSystem m_com1system;
            BlackMisc::Aviation::CComSystem m_com2system;
            BlackMisc::Aviation::CTransponder m_transponder;
            CAircraftIcao m_icao;
            BlackMisc::PhysicalQuantities::CLength m_distanceToPlane;


        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraft)

#endif // guard
