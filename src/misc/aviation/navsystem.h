// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_NAVSYSTEM_H
#define SWIFT_MISC_AVIATION_NAVSYSTEM_H

#include "misc/aviation/modulator.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::aviation
{
    //! NAV system (radio navigation)
    class SWIFT_MISC_EXPORT CNavSystem :
        public CModulator<CNavSystem>,
        public mixin::MetaType<CNavSystem>,
        public mixin::JsonOperators<CNavSystem>,
        public mixin::Index<CNavSystem>
    {
    public:
        //! Base type
        using base_type = CModulator<CNavSystem>;

        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CNavSystem)
        SWIFT_MISC_DECLARE_USING_MIXIN_INDEX(CNavSystem)

        //! Default constructor
        CNavSystem() = default;

        //! Constructor
        CNavSystem(const QString &name, const swift::misc::physical_quantities::CFrequency &activeFrequency,
                   const swift::misc::physical_quantities::CFrequency &standbyFrequency)
            : CModulator(name, activeFrequency, standbyFrequency)
        {}

        //! Set active frequency
        void setFrequencyActiveMHz(double frequencyMHz)
        {
            const swift::misc::physical_quantities::CFrequency f(
                math::CMathUtils::round(frequencyMHz, 3), swift::misc::physical_quantities::CFrequencyUnit::MHz());
            this->setFrequencyActive(f);
        }

        //! Set standby frequency
        void setFrequencyStandbyMHz(double frequencyMHz)
        {
            const swift::misc::physical_quantities::CFrequency f(
                math::CMathUtils::round(frequencyMHz, 3), swift::misc::physical_quantities::CFrequencyUnit::MHz());
            this->setFrequencyStandby(f);
        }

        //! Valid civil aviation frequency?
        static bool isValidCivilNavigationFrequency(const swift::misc::physical_quantities::CFrequency &f)
        {
            const double fr = f.valueRounded(swift::misc::physical_quantities::CFrequencyUnit::MHz(), 3);
            return fr >= 108.0 && fr <= 117.95;
        }

        //! Valid military aviation frequency?
        static bool isValidMilitaryNavigationFrequency(const swift::misc::physical_quantities::CFrequency &f)
        {
            const double fr = f.valueRounded(swift::misc::physical_quantities::CFrequencyUnit::MHz(), 3);
            return fr >= 960.0 && fr <= 1215.0; // valid TACAN frequency
        }

        //! Valid aviation frequency (military/civil)
        static bool isValidNavigationFrequency(const swift::misc::physical_quantities::CFrequency &f)
        {
            return isValidCivilNavigationFrequency(f) || isValidMilitaryNavigationFrequency(f);
        }

        //! NAV1 unit
        static CNavSystem getNav1System(double activeFrequencyMHz, double standbyFrequencyMHz = -1)
        {
            return CNavSystem(CModulator::NameNav1(),
                              swift::misc::physical_quantities::CFrequency(
                                  activeFrequencyMHz, swift::misc::physical_quantities::CFrequencyUnit::MHz()),
                              swift::misc::physical_quantities::CFrequency(
                                  standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz,
                                  swift::misc::physical_quantities::CFrequencyUnit::MHz()));
        }

        //! NAV1 unit
        static CNavSystem getNav1System(const swift::misc::physical_quantities::CFrequency &activeFrequency,
                                        const swift::misc::physical_quantities::CFrequency &standbyFrequency = {
                                            0, swift::misc::physical_quantities::CFrequencyUnit::nullUnit() })
        {
            return CNavSystem(CModulator::NameNav1(), activeFrequency,
                              standbyFrequency.isNull() ? activeFrequency : standbyFrequency);
        }

        //! NAV2 unit
        static CNavSystem getNav2System(double activeFrequencyMHz, double standbyFrequencyMHz = -1)
        {
            return CNavSystem(CModulator::NameNav2(),
                              swift::misc::physical_quantities::CFrequency(
                                  activeFrequencyMHz, swift::misc::physical_quantities::CFrequencyUnit::MHz()),
                              swift::misc::physical_quantities::CFrequency(
                                  standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz,
                                  swift::misc::physical_quantities::CFrequencyUnit::MHz()));
        }

        //! NAV2 unit
        static CNavSystem getNav2System(const swift::misc::physical_quantities::CFrequency &activeFrequency,
                                        const swift::misc::physical_quantities::CFrequency &standbyFrequency = {
                                            0, swift::misc::physical_quantities::CFrequencyUnit::nullUnit() })
        {
            return CNavSystem(CModulator::NameNav2(), activeFrequency,
                              standbyFrequency.isNull() ? activeFrequency : standbyFrequency);
        }

    private:
        //! Easy access to derived class (CRTP template parameter)
        CNavSystem const *derived() const { return static_cast<CNavSystem const *>(this); }

        //! Easy access to derived class (CRTP template parameter)
        CNavSystem *derived() { return static_cast<CNavSystem *>(this); }
    };
} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CNavSystem)

#endif
