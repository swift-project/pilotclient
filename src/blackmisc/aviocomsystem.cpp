/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/aviocomsystem.h"

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Aviation
    {
        /*
         * Valid values?
         */
        bool CComSystem::validValues() const
        {
            if (this->isDefaultValue()) return true; // special case
            return
                (CComSystem::isValidCivilAviationFrequency(this->getFrequencyActive()) ||
                 CComSystem::isValidMilitaryFrequency(this->getFrequencyActive())) &&
                (CComSystem::isValidCivilAviationFrequency(this->getFrequencyStandby()) ||
                 CComSystem::isValidMilitaryFrequency(this->getFrequencyStandby()));
        }

        /*
         * Validate
         */
        bool CComSystem::validate(bool strict) const
        {
            if (this->isDefaultValue()) return true;
            bool valid = this->validValues();
            if (!strict) return valid;
            Q_ASSERT_X(valid, "CComSystem::validate", "illegal values");
            if (!valid) throw std::range_error("Illegal values in CComSystem::validate");
            return true;
        }

        void CComSystem::setFrequencyActiveMHz(double frequencyMHz)
        {
            CFrequency f(frequencyMHz, CFrequencyUnit::MHz());
            if (f == this->getFrequencyActive()) return; // save all the comparisons / rounding
            CComSystem::roundTo25KHz(f);
            this->CModulator::setFrequencyActive(f);
            this->validate(true);
        }

        void CComSystem::setFrequencyStandbyMHz(double frequencyMHz)
        {
            CFrequency f(frequencyMHz, CFrequencyUnit::MHz());
            if (f == this->getFrequencyStandby()) return; // save all the comparisons / rounding
            CComSystem::roundTo25KHz(f);
            this->CModulator::setFrequencyStandby(f);
            this->validate(true);
        }

        /*
         * Round to 25KHz
         */
        void CComSystem::roundTo25KHz(PhysicalQuantities::CFrequency &frequency)
        {
            double f = frequency.valueRounded(CFrequencyUnit::kHz(), 0);
            quint32 d = static_cast<quint32>(f / 25.0);
            frequency.switchUnit(CFrequencyUnit::MHz());
            double f0 = frequency.valueRounded(CFrequencyUnit::MHz(), 3);
            double f1 = CMath::round(d * (25.0 / 1000.0), 3);
            double f2 = CMath::round((d + 1) * (25.0 / 1000.0), 3);
            bool down = qAbs(f1 - f0) < qAbs(f2 - f0); // which is the closest value
            frequency.setCurrentUnitValue(down ? f1 : f2);
        }
    } // namespace
}
