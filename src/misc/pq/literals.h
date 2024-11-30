// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PQ_LITERALS_H
#define SWIFT_MISC_PQ_LITERALS_H

#include "misc/swiftmiscexport.h"

namespace swift::misc::physical_quantities
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
        SWIFT_MISC_EXPORT CAcceleration operator""_ft_s2(long double);
        SWIFT_MISC_EXPORT CAcceleration operator""_ft_s2(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for m/s^2
        SWIFT_MISC_EXPORT CAcceleration operator""_m_s2(long double);
        SWIFT_MISC_EXPORT CAcceleration operator""_m_s2(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for degrees (Angle)
        SWIFT_MISC_EXPORT CAngle operator""_deg(long double);
        SWIFT_MISC_EXPORT CAngle operator""_deg(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for radian
        SWIFT_MISC_EXPORT CAngle operator""_rad(long double);
        SWIFT_MISC_EXPORT CAngle operator""_rad(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for hertz
        SWIFT_MISC_EXPORT CFrequency operator""_Hz(long double);
        SWIFT_MISC_EXPORT CFrequency operator""_Hz(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for kilohertz
        SWIFT_MISC_EXPORT CFrequency operator""_kHz(long double);
        SWIFT_MISC_EXPORT CFrequency operator""_kHz(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for megahertz
        SWIFT_MISC_EXPORT CFrequency operator""_MHz(long double);
        SWIFT_MISC_EXPORT CFrequency operator""_MHz(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for gigahertz
        SWIFT_MISC_EXPORT CFrequency operator""_GHz(long double);
        SWIFT_MISC_EXPORT CFrequency operator""_GHz(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for meters
        SWIFT_MISC_EXPORT CLength operator""_m(long double);
        SWIFT_MISC_EXPORT CLength operator""_m(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for nautical miles
        SWIFT_MISC_EXPORT CLength operator""_NM(long double);
        SWIFT_MISC_EXPORT CLength operator""_NM(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for statute miles
        SWIFT_MISC_EXPORT CLength operator""_SM(long double);
        SWIFT_MISC_EXPORT CLength operator""_SM(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for centimeters
        SWIFT_MISC_EXPORT CLength operator""_cm(long double);
        SWIFT_MISC_EXPORT CLength operator""_cm(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for feet
        SWIFT_MISC_EXPORT CLength operator""_ft(long double);
        SWIFT_MISC_EXPORT CLength operator""_ft(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for kilometer
        SWIFT_MISC_EXPORT CLength operator""_km(long double);
        SWIFT_MISC_EXPORT CLength operator""_km(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for miles
        SWIFT_MISC_EXPORT CLength operator""_mi(long double);
        SWIFT_MISC_EXPORT CLength operator""_mi(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for grams
        SWIFT_MISC_EXPORT CMass operator""_g(long double);
        SWIFT_MISC_EXPORT CMass operator""_g(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for kilograms
        SWIFT_MISC_EXPORT CMass operator""_kg(long double);
        SWIFT_MISC_EXPORT CMass operator""_kg(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for pounds
        SWIFT_MISC_EXPORT CMass operator""_lb(long double);
        SWIFT_MISC_EXPORT CMass operator""_lb(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for tonnes
        SWIFT_MISC_EXPORT CMass operator""_tonne(long double);
        SWIFT_MISC_EXPORT CMass operator""_tonne(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for US-ton
        SWIFT_MISC_EXPORT CMass operator""_shortTon(long double);
        SWIFT_MISC_EXPORT CMass operator""_shortTon(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for pascal
        SWIFT_MISC_EXPORT CPressure operator""_Pa(long double);
        SWIFT_MISC_EXPORT CPressure operator""_Pa(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for bar
        SWIFT_MISC_EXPORT CPressure operator""_bar(long double);
        SWIFT_MISC_EXPORT CPressure operator""_bar(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for hectopascal
        SWIFT_MISC_EXPORT CPressure operator""_hPa(long double);
        SWIFT_MISC_EXPORT CPressure operator""_hPa(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for pounds per square inch (psi)
        SWIFT_MISC_EXPORT CPressure operator""_psi(long double);
        SWIFT_MISC_EXPORT CPressure operator""_psi(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for inch of mercury (inHg)
        SWIFT_MISC_EXPORT CPressure operator""_inHg(long double);
        SWIFT_MISC_EXPORT CPressure operator""_inHg(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for milibar
        SWIFT_MISC_EXPORT CPressure operator""_mbar(long double);
        SWIFT_MISC_EXPORT CPressure operator""_mbar(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for milimetre of mercury
        SWIFT_MISC_EXPORT CPressure operator""_mmHg(long double);
        SWIFT_MISC_EXPORT CPressure operator""_mmHg(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for knots
        SWIFT_MISC_EXPORT CSpeed operator""_kts(long double);
        SWIFT_MISC_EXPORT CSpeed operator""_kts(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for m/s
        SWIFT_MISC_EXPORT CSpeed operator""_m_s(long double);
        SWIFT_MISC_EXPORT CSpeed operator""_m_s(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for NM/h
        SWIFT_MISC_EXPORT CSpeed operator""_NM_h(long double);
        SWIFT_MISC_EXPORT CSpeed operator""_NM_h(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for ft/s
        SWIFT_MISC_EXPORT CSpeed operator""_ft_s(long double);
        SWIFT_MISC_EXPORT CSpeed operator""_ft_s(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for km/h
        SWIFT_MISC_EXPORT CSpeed operator""_km_h(long double);
        SWIFT_MISC_EXPORT CSpeed operator""_km_h(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for ft/min
        SWIFT_MISC_EXPORT CSpeed operator""_ft_min(long double);
        SWIFT_MISC_EXPORT CSpeed operator""_ft_min(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for degrees Celsius
        SWIFT_MISC_EXPORT CTemperature operator""_degC(long double);
        SWIFT_MISC_EXPORT CTemperature operator""_degC(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for degrees Fahrenheit
        SWIFT_MISC_EXPORT CTemperature operator""_degF(long double);
        SWIFT_MISC_EXPORT CTemperature operator""_degF(unsigned long long);
        //! @}

        //! @{
        //! User-defined literal for degrees Kelvin
        SWIFT_MISC_EXPORT CTemperature operator""_degK(long double);
        SWIFT_MISC_EXPORT CTemperature operator""_degK(unsigned long long);
        //! @}
    } // namespace Literals

} // namespace swift::misc::physical_quantities

#endif
