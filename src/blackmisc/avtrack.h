/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_AVTRACK_H
#define BLACKMISC_AVTRACK_H

#include "blackmisc/pqangle.h"

namespace BlackMisc
{
    namespace Aviation
    {

        /*!
         * \brief Track as used in aviation, can be true or magnetic Track
         * \remarks Intentionally allowing +/- BlackMisc::PhysicalQuantities::CAngle ,
         *          and >= / <= CAngle.
         */
        class CTrack : public BlackMisc::PhysicalQuantities::CAngle
        {
        public:
            /*!
             * Enum type to distinguish between true north and magnetic north
             */
            enum ReferenceNorth
            {
                Magnetic = 0,   //!< magnetic north
                True = 1        //!< true north
            };

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CTrack)
            ReferenceNorth m_north; //!< magnetic or true?

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::marshallFromDbus()
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus()
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

        public:
            //! \brief Default constructor: 0 Track magnetic
            CTrack() : BlackMisc::PhysicalQuantities::CAngle(0, BlackMisc::PhysicalQuantities::CAngleUnit::rad()), m_north(Magnetic) {}

            //! \brief Constructor
            CTrack(double value, ReferenceNorth north, const BlackMisc::PhysicalQuantities::CAngleUnit &unit) : BlackMisc::PhysicalQuantities::CAngle(value, unit), m_north(north) {}

            //! \brief Constructor by CAngle
            CTrack(BlackMisc::PhysicalQuantities::CAngle track, ReferenceNorth north) : BlackMisc::PhysicalQuantities::CAngle(track), m_north(north) {}

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! \brief Equal operator ==
            bool operator ==(const CTrack &other) const;

            //! \brief Unequal operator !=
            bool operator !=(const CTrack &other) const;

            //! \brief Magnetic Track?
            bool isMagneticTrack() const
            {
                return Magnetic == this->m_north;
                (void)QT_TRANSLATE_NOOP("Aviation", "magnetic");
            }

            //! \brief True Track?
            bool isTrueTrack() const
            {
                return True == this->m_north;
                (void)QT_TRANSLATE_NOOP("Aviation", "true");
            }

            //! \brief Get reference north (magnetic or true)
            ReferenceNorth getReferenceNorth() const { return m_north; }

            //! \brief Register metadata
            static void registerMetadata();
        };

    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CTrack, (o.m_north))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CTrack)

#endif // guard
