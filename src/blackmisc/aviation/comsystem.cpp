/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/stringutils.h"

#include <QDBusMetaType>
#include <QtGlobal>

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Math;

namespace BlackMisc::Aviation
{
    void CComSystem::registerMetadata()
    {
        Mixin::MetaType<CComSystem>::registerMetadata();
        qDBusRegisterMetaType<ChannelSpacing>();
        qDBusRegisterMetaType<ComUnit>();
        qRegisterMetaTypeStreamOperators<ChannelSpacing>();
        qRegisterMetaTypeStreamOperators<ComUnit>();
    }

    void CComSystem::setFrequencyActiveMHz(double frequencyMHz)
    {
        const CFrequency f(frequencyMHz, CFrequencyUnit::MHz());
        this->setFrequencyActive(f);
    }

    void CComSystem::setFrequencyStandbyMHz(double frequencyMHz)
    {
        const CFrequency f(frequencyMHz, CFrequencyUnit::MHz());
        this->setFrequencyStandby(f);
    }

    void CComSystem::setFrequencyActive(const CFrequency &frequency)
    {
        if (frequency == this->getFrequencyActive()) { return; } // save all the comparisons / rounding
        CFrequency fRounded(frequency);
        roundToChannelSpacing(fRounded, m_channelSpacing);
        this->CModulator::setFrequencyActive(fRounded);
    }

    void CComSystem::setFrequencyStandby(const CFrequency &frequency)
    {
        if (frequency == this->getFrequencyStandby()) { return; } // save all the comparisons / rounding
        CFrequency fRounded(frequency);
        roundToChannelSpacing(fRounded, m_channelSpacing);
        this->CModulator::setFrequencyStandby(fRounded);
    }

    bool CComSystem::isActiveFrequencyWithin8_33kHzChannel(const CFrequency &comFrequency) const
    {
        return isWithinChannelSpacing(this->getFrequencyActive(), comFrequency, ChannelSpacing8_33KHz);
    }

    bool CComSystem::isActiveFrequencyWithin25kHzChannel(const CFrequency &comFrequency) const
    {
        return isWithinChannelSpacing(this->getFrequencyActive(), comFrequency, ChannelSpacing25KHz);
    }

    bool CComSystem::isActiveFrequencyWithin50kHzChannel(const CFrequency &comFrequency) const
    {
        return isWithinChannelSpacing(this->getFrequencyActive(), comFrequency, ChannelSpacing50KHz);
    }

    bool CComSystem::isActiveFrequencyWithinChannelSpacing(const CFrequency &comFrequency) const
    {
        return isWithinChannelSpacing(this->getFrequencyActive(), comFrequency, m_channelSpacing);
    }

    void CComSystem::setActiveUnicom()
    {
        this->toggleActiveStandby();
        this->setFrequencyActive(PhysicalQuantities::CPhysicalQuantitiesConstants::FrequencyUnicom());
    }

    void CComSystem::setActiveInternationalAirDistress()
    {
        this->toggleActiveStandby();
        this->setFrequencyActive(PhysicalQuantities::CPhysicalQuantitiesConstants::FrequencyInternationalAirDistress());
    }

    CComSystem CComSystem::getCom1System(double activeFrequencyMHz, double standbyFrequencyMHz)
    {
        return CComSystem(CModulator::NameCom1(), PhysicalQuantities::CFrequency(activeFrequencyMHz, PhysicalQuantities::CFrequencyUnit::MHz()), PhysicalQuantities::CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz, PhysicalQuantities::CFrequencyUnit::MHz()));
    }

    CComSystem CComSystem::getCom1System(const CFrequency &activeFrequency, const CFrequency &standbyFrequency)
    {
        return CComSystem(CModulator::NameCom1(), activeFrequency, standbyFrequency.isNull() ? activeFrequency : standbyFrequency);
    }

    CComSystem CComSystem::getCom2System(double activeFrequencyMHz, double standbyFrequencyMHz)
    {
        return CComSystem(CModulator::NameCom2(), PhysicalQuantities::CFrequency(activeFrequencyMHz, PhysicalQuantities::CFrequencyUnit::MHz()), PhysicalQuantities::CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz, PhysicalQuantities::CFrequencyUnit::MHz()));
    }

    CComSystem CComSystem::getCom2System(const CFrequency &activeFrequency, const CFrequency &standbyFrequency)
    {
        return CComSystem(CModulator::NameCom2(), activeFrequency, standbyFrequency.isNull() ? activeFrequency : standbyFrequency);
    }

    bool CComSystem::isValidCivilAviationFrequency(const CFrequency &f)
    {
        if (f.isNull()) return false;

        // comparsion in int avoids double compare issues
        const int fr = f.valueInteger(PhysicalQuantities::CFrequencyUnit::kHz());
        return fr >= 118000 && fr <= 136975;
    }

    bool CComSystem::isValidMilitaryFrequency(const CFrequency &f)
    {
        if (f.isNull()) return false;
        const int fr = f.valueInteger(PhysicalQuantities::CFrequencyUnit::kHz());
        return fr >= 220000 && fr <= 399950;
    }

    bool CComSystem::isValidComFrequency(const CFrequency &f)
    {
        if (f.isNull()) { return false; }
        return isValidCivilAviationFrequency(f) || isValidMilitaryFrequency(f);
    }

    bool CComSystem::isValid8_33kHzChannel(int fKHz)
    {
        const int lastDigits = static_cast<int>(fKHz) % 100;
        return fKHz % 5 == 0 && lastDigits != 20 && lastDigits != 45 && lastDigits != 70 && lastDigits != 95;
    }

    int CComSystem::round8_33kHzChannel(int fKHz)
    {
        if (!isValid8_33kHzChannel(fKHz))
        {
            const int diff = static_cast<int>(fKHz) % 5;
            int lower = fKHz - diff;
            if (!isValid8_33kHzChannel(lower)) { lower -= 5; }
            Q_ASSERT_X(isValid8_33kHzChannel(lower), Q_FUNC_INFO, "Lower frequency not valid");

            int upper = fKHz + (5 - diff);
            if (!isValid8_33kHzChannel(upper)) { upper += 5; }
            Q_ASSERT_X(isValid8_33kHzChannel(upper), Q_FUNC_INFO, "Upper frequency not valid");

            const int lowerDiff = abs(fKHz - lower);
            const int upperDiff = abs(fKHz - upper);

            fKHz = lowerDiff < upperDiff ? lower : upper;
            fKHz = std::clamp(fKHz, 118000, 136990);
        }
        return fKHz;
    }

    void CComSystem::roundToChannelSpacing(CFrequency &frequency, ChannelSpacing channelSpacing)
    {
        if (frequency.isNull()) { return; }
        const double channelSpacingKHz = CComSystem::channelSpacingToFrequencyKHz(channelSpacing);
        const double fKHz = frequency.valueRounded(CFrequencyUnit::kHz(), 0);

        if (channelSpacing == ChannelSpacing8_33KHz)
        {
            const int freqKHz = round8_33kHzChannel(fKHz);
            frequency.switchUnit(CFrequencyUnit::kHz());
            frequency.setCurrentUnitValue(freqKHz);
        }
        else
        {
            const int dDown = static_cast<int>(fKHz / channelSpacingKHz);
            const double fDownKHz = dDown * channelSpacingKHz;
            const double fUpKHz = (dDown + 1) * channelSpacingKHz;
            const bool down = qAbs(fKHz - fDownKHz) < qAbs(fUpKHz - fKHz); // which is the closest value
            const double fMHz(CMathUtils::round((down ? fDownKHz : fUpKHz) / 1000.0, 3));
            frequency.switchUnit(CFrequencyUnit::MHz());
            frequency.setCurrentUnitValue(fMHz);
        }
    }

    bool CComSystem::isExclusiveWithin8_33kHzChannel(const PhysicalQuantities::CFrequency &freq)
    {
        const int freqKHz = freq.value(CFrequencyUnit::kHz());
        if (freqKHz < 118000 || freqKHz >= 137000 || !isValid8_33kHzChannel(freqKHz)) { return false; }
        return !isWithin25kHzChannel(freq);
    }

    bool CComSystem::isWithin25kHzChannel(const PhysicalQuantities::CFrequency &freq)
    {
        const int end = static_cast<int>(freq.value(CFrequencyUnit::kHz())) % 100;
        return end == 0 || end == 25 || end == 50 || end == 75;
    }

    bool CComSystem::isWithinChannelSpacing(const CFrequency &setFrequency, const CFrequency &compareFrequency, CComSystem::ChannelSpacing channelSpacing)
    {
        if (setFrequency.isNull() || compareFrequency.isNull()) { return false; }
        if (setFrequency == compareFrequency) { return true; } // shortcut for many of such comparisons
        const double channelSpacingKHz = 0.5 * CComSystem::channelSpacingToFrequencyKHz(channelSpacing);
        const double compareFrequencyKHz = compareFrequency.value(CFrequencyUnit::kHz());
        const double setFrequencyKHz = setFrequency.value(CFrequencyUnit::kHz());
        return (setFrequencyKHz - channelSpacingKHz < compareFrequencyKHz) &&
                (setFrequencyKHz + channelSpacingKHz > compareFrequencyKHz);
    }

    CFrequency CComSystem::parseComFrequency(const QString &input, CPqString::SeparatorMode sep)
    {
        if (input.isEmpty()) { return CFrequency::null(); }
        CFrequency comFreq;
        if (isDigitsOnlyString(input))
        {
            const double f = input.toDouble();
            comFreq = CFrequency(f, f > 999 ? CFrequencyUnit::kHz() : CFrequencyUnit::MHz());
        }
        else
        {
            comFreq.parseFromString(input, sep);
            if (comFreq.isNull())
            {
                bool ok;
                const double f = CPqString::parseNumber(input, ok, sep);
                if (ok)
                {
                    comFreq = CFrequency(f, f > 999 ? CFrequencyUnit::kHz() : CFrequencyUnit::MHz());
                }
                else
                {
                    comFreq = CFrequency::null();
                }
            }
        }

        if (comFreq.isNull()) { return CFrequency::null(); }
        roundToChannelSpacing(comFreq, ChannelSpacing8_33KHz);
        return isValidComFrequency(comFreq) ? comFreq : CFrequency::null();
    }

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
