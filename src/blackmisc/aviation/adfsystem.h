/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_ADFSYSTEM_H
#define BLACKMISC_AVIATION_ADFSYSTEM_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/modulator.h"
#include <stdexcept>

namespace BlackMisc
{
    namespace Aviation
    {

        //! ADF system ("for NDBs")
        class BLACKMISC_EXPORT CAdfSystem :
            public CModulator<CAdfSystem>,
            public Mixin::MetaType<CAdfSystem>,
            public Mixin::JsonOperators<CAdfSystem>
        {
        public:
            //! Base type
            using base_type = CModulator<CAdfSystem>;

            using Mixin::MetaType<CAdfSystem>::registerMetadata;
            using Mixin::MetaType<CAdfSystem>::getMetaTypeId;
            using Mixin::MetaType<CAdfSystem>::isA;
            using Mixin::MetaType<CAdfSystem>::toCVariant;
            using Mixin::MetaType<CAdfSystem>::toQVariant;
            using Mixin::MetaType<CAdfSystem>::convertFromCVariant;
            using Mixin::MetaType<CAdfSystem>::convertFromQVariant;

            //! Default constructor
            CAdfSystem() = default;

            //! Constructor
            CAdfSystem(const QString &name, const PhysicalQuantities::CFrequency &activeFrequency, const PhysicalQuantities::CFrequency &standbyFrequency = CModulator::FrequencyNotSet()):
                CModulator(name, activeFrequency, standbyFrequency == CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency)
            { }

            //! Valid aviation frequency?
            static bool isValidFrequency(PhysicalQuantities::CFrequency f)
            {
                double fr = f.valueRounded(PhysicalQuantities::CFrequencyUnit::kHz(), 3);
                return fr >= 190.0 && fr <= 1750.0;
            }

            //! ADF1 unit
            static CAdfSystem GetAdf1System(double activeFrequencyKHz, double standbyFrequencyKHz = -1)
            {
                return CAdfSystem(CModulator::NameCom1(), PhysicalQuantities::CFrequency(activeFrequencyKHz, PhysicalQuantities::CFrequencyUnit::MHz()), PhysicalQuantities::CFrequency(standbyFrequencyKHz < 0 ? activeFrequencyKHz : standbyFrequencyKHz, PhysicalQuantities::CFrequencyUnit::MHz()));
            }

            //! ADF1 unit
            static CAdfSystem GetAdf1System(PhysicalQuantities::CFrequency activeFrequency, PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CAdfSystem(CModulator::NameCom1(), activeFrequency, standbyFrequency ==  CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);
            }

            //! ADF2 unit
            static CAdfSystem GetAdf2System(double activeFrequencyKHz, double standbyFrequencyKHz = -1)
            {
                return CAdfSystem(CModulator::NameCom2(), PhysicalQuantities::CFrequency(activeFrequencyKHz, PhysicalQuantities::CFrequencyUnit::MHz()), PhysicalQuantities::CFrequency(standbyFrequencyKHz < 0 ? activeFrequencyKHz : standbyFrequencyKHz, PhysicalQuantities::CFrequencyUnit::MHz()));
            }

            //! ADF2 unit
            static CAdfSystem GetAdf2System(PhysicalQuantities::CFrequency activeFrequency, PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CAdfSystem(CModulator::NameCom2(), activeFrequency, standbyFrequency ==  CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);
            }

        protected:
            //! \copydoc CAvionicsBase::validValues
            virtual bool validValues() const override
            {
                if (this->isDefaultValue()) return true; // special case
                return
                    this->isValidFrequency(this->getFrequencyActive()) &&
                    this->isValidFrequency(this->getFrequencyStandby());
            }
        };

    }
}

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAdfSystem)

#endif // guard
