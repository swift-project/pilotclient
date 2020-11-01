/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_WEATHER_VISIBILITYLAYER_H
#define BLACKMISC_WEATHER_VISIBILITYLAYER_H

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Weather
    {
        /*!
         * Value object for a visibility layer
         */
        class BLACKMISC_EXPORT CVisibilityLayer : public CValueObject<CVisibilityLayer>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexBase = BlackMisc::CPropertyIndex::GlobalIndexCVisibilityLayer,
                IndexTop,
                IndexVisibility,
            };

            //! Default constructor.
            CVisibilityLayer() = default;

            //! Constructor
            CVisibilityLayer(const BlackMisc::Aviation::CAltitude &base,
                             const BlackMisc::Aviation::CAltitude &top,
                             const PhysicalQuantities::CLength &visibility);

            //! Set base
            void setBase(const BlackMisc::Aviation::CAltitude &base) { m_base = base; }

            //! Get base
            BlackMisc::Aviation::CAltitude getBase() const { return m_base; }

            //! Set top
            void setTop(const BlackMisc::Aviation::CAltitude &top) { m_top = top; }

            //! Get top
            BlackMisc::Aviation::CAltitude getTop() const { return m_top; }

            //! Set visibility
            void setVisibility(const PhysicalQuantities::CLength &visibility) { m_visibility = visibility; }

            //! Get visibility
            PhysicalQuantities::CLength getVisibility() const { return m_visibility; }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            BlackMisc::Aviation::CAltitude m_base;
            BlackMisc::Aviation::CAltitude m_top;
            PhysicalQuantities::CLength m_visibility { 100 , PhysicalQuantities::CLengthUnit::km() };

            BLACK_METACLASS(
                CVisibilityLayer,
                BLACK_METAMEMBER(base),
                BLACK_METAMEMBER(top),
                BLACK_METAMEMBER(visibility)
            );
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CVisibilityLayer)

#endif // guard
