// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_AVIATION_NAVSYSTEM_H
#define BLACKMISC_AVIATION_NAVSYSTEM_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/modulator.h"

namespace BlackMisc::Aviation
{
    //! NAV system (radio navigation)
    class BLACKMISC_EXPORT CNavSystem :
        public CModulator<CNavSystem>,
        public Mixin::MetaType<CNavSystem>,
        public Mixin::JsonOperators<CNavSystem>,
        public Mixin::Index<CNavSystem>
    {
    public:
        //! Base type
        using base_type = CModulator<CNavSystem>;

        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CNavSystem)
        BLACKMISC_DECLARE_USING_MIXIN_INDEX(CNavSystem)

        //! Default constructor
        CNavSystem() = default;

        //! Constructor
        CNavSystem(const QString &name, const BlackMisc::PhysicalQuantities::CFrequency &activeFrequency, const BlackMisc::PhysicalQuantities::CFrequency &standbyFrequency) : CModulator(name, activeFrequency, standbyFrequency)
        {}

        //! Set active frequency
        void setFrequencyActiveMHz(double frequencyMHz)
        {
            const BlackMisc::PhysicalQuantities::CFrequency f(Math::CMathUtils::round(frequencyMHz, 3), BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz());
            this->setFrequencyActive(f);
        }

        //! Set standby frequency
        void setFrequencyStandbyMHz(double frequencyMHz)
        {
            const BlackMisc::PhysicalQuantities::CFrequency f(Math::CMathUtils::round(frequencyMHz, 3), BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz());
            this->setFrequencyStandby(f);
        }

        //! Valid civil aviation frequency?
        static bool isValidCivilNavigationFrequency(const BlackMisc::PhysicalQuantities::CFrequency &f)
        {
            const double fr = f.valueRounded(BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz(), 3);
            return fr >= 108.0 && fr <= 117.95;
        }

        //! Valid military aviation frequency?
        static bool isValidMilitaryNavigationFrequency(const BlackMisc::PhysicalQuantities::CFrequency &f)
        {
            const double fr = f.valueRounded(BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz(), 3);
            return fr >= 960.0 && fr <= 1215.0; // valid TACAN frequency
        }

        //! Valid aviation frequency (military/civil)
        static bool isValidNavigationFrequency(const BlackMisc::PhysicalQuantities::CFrequency &f)
        {
            return isValidCivilNavigationFrequency(f) || isValidMilitaryNavigationFrequency(f);
        }

        //! NAV1 unit
        static CNavSystem getNav1System(double activeFrequencyMHz, double standbyFrequencyMHz = -1)
        {
            return CNavSystem(CModulator::NameNav1(), BlackMisc::PhysicalQuantities::CFrequency(activeFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()), BlackMisc::PhysicalQuantities::CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()));
        }

        //! NAV1 unit
        static CNavSystem getNav1System(const BlackMisc::PhysicalQuantities::CFrequency &activeFrequency,
                                        const BlackMisc::PhysicalQuantities::CFrequency &standbyFrequency = { 0, BlackMisc::PhysicalQuantities::CFrequencyUnit::nullUnit() })
        {
            return CNavSystem(CModulator::NameNav1(), activeFrequency, standbyFrequency.isNull() ? activeFrequency : standbyFrequency);
        }

        //! NAV2 unit
        static CNavSystem getNav2System(double activeFrequencyMHz, double standbyFrequencyMHz = -1)
        {
            return CNavSystem(CModulator::NameNav2(), BlackMisc::PhysicalQuantities::CFrequency(activeFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()), BlackMisc::PhysicalQuantities::CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()));
        }

        //! NAV2 unit
        static CNavSystem getNav2System(const BlackMisc::PhysicalQuantities::CFrequency &activeFrequency,
                                        const BlackMisc::PhysicalQuantities::CFrequency &standbyFrequency = { 0, BlackMisc::PhysicalQuantities::CFrequencyUnit::nullUnit() })
        {
            return CNavSystem(CModulator::NameNav2(), activeFrequency, standbyFrequency.isNull() ? activeFrequency : standbyFrequency);
        }

    private:
        //! Easy access to derived class (CRTP template parameter)
        CNavSystem const *derived() const { return static_cast<CNavSystem const *>(this); }

        //! Easy access to derived class (CRTP template parameter)
        CNavSystem *derived() { return static_cast<CNavSystem *>(this); }
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CNavSystem)

#endif // guard
