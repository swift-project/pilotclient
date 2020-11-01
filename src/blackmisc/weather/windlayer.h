/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_WEATHER_WINDLAYER_H
#define BLACKMISC_WEATHER_WINDLAYER_H

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Weather
    {
        /*!
         * Value object for a wind layer
         */
        class BLACKMISC_EXPORT CWindLayer : public CValueObject<CWindLayer>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexLevel = BlackMisc::CPropertyIndexRef::GlobalIndexCWindLayer,
                IndexDirection,
                IndexDirectionVariable,
                IndexSpeed,
                IndexGustSpeed
            };

            //! Default constructor.
            CWindLayer() = default;

            //! Constructor
            CWindLayer(const BlackMisc::Aviation::CAltitude &level, const PhysicalQuantities::CAngle &direction,
                       const PhysicalQuantities::CSpeed &speed, const PhysicalQuantities::CSpeed &gustSpeed);

            //! Set level
            void setLevel(const BlackMisc::Aviation::CAltitude &level) { m_level = level; }

            //! Get level
            BlackMisc::Aviation::CAltitude getLevel() const { return m_level; }

            //! Set direction
            void setDirection(const PhysicalQuantities::CAngle &main) { m_directionMain = main; }

            //! Set direction
            //! Main direction will not be modified
            void setDirection(const PhysicalQuantities::CAngle &from, const PhysicalQuantities::CAngle &to)
            {
                m_directionFrom = from;
                m_directionTo = to;
            }

            //! Set direction
            void setDirection(const PhysicalQuantities::CAngle &main, const PhysicalQuantities::CAngle &from, const PhysicalQuantities::CAngle &to)
            {
                m_directionMain = main;
                m_directionFrom = from;
                m_directionTo = to;
            }

            //! Get direction
            PhysicalQuantities::CAngle getDirection() const { return m_directionMain; }

            //! Get direction from
            PhysicalQuantities::CAngle getDirectionFrom() const { return m_directionFrom; }

            //! Get direction to
            PhysicalQuantities::CAngle getDirectionTo() const { return m_directionTo; }

            //! Set direction to variable
            void setDirectionVariable(bool variable = true) { m_directionVariable = variable; }

            //! Is direction variable?
            bool isDirectionVariable() const { return m_directionVariable; }

            //! Set speed
            void setSpeed(const PhysicalQuantities::CSpeed &speed) { m_speed = speed; }

            //! Get descriptor
            PhysicalQuantities::CSpeed getSpeed() const { return m_speed; }

            //! Set gust speed
            void setGustSpeed(const PhysicalQuantities::CSpeed &gustSpeed) { m_gustSpeed = gustSpeed; }

            //! Get weather phenomenas
            PhysicalQuantities::CSpeed getGustSpeed() const { return m_gustSpeed; }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            BlackMisc::Aviation::CAltitude m_level;
            PhysicalQuantities::CAngle m_directionMain;
            PhysicalQuantities::CAngle m_directionFrom;
            PhysicalQuantities::CAngle m_directionTo;
            bool m_directionVariable = false;
            PhysicalQuantities::CSpeed m_speed;
            PhysicalQuantities::CSpeed m_gustSpeed;

            BLACK_METACLASS(
                CWindLayer,
                BLACK_METAMEMBER(level),
                BLACK_METAMEMBER(directionMain),
                BLACK_METAMEMBER(directionFrom),
                BLACK_METAMEMBER(directionTo),
                BLACK_METAMEMBER(directionVariable),
                BLACK_METAMEMBER(speed),
                BLACK_METAMEMBER(gustSpeed)
            );
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CWindLayer)

#endif // guard
