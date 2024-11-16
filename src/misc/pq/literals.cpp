// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/pq/literals.h"

#include "misc/pq/acceleration.h"
#include "misc/pq/angle.h"
#include "misc/pq/frequency.h"
#include "misc/pq/length.h"
#include "misc/pq/mass.h"
#include "misc/pq/pressure.h"
#include "misc/pq/speed.h"
#include "misc/pq/temperature.h"
#include "misc/pq/time.h"
#include "misc/pq/units.h"

namespace swift::misc::physical_quantities::Literals
{
    CAcceleration operator""_ft_s2(long double value)
    {
        return { static_cast<double>(value), CAccelerationUnit::ft_s2() };
    }

    CAcceleration operator""_ft_s2(unsigned long long value)
    {
        return { static_cast<double>(value), CAccelerationUnit::ft_s2() };
    }

    CAcceleration operator""_m_s2(long double value)
    {
        return { static_cast<double>(value), CAccelerationUnit::m_s2() };
    }

    CAcceleration operator""_m_s2(unsigned long long value)
    {
        return { static_cast<double>(value), CAccelerationUnit::m_s2() };
    }

    CAngle operator""_deg(long double value)
    {
        return { static_cast<double>(value), CAngleUnit::deg() };
    }

    CAngle operator""_deg(unsigned long long value)
    {
        return { static_cast<double>(value), CAngleUnit::deg() };
    }

    CAngle operator""_rad(long double value)
    {
        return { static_cast<double>(value), CAngleUnit::rad() };
    }

    CAngle operator""_rad(unsigned long long value)
    {
        return { static_cast<double>(value), CAngleUnit::rad() };
    }

    CFrequency operator""_Hz(long double value)
    {
        return { static_cast<double>(value), CFrequencyUnit::Hz() };
    }

    CFrequency operator""_Hz(unsigned long long value)
    {
        return { static_cast<double>(value), CFrequencyUnit::Hz() };
    }

    CFrequency operator""_kHz(long double value)
    {
        return { static_cast<double>(value), CFrequencyUnit::kHz() };
    }

    CFrequency operator""_kHz(unsigned long long value)
    {
        return { static_cast<double>(value), CFrequencyUnit::kHz() };
    }

    CFrequency operator""_MHz(long double value)
    {
        return { static_cast<double>(value), CFrequencyUnit::MHz() };
    }

    CFrequency operator""_MHz(unsigned long long value)
    {
        return { static_cast<double>(value), CFrequencyUnit::MHz() };
    }

    CFrequency operator""_GHz(long double value)
    {
        return { static_cast<double>(value), CFrequencyUnit::GHz() };
    }

    CFrequency operator""_GHz(unsigned long long value)
    {
        return { static_cast<double>(value), CFrequencyUnit::GHz() };
    }

    CLength operator""_m(long double value)
    {
        return { static_cast<double>(value), CLengthUnit::m() };
    }

    CLength operator""_m(unsigned long long value)
    {
        return { static_cast<double>(value), CLengthUnit::m() };
    }

    CLength operator""_NM(long double value)
    {
        return { static_cast<double>(value), CLengthUnit::NM() };
    }

    CLength operator""_NM(unsigned long long value)
    {
        return { static_cast<double>(value), CLengthUnit::NM() };
    }

    CLength operator""_SM(long double value)
    {
        return { static_cast<double>(value), CLengthUnit::SM() };
    }

    CLength operator""_SM(unsigned long long value)
    {
        return { static_cast<double>(value), CLengthUnit::SM() };
    }

    CLength operator""_cm(long double value)
    {
        return { static_cast<double>(value), CLengthUnit::cm() };
    }

    CLength operator""_cm(unsigned long long value)
    {
        return { static_cast<double>(value), CLengthUnit::cm() };
    }

    CLength operator""_ft(long double value)
    {
        return { static_cast<double>(value), CLengthUnit::ft() };
    }

    CLength operator""_ft(unsigned long long value)
    {
        return { static_cast<double>(value), CLengthUnit::ft() };
    }

    CLength operator""_km(long double value)
    {
        return { static_cast<double>(value), CLengthUnit::km() };
    }

    CLength operator""_km(unsigned long long value)
    {
        return { static_cast<double>(value), CLengthUnit::km() };
    }

    CLength operator""_mi(long double value)
    {
        return { static_cast<double>(value), CLengthUnit::mi() };
    }

    CLength operator""_mi(unsigned long long value)
    {
        return { static_cast<double>(value), CLengthUnit::mi() };
    }

    CMass operator""_g(long double value)
    {
        return { static_cast<double>(value), CMassUnit::g() };
    }

    CMass operator""_g(unsigned long long value)
    {
        return { static_cast<double>(value), CMassUnit::g() };
    }

    CMass operator""_kg(long double value)
    {
        return { static_cast<double>(value), CMassUnit::kg() };
    }

    CMass operator""_kg(unsigned long long value)
    {
        return { static_cast<double>(value), CMassUnit::kg() };
    }

    CMass operator""_lb(long double value)
    {
        return { static_cast<double>(value), CMassUnit::lb() };
    }

    CMass operator""_lb(unsigned long long value)
    {
        return { static_cast<double>(value), CMassUnit::lb() };
    }

    CMass operator""_tonne(long double value)
    {
        return { static_cast<double>(value), CMassUnit::tonne() };
    }

    CMass operator""_tonne(unsigned long long value)
    {
        return { static_cast<double>(value), CMassUnit::tonne() };
    }

    CMass operator""_shortTon(long double value)
    {
        return { static_cast<double>(value), CMassUnit::shortTon() };
    }

    CMass operator""_shortTon(unsigned long long value)
    {
        return { static_cast<double>(value), CMassUnit::shortTon() };
    }

    CPressure operator""_Pa(long double value)
    {
        return { static_cast<double>(value), CPressureUnit::Pa() };
    }

    CPressure operator""_Pa(unsigned long long value)
    {
        return { static_cast<double>(value), CPressureUnit::Pa() };
    }

    CPressure operator""_bar(long double value)
    {
        return { static_cast<double>(value), CPressureUnit::bar() };
    }

    CPressure operator""_bar(unsigned long long value)
    {
        return { static_cast<double>(value), CPressureUnit::bar() };
    }

    CPressure operator""_hPa(long double value)
    {
        return { static_cast<double>(value), CPressureUnit::hPa() };
    }

    CPressure operator""_hPa(unsigned long long value)
    {
        return { static_cast<double>(value), CPressureUnit::hPa() };
    }

    CPressure operator""_psi(long double value)
    {
        return { static_cast<double>(value), CPressureUnit::psi() };
    }

    CPressure operator""_psi(unsigned long long value)
    {
        return { static_cast<double>(value), CPressureUnit::psi() };
    }

    CPressure operator""_inHg(long double value)
    {
        return { static_cast<double>(value), CPressureUnit::inHg() };
    }

    CPressure operator""_inHg(unsigned long long value)
    {
        return { static_cast<double>(value), CPressureUnit::inHg() };
    }

    CPressure operator""_mbar(long double value)
    {
        return { static_cast<double>(value), CPressureUnit::mbar() };
    }

    CPressure operator""_mbar(unsigned long long value)
    {
        return { static_cast<double>(value), CPressureUnit::mbar() };
    }

    CPressure operator""_mmHg(long double value)
    {
        return { static_cast<double>(value), CPressureUnit::mmHg() };
    }

    CPressure operator""_mmHg(unsigned long long value)
    {
        return { static_cast<double>(value), CPressureUnit::mmHg() };
    }

    CSpeed operator""_kts(long double value)
    {
        return { static_cast<double>(value), CSpeedUnit::kts() };
    }

    CSpeed operator""_kts(unsigned long long value)
    {
        return { static_cast<double>(value), CSpeedUnit::kts() };
    }

    CSpeed operator""_m_s(long double value)
    {
        return { static_cast<double>(value), CSpeedUnit::m_s() };
    }

    CSpeed operator""_m_s(unsigned long long value)
    {
        return { static_cast<double>(value), CSpeedUnit::m_s() };
    }

    CSpeed operator""_NM_h(long double value)
    {
        return { static_cast<double>(value), CSpeedUnit::NM_h() };
    }

    CSpeed operator""_NM_h(unsigned long long value)
    {
        return { static_cast<double>(value), CSpeedUnit::NM_h() };
    }

    CSpeed operator""_ft_s(long double value)
    {
        return { static_cast<double>(value), CSpeedUnit::ft_s() };
    }

    CSpeed operator""_ft_s(unsigned long long value)
    {
        return { static_cast<double>(value), CSpeedUnit::ft_s() };
    }

    CSpeed operator""_km_h(long double value)
    {
        return { static_cast<double>(value), CSpeedUnit::km_h() };
    }

    CSpeed operator""_km_h(unsigned long long value)
    {
        return { static_cast<double>(value), CSpeedUnit::km_h() };
    }

    CSpeed operator""_ft_min(long double value)
    {
        return { static_cast<double>(value), CSpeedUnit::ft_min() };
    }

    CSpeed operator""_ft_min(unsigned long long value)
    {
        return { static_cast<double>(value), CSpeedUnit::ft_min() };
    }

    CTemperature operator""_degC(long double value)
    {
        return { static_cast<double>(value), CTemperatureUnit::C() };
    }

    CTemperature operator""_degC(unsigned long long value)
    {
        return { static_cast<double>(value), CTemperatureUnit::C() };
    }

    CTemperature operator""_degF(long double value)
    {
        return { static_cast<double>(value), CTemperatureUnit::F() };
    }

    CTemperature operator""_degF(unsigned long long value)
    {
        return { static_cast<double>(value), CTemperatureUnit::F() };
    }

    CTemperature operator""_degK(long double value)
    {
        return { static_cast<double>(value), CTemperatureUnit::K() };
    }

    CTemperature operator""_degK(unsigned long long value)
    {
        return { static_cast<double>(value), CTemperatureUnit::K() };
    }

    CTime operator""_d(long double value)
    {
        return { static_cast<double>(value), CTimeUnit::d() };
    }

    CTime operator""_d(unsigned long long value)
    {
        return { static_cast<double>(value), CTimeUnit::d() };
    }

    CTime operator""_h(long double value)
    {
        return { static_cast<double>(value), CTimeUnit::h() };
    }

    CTime operator""_h(unsigned long long value)
    {
        return { static_cast<double>(value), CTimeUnit::h() };
    }

    CTime operator""_s(long double value)
    {
        return { static_cast<double>(value), CTimeUnit::s() };
    }

    CTime operator""_s(unsigned long long value)
    {
        return { static_cast<double>(value), CTimeUnit::s() };
    }

    CTime operator""_ms(long double value)
    {
        return { static_cast<double>(value), CTimeUnit::ms() };
    }

    CTime operator""_ms(unsigned long long value)
    {
        return { static_cast<double>(value), CTimeUnit::ms() };
    }

    CTime operator""_min(long double value)
    {
        return { static_cast<double>(value), CTimeUnit::min() };
    }

    CTime operator""_min(unsigned long long value)
    {
        return { static_cast<double>(value), CTimeUnit::min() };
    }

    CTime operator""_hms(long double value)
    {
        return { static_cast<double>(value), CTimeUnit::hms() };
    }

    CTime operator""_hms(unsigned long long value)
    {
        return { static_cast<double>(value), CTimeUnit::hms() };
    }

    CTime operator""_hrmin(long double value)
    {
        return { static_cast<double>(value), CTimeUnit::hrmin() };
    }

    CTime operator""_hrmin(unsigned long long value)
    {
        return { static_cast<double>(value), CTimeUnit::hrmin() };
    }

    CTime operator""_minsec(long double value)
    {
        return { static_cast<double>(value), CTimeUnit::minsec() };
    }

    CTime operator""_minsec(unsigned long long value)
    {
        return { static_cast<double>(value), CTimeUnit::minsec() };
    }
} // namespace swift::misc::physical_quantities::Literals
