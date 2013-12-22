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
            ReferenceNorth m_north; //!< magnetic or true?

        protected:
            /*!
             * \brief Specific stream operation for Track
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

        public:
            /*!
             * \brief Default constructor: 0 Track magnetic
             */
            CTrack() : BlackMisc::PhysicalQuantities::CAngle(0, BlackMisc::PhysicalQuantities::CAngleUnit::rad()), m_north(Magnetic) {}

            /*!
             * \brief Constructor
             * \param value
             * \param north
             * \param unit
             */
            CTrack(double value, ReferenceNorth north, const BlackMisc::PhysicalQuantities::CAngleUnit &unit) : BlackMisc::PhysicalQuantities::CAngle(value, unit), m_north(north) {}

            /*!
             * \brief Constructor by CAngle
             * \param track
             * \param north
             */
            CTrack(BlackMisc::PhysicalQuantities::CAngle track, ReferenceNorth north) : BlackMisc::PhysicalQuantities::CAngle(track), m_north(north) {}

            /*!
             * \brief Equal operator ==
             * \param other
             * \return
             */
            bool operator ==(const CTrack &other) const;

            /*!
             * \brief Unequal operator ==
             * \param other
             * \return
             */
            bool operator !=(const CTrack &other) const;

            /*!
             * \brief Magnetic Track?
             * \return
             */
            bool isMagneticTrack() const
            {
                return Magnetic == this->m_north;
                (void)QT_TRANSLATE_NOOP("Aviation", "magnetic");
            }

            /*!
             * \brief True Track?
             * \return
             */
            bool isTrueTrack() const
            {
                return True == this->m_north;
                (void)QT_TRANSLATE_NOOP("Aviation", "true");
            }

            /*!
             * \brief Get reference north (magnetic or true)
             * \return
             */
            ReferenceNorth getReferenceNorth() const { return m_north; }

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CTrack)

#endif // guard
