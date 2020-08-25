/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_HEADING_H
#define BLACKMISC_AVIATION_HEADING_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/mixin/mixindbus.h"
#include "blackmisc/mixin/mixinhash.h"
#include "blackmisc/mixin/mixinjson.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/mixin/mixinindex.h"
#include "blackmisc/mixin/mixinstring.h"
#include "blackmisc/mixin/mixinmetatype.h"

#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Aviation
    {
        /*!
         * Heading as used in aviation, can be true or magnetic heading
         * \remarks Intentionally allowing +/- CAngle , and >= / <= CAngle.
         */
        class BLACKMISC_EXPORT CHeading :
            public PhysicalQuantities::CAngle,
            public Mixin::MetaType<CHeading>,
            public Mixin::EqualsByMetaClass<CHeading>,
            public Mixin::CompareByMetaClass<CHeading>,
            public Mixin::HashByMetaClass<CHeading>,
            public Mixin::DBusByMetaClass<CHeading>,
            public Mixin::DataStreamByMetaClass<CHeading>,
            public Mixin::JsonByMetaClass<CHeading>,
            public Mixin::String<CHeading>,
            public Mixin::Index<CHeading>
        {
        public:
            //! Base type
            using base_type = PhysicalQuantities::CAngle;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CHeading)
            BLACKMISC_DECLARE_USING_MIXIN_STRING(CHeading)
            BLACKMISC_DECLARE_USING_MIXIN_DBUS(CHeading)
            BLACKMISC_DECLARE_USING_MIXIN_DATASTREAM(CHeading)
            BLACKMISC_DECLARE_USING_MIXIN_JSON(CHeading)
            BLACKMISC_DECLARE_USING_MIXIN_INDEX(CHeading)

            //! Enum type to distinguish between true north and magnetic north
            enum ReferenceNorth
            {
                Magnetic = 0,   //!< magnetic north
                True = 1        //!< true north
            };

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! Default constructor: 0
            CHeading() : CAngle(0, PhysicalQuantities::CAngleUnit::rad()), m_north(True) {}

            //! Constructor
            CHeading(double value, const PhysicalQuantities::CAngleUnit &unit) : CAngle(value, unit), m_north(True) {}

            //! Constructor
            CHeading(double value, ReferenceNorth north, const PhysicalQuantities::CAngleUnit &unit) : CAngle(value, unit), m_north(north) {}

            //! Constructor by CAngle
            CHeading(const CAngle &heading, ReferenceNorth north) : CAngle(heading), m_north(north) {}

            //! Magnetic heading?
            bool isMagneticHeading() const { return Magnetic == this->m_north; }

            //! True heading?
            bool isTrueHeading() const { return True == this->m_north; }

            //! Get reference north (magnetic or true)
            ReferenceNorth getReferenceNorth() const { return m_north; }

            //! Normalize to [0, 359.99]
            void normalizeTo360Degrees();

            //! Normalize to +- 180deg, [-179.99, 180.0]
            void normalizeToPlusMinus180Degrees();

            //! As [-179.99, 180.0] normalized heading
            CHeading normalizedToPlusMinus180Degrees() const;

            //! As [0, 359.99] normalized heading
            CHeading normalizedTo360Degrees() const;

            //! Register metadata
            static void registerMetadata();

        private:
            ReferenceNorth m_north; //!< magnetic or true?

            BLACK_METACLASS(
                CHeading,
                BLACK_METAMEMBER(north)
            );
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Aviation::CHeading)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CHeading::ReferenceNorth)

#endif // guard
