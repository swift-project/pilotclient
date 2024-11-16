// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/comsystem.h"

#include <QDBusMetaType>
#include <QtGlobal>

#include "misc/math/mathutils.h"
#include "misc/pq/literals.h"
#include "misc/stringutils.h"

using namespace swift::misc::physical_quantities;
using namespace swift::misc::math;

namespace swift::misc::aviation
{
    void CComSystem::registerMetadata()
    {
        mixin::MetaType<CComSystem>::registerMetadata();
        qDBusRegisterMetaType<ChannelSpacing>();
        qDBusRegisterMetaType<ComUnit>();
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

    bool CComSystem::isActiveFrequencySameFrequency(const CFrequency &comFrequency) const
    {
        return isSameFrequency(this->getFrequencyActive(), comFrequency);
    }

    void CComSystem::setActiveUnicom()
    {
        this->toggleActiveStandby();
        this->setFrequencyActive(physical_quantities::CPhysicalQuantitiesConstants::FrequencyUnicom());
    }

    void CComSystem::setActiveInternationalAirDistress()
    {
        this->toggleActiveStandby();
        this->setFrequencyActive(
            physical_quantities::CPhysicalQuantitiesConstants::FrequencyInternationalAirDistress());
    }

    CComSystem CComSystem::getCom1System(double activeFrequencyMHz, double standbyFrequencyMHz)
    {
        return CComSystem(
            CModulator::NameCom1(),
            physical_quantities::CFrequency(activeFrequencyMHz, physical_quantities::CFrequencyUnit::MHz()),
            physical_quantities::CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz,
                                            physical_quantities::CFrequencyUnit::MHz()));
    }

    CComSystem CComSystem::getCom1System(const CFrequency &activeFrequency, const CFrequency &standbyFrequency)
    {
        return CComSystem(CModulator::NameCom1(), activeFrequency,
                          standbyFrequency.isNull() ? activeFrequency : standbyFrequency);
    }

    CComSystem CComSystem::getCom2System(double activeFrequencyMHz, double standbyFrequencyMHz)
    {
        return CComSystem(
            CModulator::NameCom2(),
            physical_quantities::CFrequency(activeFrequencyMHz, physical_quantities::CFrequencyUnit::MHz()),
            physical_quantities::CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz,
                                            physical_quantities::CFrequencyUnit::MHz()));
    }

    CComSystem CComSystem::getCom2System(const CFrequency &activeFrequency, const CFrequency &standbyFrequency)
    {
        return CComSystem(CModulator::NameCom2(), activeFrequency,
                          standbyFrequency.isNull() ? activeFrequency : standbyFrequency);
    }

    bool CComSystem::isValidCivilAviationFrequency(const CFrequency &f)
    {
        if (f.isNull()) return false;

        // comparsion in int avoids double compare issues
        const int fr = f.valueInteger(physical_quantities::CFrequencyUnit::kHz());
        return fr >= 118000 && fr <= 136990;
    }

    bool CComSystem::isValidMilitaryFrequency(const CFrequency &f)
    {
        if (f.isNull()) return false;
        const int fr = f.valueInteger(physical_quantities::CFrequencyUnit::kHz());
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

    bool CComSystem::isExclusiveWithin8_33kHzChannel(const physical_quantities::CFrequency &freq)
    {
        const int freqKHz = freq.value(CFrequencyUnit::kHz());
        if (freqKHz < 118000 || freqKHz >= 137000 || !isValid8_33kHzChannel(freqKHz)) { return false; }
        return !isWithin25kHzChannel(freq);
    }

    bool CComSystem::isWithin25kHzChannel(const physical_quantities::CFrequency &freq)
    {
        const int end = static_cast<int>(freq.value(CFrequencyUnit::kHz())) % 100;
        return end == 0 || end == 25 || end == 50 || end == 75;
    }

    bool CComSystem::isSameFrequency(const CFrequency &freq1, const CFrequency &freq2)
    {
        using namespace swift::misc::physical_quantities::Literals;
        if (freq1.isNull() || freq2.isNull()) { return false; }

        // Normalize .x20 => .x25 and .70 => .x75
        auto normalize = [](CFrequency freq) {
            const int freq_end = static_cast<int>(freq.value(CFrequencyUnit::kHz())) % 100;
            if (freq_end == 20 || freq_end == 70) { freq += 5_kHz; }
            return freq;
        };

        CFrequency normalized_freq1 = normalize(freq1);
        CFrequency normalized_freq2 = normalize(freq2);

        if (normalized_freq1 == normalized_freq2) { return true; } // shortcut for many of such comparisons

        // Avoid precision errors in Hz range
        return normalized_freq1.valueInteger(CFrequencyUnit::kHz()) ==
               normalized_freq2.valueInteger(CFrequencyUnit::kHz());
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
                if (ok) { comFreq = CFrequency(f, f > 999 ? CFrequencyUnit::kHz() : CFrequencyUnit::MHz()); }
                else { comFreq = CFrequency::null(); }
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
} // namespace swift::misc::aviation
