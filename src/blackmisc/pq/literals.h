/* Copyright (C) 2020
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file
#ifndef BLACKMISC_PQ_LITERALS_H
#define BLACKMISC_PQ_LITERALS_H

#include "blackmisc/blackmiscexport.h"

namespace BlackMisc::PhysicalQuantities
{
    class CAcceleration;
    class CAngle;
    class CFrequency;
    class CLength;
    class CMass;
    class CPressure;
    class CSpeed;
    class CTemperature;
    class CTime;
    namespace Literals
    {
        //! @{
        //! User-defined literal for ft/s^2
        BLACKMISC_EXPORT CAcceleration operator""_ft_s2(long double);
        BLACKMISC_EXPORT CAcceleration operator""_ft_s2(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for m/s^2
        BLACKMISC_EXPORT CAcceleration operator""_m_s2(long double);
        BLACKMISC_EXPORT CAcceleration operator""_m_s2(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for degrees (Angle)
        BLACKMISC_EXPORT CAngle operator""_deg(long double);
        BLACKMISC_EXPORT CAngle operator""_deg(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for radian
        BLACKMISC_EXPORT CAngle operator""_rad(long double);
        BLACKMISC_EXPORT CAngle operator""_rad(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for hertz
        BLACKMISC_EXPORT CFrequency operator""_Hz(long double);
        BLACKMISC_EXPORT CFrequency operator""_Hz(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for kilohertz
        BLACKMISC_EXPORT CFrequency operator""_kHz(long double);
        BLACKMISC_EXPORT CFrequency operator""_kHz(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for megahertz
        BLACKMISC_EXPORT CFrequency operator""_MHz(long double);
        BLACKMISC_EXPORT CFrequency operator""_MHz(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for gigahertz
        BLACKMISC_EXPORT CFrequency operator""_GHz(long double);
        BLACKMISC_EXPORT CFrequency operator""_GHz(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for meters
        BLACKMISC_EXPORT CLength operator""_m(long double);
        BLACKMISC_EXPORT CLength operator""_m(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for nautical miles
        BLACKMISC_EXPORT CLength operator""_NM(long double);
        BLACKMISC_EXPORT CLength operator""_NM(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for statute miles
        BLACKMISC_EXPORT CLength operator""_SM(long double);
        BLACKMISC_EXPORT CLength operator""_SM(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for centimeters
        BLACKMISC_EXPORT CLength operator""_cm(long double);
        BLACKMISC_EXPORT CLength operator""_cm(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for feet
        BLACKMISC_EXPORT CLength operator""_ft(long double);
        BLACKMISC_EXPORT CLength operator""_ft(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for kilometer
        BLACKMISC_EXPORT CLength operator""_km(long double);
        BLACKMISC_EXPORT CLength operator""_km(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for miles
        BLACKMISC_EXPORT CLength operator""_mi(long double);
        BLACKMISC_EXPORT CLength operator""_mi(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for grams
        BLACKMISC_EXPORT CMass operator""_g(long double);
        BLACKMISC_EXPORT CMass operator""_g(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for kilograms
        BLACKMISC_EXPORT CMass operator""_kg(long double);
        BLACKMISC_EXPORT CMass operator""_kg(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for pounds
        BLACKMISC_EXPORT CMass operator""_lb(long double);
        BLACKMISC_EXPORT CMass operator""_lb(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for tonnes
        BLACKMISC_EXPORT CMass operator""_tonne(long double);
        BLACKMISC_EXPORT CMass operator""_tonne(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for US-ton
        BLACKMISC_EXPORT CMass operator""_shortTon(long double);
        BLACKMISC_EXPORT CMass operator""_shortTon(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for pascal
        BLACKMISC_EXPORT CPressure operator""_Pa(long double);
        BLACKMISC_EXPORT CPressure operator""_Pa(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for bar
        BLACKMISC_EXPORT CPressure operator""_bar(long double);
        BLACKMISC_EXPORT CPressure operator""_bar(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for hectopascal
        BLACKMISC_EXPORT CPressure operator""_hPa(long double);
        BLACKMISC_EXPORT CPressure operator""_hPa(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for pounds per square inch (psi)
        BLACKMISC_EXPORT CPressure operator""_psi(long double);
        BLACKMISC_EXPORT CPressure operator""_psi(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for inch of mercury (inHg)
        BLACKMISC_EXPORT CPressure operator""_inHg(long double);
        BLACKMISC_EXPORT CPressure operator""_inHg(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for milibar
        BLACKMISC_EXPORT CPressure operator""_mbar(long double);
        BLACKMISC_EXPORT CPressure operator""_mbar(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for milimetre of mercury
        BLACKMISC_EXPORT CPressure operator""_mmHg(long double);
        BLACKMISC_EXPORT CPressure operator""_mmHg(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for knots
        BLACKMISC_EXPORT CSpeed operator""_kts(long double);
        BLACKMISC_EXPORT CSpeed operator""_kts(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for m/s
        BLACKMISC_EXPORT CSpeed operator""_m_s(long double);
        BLACKMISC_EXPORT CSpeed operator""_m_s(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for NM/h
        BLACKMISC_EXPORT CSpeed operator""_NM_h(long double);
        BLACKMISC_EXPORT CSpeed operator""_NM_h(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for ft/s
        BLACKMISC_EXPORT CSpeed operator""_ft_s(long double);
        BLACKMISC_EXPORT CSpeed operator""_ft_s(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for km/h
        BLACKMISC_EXPORT CSpeed operator""_km_h(long double);
        BLACKMISC_EXPORT CSpeed operator""_km_h(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for ft/min
        BLACKMISC_EXPORT CSpeed operator""_ft_min(long double);
        BLACKMISC_EXPORT CSpeed operator""_ft_min(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for degrees Celsius
        BLACKMISC_EXPORT CTemperature operator""_degC(long double);
        BLACKMISC_EXPORT CTemperature operator""_degC(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for degrees Fahrenheit
        BLACKMISC_EXPORT CTemperature operator""_degF(long double);
        BLACKMISC_EXPORT CTemperature operator""_degF(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for degrees Kelvin
        BLACKMISC_EXPORT CTemperature operator""_degK(long double);
        BLACKMISC_EXPORT CTemperature operator""_degK(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for days
        BLACKMISC_EXPORT CTime operator""_d(long double);
        BLACKMISC_EXPORT CTime operator""_d(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for hours
        BLACKMISC_EXPORT CTime operator""_h(long double);
        BLACKMISC_EXPORT CTime operator""_h(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for seconds
        BLACKMISC_EXPORT CTime operator""_s(long double);
        BLACKMISC_EXPORT CTime operator""_s(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for miliseconds
        BLACKMISC_EXPORT CTime operator""_ms(long double);
        BLACKMISC_EXPORT CTime operator""_ms(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for minutes
        BLACKMISC_EXPORT CTime operator""_min(long double);
        BLACKMISC_EXPORT CTime operator""_min(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for hours/minutes/seconds
        BLACKMISC_EXPORT CTime operator""_hms(long double);
        BLACKMISC_EXPORT CTime operator""_hms(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for hours/minutes
        BLACKMISC_EXPORT CTime operator""_hrmin(long double);
        BLACKMISC_EXPORT CTime operator""_hrmin(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for minutes/seconds
        BLACKMISC_EXPORT CTime operator""_minsec(long double);
        BLACKMISC_EXPORT CTime operator""_minsec(unsigned long long);
        //! @}
    }

}

#endif // BLACKMISC_PQ_LITERALS_H
