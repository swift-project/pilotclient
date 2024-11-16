// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_HEADING_H
#define SWIFT_MISC_AVIATION_HEADING_H

#include <QMetaType>
#include <QString>

#include "misc/metaclass.h"
#include "misc/mixin/mixincompare.h"
#include "misc/mixin/mixindbus.h"
#include "misc/mixin/mixinhash.h"
#include "misc/mixin/mixinindex.h"
#include "misc/mixin/mixinjson.h"
#include "misc/mixin/mixinmetatype.h"
#include "misc/mixin/mixinstring.h"
#include "misc/pq/angle.h"
#include "misc/pq/units.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::aviation
{
    /*!
     * Heading as used in aviation, can be true or magnetic heading
     * \remarks Intentionally allowing +/- CAngle , and >= / <= CAngle.
     */
    class SWIFT_MISC_EXPORT CHeading :
        public physical_quantities::CAngle,
        public mixin::MetaType<CHeading>,
        public mixin::EqualsByMetaClass<CHeading>,
        public mixin::CompareByMetaClass<CHeading>,
        public mixin::HashByMetaClass<CHeading>,
        public mixin::DBusByMetaClass<CHeading>,
        public mixin::DataStreamByMetaClass<CHeading>,
        public mixin::JsonByMetaClass<CHeading>,
        public mixin::String<CHeading>,
        public mixin::Index<CHeading>
    {
    public:
        //! Base type
        using base_type = physical_quantities::CAngle;

        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CHeading)
        SWIFT_MISC_DECLARE_USING_MIXIN_STRING(CHeading)
        SWIFT_MISC_DECLARE_USING_MIXIN_DBUS(CHeading)
        SWIFT_MISC_DECLARE_USING_MIXIN_DATASTREAM(CHeading)
        SWIFT_MISC_DECLARE_USING_MIXIN_JSON(CHeading)
        SWIFT_MISC_DECLARE_USING_MIXIN_INDEX(CHeading)

        //! Enum type to distinguish between true north and magnetic north
        enum ReferenceNorth
        {
            Magnetic = 0, //!< magnetic north
            True = 1 //!< true north
        };

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Default constructor: 0
        CHeading() : CAngle(0, physical_quantities::CAngleUnit::rad()), m_north(True) {}

        //! Constructor
        CHeading(double value, const physical_quantities::CAngleUnit &unit) : CAngle(value, unit), m_north(True) {}

        //! Constructor
        CHeading(double value, ReferenceNorth north, const physical_quantities::CAngleUnit &unit)
            : CAngle(value, unit), m_north(north)
        {}

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

        SWIFT_METACLASS(
            CHeading,
            SWIFT_METAMEMBER(north));
    };
} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CHeading)
Q_DECLARE_METATYPE(swift::misc::aviation::CHeading::ReferenceNorth)

#endif // guard
