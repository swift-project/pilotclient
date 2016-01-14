/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_WEATHER_CLOUDLAYER_H
#define BLACKMISC_WEATHER_CLOUDLAYER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/aviation/altitude.h"

namespace BlackMisc
{
    namespace Weather
    {
        /*!
         * Value object for a cloud layer
         */
        class BLACKMISC_EXPORT CCloudLayer : public CValueObject<CCloudLayer>
        {
        public:
            //! Cloud coverage
            enum Coverage
            {
                None,
                Few,
                Scattered,
                Broken,
                Overcast
            };

            //! Properties by index
            enum ColumnIndex
            {
                IndexCloudLayer = BlackMisc::CPropertyIndex::GlobalIndexCCloudLayer,
                IndexCeiling,
                IndexCoverage
            };

            //! Default constructor.
            CCloudLayer() = default;

            //! Constructor
            CCloudLayer(BlackMisc::Aviation::CAltitude ceiling, Coverage coverage);

            //! Set base
            void setBase(const BlackMisc::Aviation::CAltitude &base) { m_base = base; }

            //! Get base
            BlackMisc::Aviation::CAltitude getBase() const { return m_base; }

            //! Set ceiling
            void setCeiling(BlackMisc::Aviation::CAltitude ceiling) { m_ceiling = ceiling; }

            //! Get ceiling
            BlackMisc::Aviation::CAltitude getCeiling() const { return m_ceiling; }

            //! Set coverage
            void setCoverage(Coverage coverage) { m_coverage = coverage; }

            //! Get coverage
            Coverage getCoverage() const { return m_coverage; }

            //! Set coverage in %
            void setCoveragePercent(int coverage) { m_coveragePercent = coverage; }

            //! Get coverage in %
            int getCoveragePercent() const { return m_coveragePercent; }

            //! \copydoc CValueObject::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc CValueObject::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CCloudLayer)
            BlackMisc::Aviation::CAltitude m_base;
            BlackMisc::Aviation::CAltitude m_ceiling;
            Coverage m_coverage;
            int m_coveragePercent;
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CCloudLayer)
Q_DECLARE_METATYPE(BlackMisc::Weather::CCloudLayer::Coverage)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Weather::CCloudLayer, (
    attr(o.m_ceiling),
    attr(o.m_coverage)
))

#endif // guard
