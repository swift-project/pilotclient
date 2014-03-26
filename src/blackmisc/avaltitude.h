/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_AVALTITUDE_H
#define BLACKMISC_AVALTITUDE_H
#include "blackmisc/pqlength.h"

namespace BlackMisc
{
    namespace Aviation
    {

        /*!
         * \brief Altitude as used in aviation, can be AGL or MSL altitude
         * \remarks Intentionally allowing +/- CLength , and >= / <= CLength.
         */
        class CAltitude : public BlackMisc::PhysicalQuantities::CLength
        {
        public:
            /*!
             * Enum type to distinguish between MSL and AGL
             */
            enum ReferenceDatum
            {
                MeanSeaLevel = 0,   //!< MSL
                AboveGround = 1     //!< AGL
            };

        private:
            ReferenceDatum m_datum; //!< MSL or AGL?

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n) const override;

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

            //! \copydoc CValueObject::marshallToDbus
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

        public:
            //! \brief Default constructor: 0 Altitude true
            CAltitude() : BlackMisc::PhysicalQuantities::CLength(0, BlackMisc::PhysicalQuantities::CLengthUnit::m()), m_datum(MeanSeaLevel) {}

            /*!
             * \brief Constructor
             * \param value
             * \param datum MSL or AGL?
             * \param unit
             */
            CAltitude(double value, ReferenceDatum datum, const BlackMisc::PhysicalQuantities::CLengthUnit &unit) : BlackMisc::PhysicalQuantities::CLength(value, unit), m_datum(datum) {}

            //! \brief Constructor by CLength
            CAltitude(BlackMisc::PhysicalQuantities::CLength altitude, ReferenceDatum datum) : BlackMisc::PhysicalQuantities::CLength(altitude), m_datum(datum) {}

            //! \brief Equal operator ==
            bool operator ==(const CAltitude &other) const;

            //! \brief Unequal operator !=
            bool operator !=(const CAltitude &other) const;

            //! \brief AGL Above ground level?
            bool isAboveGroundLevel() const
            {
                return AboveGround == this->m_datum;
            }

            //! \brief MSL Mean sea level?
            bool isMeanSeaLevel() const
            {
                return MeanSeaLevel == this->m_datum;
            }

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! \brief Get reference datum (MSL or AGL)
            ReferenceDatum getReferenceDatum() const
            {
                return m_datum;
            }

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAltitude)

#endif // guard
