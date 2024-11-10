// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_TRACK_H
#define SWIFT_MISC_AVIATION_TRACK_H

#include "misc/swiftmiscexport.h"
#include "misc/mixin/mixincompare.h"
#include "misc/mixin/mixindbus.h"
#include "misc/mixin/mixinhash.h"
#include "misc/mixin/mixinjson.h"
#include "misc/metaclass.h"
#include "misc/pq/angle.h"
#include "misc/pq/units.h"
#include "misc/mixin/mixinindex.h"
#include "misc/mixin/mixinstring.h"
#include "misc/mixin/mixinmetatype.h"

#include <QMetaType>
#include <QString>
#include <QtGlobal>

namespace swift::misc::aviation
{

    /*!
     * \brief Track as used in aviation, can be true or magnetic Track
     * \remarks Intentionally allowing +/- swift::misc::physical_quantities::CAngle ,
     *          and >= / <= CAngle.
     */
    class SWIFT_MISC_EXPORT CTrack :
        public physical_quantities::CAngle,
        public mixin::MetaType<CTrack>,
        public mixin::EqualsByMetaClass<CTrack>,
        public mixin::CompareByMetaClass<CTrack>,
        public mixin::HashByMetaClass<CTrack>,
        public mixin::DBusByMetaClass<CTrack>,
        public mixin::DataStreamByMetaClass<CTrack>,
        public mixin::JsonByMetaClass<CTrack>,
        public mixin::String<CTrack>,
        public mixin::Index<CTrack>
    {
    public:
        //! Base type
        using base_type = physical_quantities::CAngle;

        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CTrack)
        SWIFT_MISC_DECLARE_USING_MIXIN_STRING(CTrack)
        SWIFT_MISC_DECLARE_USING_MIXIN_DBUS(CTrack)
        SWIFT_MISC_DECLARE_USING_MIXIN_DATASTREAM(CTrack)
        SWIFT_MISC_DECLARE_USING_MIXIN_JSON(CTrack)
        SWIFT_MISC_DECLARE_USING_MIXIN_INDEX(CTrack)

        /*!
         * Enum type to distinguish between true north and magnetic north
         */
        enum ReferenceNorth
        {
            Magnetic = 0, //!< magnetic north
            True = 1 //!< true north
        };

        //! \brief Default constructor: 0 Track magnetic
        CTrack() : CAngle(0, swift::misc::physical_quantities::CAngleUnit::rad()), m_north(Magnetic) {}

        //! \brief Constructor
        CTrack(double value, ReferenceNorth north, const swift::misc::physical_quantities::CAngleUnit &unit) : CAngle(value, unit), m_north(north) {}

        //! \brief Constructor by CAngle
        CTrack(const swift::misc::physical_quantities::CAngle &track, ReferenceNorth north) : CAngle(track), m_north(north) {}

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

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        ReferenceNorth m_north; //!< magnetic or true?

        SWIFT_METACLASS(
            CTrack,
            SWIFT_METAMEMBER(north));
    };
}

Q_DECLARE_METATYPE(swift::misc::aviation::CTrack)

#endif
