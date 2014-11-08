/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVTRACK_H
#define BLACKMISC_AVTRACK_H

#include "blackmisc/pqangle.h"

namespace BlackMisc
{
    namespace Aviation { class CTrack; }

    //! \private
    template <> struct CValueObjectStdTuplePolicy<Aviation::CTrack> : public CValueObjectStdTuplePolicy<>
    {
        using Compare = Policy::Compare::MetaTuple;
        using Hash = Policy::Hash::MetaTuple;
        using DBus = Policy::DBus::MetaTuple;
        using Json = Policy::Json::MetaTuple;
    };

    namespace Aviation
    {
        /*!
         * \brief Track as used in aviation, can be true or magnetic Track
         * \remarks Intentionally allowing +/- BlackMisc::PhysicalQuantities::CAngle ,
         *          and >= / <= CAngle.
         */
        class CTrack : public CValueObjectStdTuple<CTrack, PhysicalQuantities::CAngle>
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

            //! \brief Default constructor: 0 Track magnetic
            CTrack() : CValueObjectStdTuple(0, BlackMisc::PhysicalQuantities::CAngleUnit::rad()), m_north(Magnetic) {}

            //! \brief Constructor
            CTrack(double value, ReferenceNorth north, const BlackMisc::PhysicalQuantities::CAngleUnit &unit) : CValueObjectStdTuple(value, unit), m_north(north) {}

            //! \brief Constructor by CAngle
            CTrack(BlackMisc::PhysicalQuantities::CAngle track, ReferenceNorth north) : CValueObjectStdTuple(track), m_north(north) {}

            //! \brief Magnetic Track?
            bool isMagneticTrack() const
            {
                (void)QT_TRANSLATE_NOOP("Aviation", "magnetic");
                return Magnetic == this->m_north;
            }

            //! \brief True Track?
            bool isTrueTrack() const
            {
                (void)QT_TRANSLATE_NOOP("Aviation", "true");
                return True == this->m_north;
            }

            //! \brief Get reference north (magnetic or true)
            ReferenceNorth getReferenceNorth() const { return m_north; }

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CTrack)
            ReferenceNorth m_north; //!< magnetic or true?
        };
    }
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CTrack, (o.m_north))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CTrack)

#endif // guard
