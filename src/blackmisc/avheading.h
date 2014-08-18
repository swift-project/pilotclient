/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVHEADING_H
#define BLACKMISC_AVHEADING_H

#include "blackmisc/pqangle.h"

namespace BlackMisc
{
    namespace Aviation
    {
        /*!
         * \brief Heading as used in aviation, can be true or magnetic heading
         * \remarks Intentionally allowing +/- CAngle , and >= / <= CAngle.
         */
        class CHeading : public BlackMisc::PhysicalQuantities::CAngle
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
            BLACK_ENABLE_TUPLE_CONVERSION(CHeading)
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
            //! \brief Default constructor: 0 heading true
            CHeading() : CAngle(0, BlackMisc::PhysicalQuantities::CAngleUnit::rad()), m_north(Magnetic) {}

            /*!
             * \brief Constructor
             * \param value
             * \param north
             * \param unit
             */
            CHeading(double value, ReferenceNorth north, const BlackMisc::PhysicalQuantities::CAngleUnit &unit) : CAngle(value, unit), m_north(north) {}

            //! \brief Constructor by CAngle
            CHeading(CAngle heading, ReferenceNorth north) : CAngle(heading), m_north(north) {}

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! \brief Equal operator ==
            bool operator ==(const CHeading &other) const;

            //! \brief Unequal operator !=
            bool operator !=(const CHeading &other) const;

            //! \brief Magnetic heading?
            bool isMagneticHeading() const { return Magnetic == this->m_north; }

            //! \brief True heading?
            bool isTrueHeading() const { return True == this->m_north; }

            //! \brief Get reference north (magnetic or true)
            ReferenceNorth getReferenceNorth() const { return m_north; }

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::fromQVariant
            virtual void fromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::fromJson
            void fromJson(const QJsonObject &json) override;

            //! \brief Register metadata
            static void registerMetadata();

            //! \copydoc TupleConverter<>::jsonMembers()
            static const QStringList &jsonMembers();
        };

    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CHeading, (o.m_north))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CHeading)

#endif // guard
