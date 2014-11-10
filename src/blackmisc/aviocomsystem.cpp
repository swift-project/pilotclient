/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/aviocomsystem.h"

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Math;

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
         * COM frequency
         */
        void CComSystem::setFrequencyActiveMHz(double frequencyMHz)
        {
            CFrequency f(frequencyMHz, CFrequencyUnit::MHz());
            this->setFrequencyActive(f);
        }

        /*
         * COM frequency
         */
        void CComSystem::setFrequencyStandbyMHz(double frequencyMHz)
        {
            CFrequency f(frequencyMHz, CFrequencyUnit::MHz());
            this->setFrequencyStandby(f);
        }

        /*
         * COM frequency
         */
        void CComSystem::setFrequencyActive(const CFrequency &frequency)
        {
            if (frequency == this->getFrequencyActive()) { return; } // save all the comparisons / rounding
            CFrequency fRounded(frequency);
            roundToChannelSpacing(fRounded, this->m_channelSpacing);
            this->CModulator::setFrequencyActive(fRounded);
        }

        /*
         * COM frequency
         */
        void CComSystem::setFrequencyStandby(const CFrequency &frequency)
        {
            if (frequency == this->getFrequencyStandby()) { return; } // save all the comparisons / rounding
            CFrequency fRounded(frequency);
            roundToChannelSpacing(fRounded, this->m_channelSpacing);
            this->CModulator::setFrequencyStandby(fRounded);
        }

        /*
         * Round to channel spacing
         */
        void CComSystem::roundToChannelSpacing(PhysicalQuantities::CFrequency &frequency, ChannelSpacing channelSpacing)
        {
            double channelSpacingKHz = CComSystem::channelSpacingToFrequencyKHz(channelSpacing);
            double f = frequency.valueRounded(CFrequencyUnit::kHz(), 0);
            quint32 d = static_cast<quint32>(f / channelSpacingKHz);
            frequency.switchUnit(CFrequencyUnit::MHz());
            double f0 = frequency.valueRounded(CFrequencyUnit::MHz(), 3);
            double f1 = CMath::round(d * (channelSpacingKHz / 1000.0), 3);
            double f2 = CMath::round((d + 1) * (channelSpacingKHz / 1000.0), 3);
            bool down = qAbs(f1 - f0) < qAbs(f2 - f0); // which is the closest value
            frequency.setCurrentUnitValue(down ? f1 : f2);
        }

        /*
         * Within channel spacing
         */
        bool CComSystem::isWithinChannelSpacing(const CFrequency &setFrequency, const CFrequency &compareFrequency, CComSystem::ChannelSpacing channelSpacing)
        {
            if (setFrequency == compareFrequency) return true; // shortcut for many of such comparisons
            double channelSpacingKHz = 0.5 * CComSystem::channelSpacingToFrequencyKHz(channelSpacing);
            double compareFrequencyKHz = compareFrequency.value(CFrequencyUnit::kHz());
            double setFrequencyKHz = setFrequency.value(CFrequencyUnit::kHz());
            return (setFrequencyKHz - channelSpacingKHz < compareFrequencyKHz) &&
                   (setFrequencyKHz + channelSpacingKHz > compareFrequencyKHz);
        }

        /*
         * Helper, give me number for channels spacing
         */
        double CComSystem::channelSpacingToFrequencyKHz(ChannelSpacing channelSpacing)
        {
            switch (channelSpacing)
            {
            case ChannelSpacing50KHz: return 50.0;
            case ChannelSpacing25KHz: return 25.0;
            case ChannelSpacing8_33KHz: return 25.0 / 3.0;
            default: qFatal("Wrong channel spacing"); return 0.0; // return just supressing compiler warning
            }
        }

    } // namespace
} // namespace
