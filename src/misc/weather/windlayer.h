// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_WEATHER_WINDLAYER_H
#define SWIFT_MISC_WEATHER_WINDLAYER_H

#include <QMetaType>
#include <QString>

#include "misc/aviation/altitude.h"
#include "misc/metaclass.h"
#include "misc/pq/angle.h"
#include "misc/pq/speed.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::weather, CWindLayer)

namespace swift::misc::weather
{
    /*!
     * Value object for a wind layer
     */
    class SWIFT_MISC_EXPORT CWindLayer : public CValueObject<CWindLayer>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexLevel = swift::misc::CPropertyIndexRef::GlobalIndexCWindLayer,
            IndexDirection,
            IndexDirectionVariable,
            IndexSpeed,
            IndexGustSpeed
        };

        //! Default constructor.
        CWindLayer() = default;

        //! Constructor
        CWindLayer(const swift::misc::aviation::CAltitude &level, const physical_quantities::CAngle &direction,
                   const physical_quantities::CSpeed &speed, const physical_quantities::CSpeed &gustSpeed);

        //! Set level
        void setLevel(const swift::misc::aviation::CAltitude &level) { m_level = level; }

        //! Get level
        swift::misc::aviation::CAltitude getLevel() const { return m_level; }

        //! Set direction
        void setDirection(const physical_quantities::CAngle &main) { m_directionMain = main; }

        //! Set direction
        //! Main direction will not be modified
        void setDirection(const physical_quantities::CAngle &from, const physical_quantities::CAngle &to)
        {
            m_directionFrom = from;
            m_directionTo = to;
        }

        //! Set direction
        void setDirection(const physical_quantities::CAngle &main, const physical_quantities::CAngle &from,
                          const physical_quantities::CAngle &to)
        {
            m_directionMain = main;
            m_directionFrom = from;
            m_directionTo = to;
        }

        //! Get direction
        physical_quantities::CAngle getDirection() const { return m_directionMain; }

        //! Get direction from
        physical_quantities::CAngle getDirectionFrom() const { return m_directionFrom; }

        //! Get direction to
        physical_quantities::CAngle getDirectionTo() const { return m_directionTo; }

        //! Set direction to variable
        void setDirectionVariable(bool variable = true) { m_directionVariable = variable; }

        //! Is direction variable?
        bool isDirectionVariable() const { return m_directionVariable; }

        //! Set speed
        void setSpeed(const physical_quantities::CSpeed &speed) { m_speed = speed; }

        //! Get descriptor
        physical_quantities::CSpeed getSpeed() const { return m_speed; }

        //! Set gust speed
        void setGustSpeed(const physical_quantities::CSpeed &gustSpeed) { m_gustSpeed = gustSpeed; }

        //! Get weather phenomenas
        physical_quantities::CSpeed getGustSpeed() const { return m_gustSpeed; }

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        swift::misc::aviation::CAltitude m_level;
        physical_quantities::CAngle m_directionMain;
        physical_quantities::CAngle m_directionFrom;
        physical_quantities::CAngle m_directionTo;
        bool m_directionVariable = false;
        physical_quantities::CSpeed m_speed;
        physical_quantities::CSpeed m_gustSpeed;

        SWIFT_METACLASS(
            CWindLayer,
            SWIFT_METAMEMBER(level),
            SWIFT_METAMEMBER(directionMain),
            SWIFT_METAMEMBER(directionFrom),
            SWIFT_METAMEMBER(directionTo),
            SWIFT_METAMEMBER(directionVariable),
            SWIFT_METAMEMBER(speed),
            SWIFT_METAMEMBER(gustSpeed));
    };

} // namespace swift::misc::weather

Q_DECLARE_METATYPE(swift::misc::weather::CWindLayer)

#endif // SWIFT_MISC_WEATHER_WINDLAYER_H
