/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_ADFSYSTEM_H
#define BLACKMISC_AVIATION_ADFSYSTEM_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/modulator.h"

#include <stdexcept>

namespace BlackMisc::Aviation
{
    //! ADF system ("for NDBs")
    class BLACKMISC_EXPORT CAdfSystem :
        public CModulator<CAdfSystem>,
        public Mixin::MetaType<CAdfSystem>,
        public Mixin::JsonOperators<CAdfSystem>,
        public Mixin::Index<CAdfSystem>
    {
    public:
        //! Base type
        using base_type = CModulator<CAdfSystem>;

        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAdfSystem)
        BLACKMISC_DECLARE_USING_MIXIN_INDEX(CAdfSystem)

        //! Default constructor
        CAdfSystem() = default;

        //! Constructor
        CAdfSystem(const QString &name, const PhysicalQuantities::CFrequency &activeFrequency,
                    const PhysicalQuantities::CFrequency &standbyFrequency = { 0, BlackMisc::PhysicalQuantities::CFrequencyUnit::nullUnit() }):
            CModulator(name, activeFrequency, standbyFrequency.isNull() ? activeFrequency : standbyFrequency)
        { }

        //! Valid aviation frequency?
        static bool isValidFrequency(const PhysicalQuantities::CFrequency &f)
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
        static CAdfSystem GetAdf1System(const PhysicalQuantities::CFrequency &activeFrequency,
                                        const PhysicalQuantities::CFrequency &standbyFrequency = { 0, BlackMisc::PhysicalQuantities::CFrequencyUnit::nullUnit() })
        {
            return CAdfSystem(CModulator::NameCom1(), activeFrequency, standbyFrequency.isNull() ? activeFrequency : standbyFrequency);
        }

        //! ADF2 unit
        static CAdfSystem GetAdf2System(double activeFrequencyKHz, double standbyFrequencyKHz = -1)
        {
            return CAdfSystem(CModulator::NameCom2(), PhysicalQuantities::CFrequency(activeFrequencyKHz, PhysicalQuantities::CFrequencyUnit::MHz()), PhysicalQuantities::CFrequency(standbyFrequencyKHz < 0 ? activeFrequencyKHz : standbyFrequencyKHz, PhysicalQuantities::CFrequencyUnit::MHz()));
        }

        //! ADF2 unit
        static CAdfSystem GetAdf2System(const PhysicalQuantities::CFrequency &activeFrequency,
                                        const PhysicalQuantities::CFrequency &standbyFrequency = { 0, BlackMisc::PhysicalQuantities::CFrequencyUnit::nullUnit() })
        {
            return CAdfSystem(CModulator::NameCom2(), activeFrequency, standbyFrequency.isNull() ? activeFrequency : standbyFrequency);
        }
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAdfSystem)

#endif // guard
