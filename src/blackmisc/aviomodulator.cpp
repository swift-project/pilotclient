/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviomodulator.h"
#include "blackmisc/aviocomsystem.h"
#include "blackmisc/avionavsystem.h"
#include "blackmisc/avioadfsystem.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/variant.h"

using BlackMisc::PhysicalQuantities::CFrequency;
using BlackMisc::PhysicalQuantities::CFrequencyUnit;

namespace BlackMisc
{
    namespace Aviation
    {
        /*
         * Toggle standby <-> active
         */
        template <class AVIO> void CModulator<AVIO>::toggleActiveStandby()
        {
            CFrequency a = this->m_frequencyActive;
            this->m_frequencyActive = this->m_frequencyStandby;
            this->m_frequencyStandby = a;
        }

        /*
         * Property by index
         */
        template <class AVIO> CVariant CModulator<AVIO>::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toCVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexActiveFrequency:
                return this->getFrequencyActive().propertyByIndex(index.copyFrontRemoved());
            case IndexStandbyFrequency:
                return this->getFrequencyStandby().propertyByIndex(index.copyFrontRemoved());
            case IndexEnabled:
                return CVariant::from(this->isEnabled());
            case IndexInputVolume:
                return CVariant::from(this->getVolumeInput());
            case IndexOutputVolume:
                return CVariant::from(this->getVolumeOutput());
            default:
                return CValueObjectStdTuple<CModulator<AVIO>, CAvionicsBase>::propertyByIndex(index);
            }
        }

        /*
         * Property by index
         */
        template <class AVIO> void CModulator<AVIO>::setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromCVariant(variant);
                return;
            }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexActiveFrequency:
                this->m_frequencyActive.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexStandbyFrequency:
                this->m_frequencyStandby.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexEnabled:
                this->setEnabled(variant.toBool());
                break;
            case IndexInputVolume:
                this->setVolumeInput(variant.toInt());
                break;
            case IndexOutputVolume:
                this->setVolumeOutput(variant.toInt());
                break;
            default:
                CValueObjectStdTuple<CModulator<AVIO>, CAvionicsBase>::setPropertyByIndex(variant, index);
                break;
            }
        }

        // see here for the reason of thess forward instantiations
        // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
        template class CModulator<CComSystem>;
        template class CModulator<CNavSystem>;
        template class CModulator<CAdfSystem>;

    } // namespace
} // namespace
