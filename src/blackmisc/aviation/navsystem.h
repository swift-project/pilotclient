/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_NAVSYSTEM_H
#define BLACKMISC_AVIATION_NAVSYSTEM_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/modulator.h"

namespace BlackMisc
{
    namespace Aviation
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
            CNavSystem(const QString &name, const BlackMisc::PhysicalQuantities::CFrequency &activeFrequency, const BlackMisc::PhysicalQuantities::CFrequency &standbyFrequency):
                CModulator(name, activeFrequency, standbyFrequency)
            { }

            //! Set active frequency
            void setFrequencyActiveMHz(double frequencyMHz) override
            {
                this->CModulator::setFrequencyActiveMHz(frequencyMHz);
            }

            //! Set standby frequency
            void setFrequencyStandbyMHz(double frequencyMHz) override
            {
                this->CModulator::setFrequencyStandbyMHz(frequencyMHz);
            }

            //! Valid civil aviation frequency?
            static bool isValidCivilNavigationFrequency(BlackMisc::PhysicalQuantities::CFrequency f)
            {
                double fr = f.valueRounded(BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz(), 3);
                return fr >= 108.0 && fr <= 117.95;
            }

            //! Valid military aviation frequency?
            static bool isValidMilitaryNavigationFrequency(BlackMisc::PhysicalQuantities::CFrequency f)
            {
                double fr = f.valueRounded(BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz(), 3);
                return fr >= 960.0 && fr <= 1215.0; // valid TACAN frequency
            }

            //! NAV1 unit
            static CNavSystem getNav1System(double activeFrequencyMHz, double standbyFrequencyMHz = -1)
            {
                return CNavSystem(CModulator::NameNav1(), BlackMisc::PhysicalQuantities::CFrequency(activeFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()), BlackMisc::PhysicalQuantities::CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()));
            }

            //! NAV1 unit
            static CNavSystem getNav1System(BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CNavSystem(CModulator::NameNav1(), activeFrequency, standbyFrequency ==  CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);
            }

            //! NAV2 unit
            static CNavSystem getNav2System(double activeFrequencyMHz, double standbyFrequencyMHz = -1)
            {
                return CNavSystem(CModulator::NameNav2(), BlackMisc::PhysicalQuantities::CFrequency(activeFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()), BlackMisc::PhysicalQuantities::CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()));
            }

            //! NAV2 unit
            static CNavSystem getNav2System(BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CNavSystem(CModulator::NameNav2(), activeFrequency, standbyFrequency ==  CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);
            }

        protected:
            //! \copydoc CModulator::validValues
            virtual bool validValues() const override
            {
                if (this->isDefaultValue()) return true; // special case
                bool v =
                    (this->isValidCivilNavigationFrequency(this->getFrequencyActive()) ||
                     this->isValidMilitaryNavigationFrequency(this->getFrequencyActive())) &&
                    (this->isValidCivilNavigationFrequency(this->getFrequencyStandby()) ||
                     this->isValidMilitaryNavigationFrequency(this->getFrequencyStandby()));
                return v;
            }

        private:
            //! Easy access to derived class (CRTP template parameter)
            CNavSystem const *derived() const { return static_cast<CNavSystem const *>(this); }

            //! Easy access to derived class (CRTP template parameter)
            CNavSystem *derived() { return static_cast<CNavSystem *>(this); }
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CNavSystem)

#endif // guard
