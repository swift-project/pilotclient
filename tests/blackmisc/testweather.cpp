/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include "testweather.h"
#include "blackmisc/weather/metardecoder.h"

using namespace BlackMisc::Weather;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMiscTest
{

    /*
     * Constructor
     */
    CTestWeather::CTestWeather(QObject *parent): QObject(parent)
    {
        // void
    }

    void CTestWeather::metarDecoder()
    {
        CMetarDecoder metarDecoder;
        CMetar metar = metarDecoder.decode("KLBB 241753Z 20009KT 10SM -SHRA FEW045 SCT220 SCT300 28/17 A3022");
        QVERIFY2(metar.getAirportIcaoCode() == CAirportIcaoCode("KLBB"), "Failed to parse airport code");
        QVERIFY2(metar.getDay() == 24, "Failed to parse day of report");
        QVERIFY2(metar.getTime() == CTime(17, 53, 0), "Failed to parse time of report");
        QVERIFY2(metar.getWindLayer().getDirection() == CAngle(200, CAngleUnit::deg()), "Failed to parse wind direction");
        QVERIFY2(metar.getWindLayer().getSpeed() == CSpeed(9, CSpeedUnit::kts()), "Failed to parse wind speed");
        QVERIFY2(metar.getWindLayer().getGustSpeed() == CSpeed(0, CSpeedUnit::kts()), "Wind gust speed should be null");
        QVERIFY2(metar.getVisibility() == CLength(10, CLengthUnit::SM()), "Failed to parse visibility");
        QVERIFY2(metar.getTemperature() == CTemperature(28, CTemperatureUnit::C()), "Failed to parse temperature");
        QVERIFY2(metar.getDewPoint() == CTemperature(17, CTemperatureUnit::C()), "Failed to parse dew point");
        QVERIFY2(metar.getAltimeter() == CPressure(30.22, CPressureUnit::inHg()), "Failed to parse altimeter");

        auto presentWeatherList = metar.getPresentWeather();
        QVERIFY2(presentWeatherList.size() == 1, "Present weather has an incorrect size");
        auto presentWeather = presentWeatherList.frontOrDefault();
        QVERIFY2(presentWeather.getIntensity() == CPresentWeather::Light, "Itensity should be light");
        QVERIFY2(presentWeather.getDescriptor() == CPresentWeather::Showers, "Descriptor should be showers");
        QVERIFY2(presentWeather.getWeatherPhenomena() & CPresentWeather::Rain, "Weather phenomina 'rain' should be set");
        QVERIFY2((presentWeather.getWeatherPhenomena() & CPresentWeather::Snow) == 0, "Weather phenomina 'Snow' should NOT be set");

        auto cloudLayers = metar.getCloudLayers();
        QVERIFY2(cloudLayers.containsCeiling(CAltitude(4500, CAltitude::AboveGround, CLengthUnit::ft())), "Cloud layer 4500 ft missing");
        QVERIFY2(cloudLayers.containsCeiling(CAltitude(22000, CAltitude::AboveGround, CLengthUnit::ft())), "Cloud layer 22000 ft missing");
        QVERIFY2(cloudLayers.containsCeiling(CAltitude(30000, CAltitude::AboveGround, CLengthUnit::ft())), "Cloud layer 30000 ft missing");
        QVERIFY2(cloudLayers.findByCeiling(CAltitude(4500, CAltitude::AboveGround, CLengthUnit::ft())).getCoverage() == CCloudLayer::Few, "Failed to parse cloud layer in 4500 ft");
        QVERIFY2(cloudLayers.findByCeiling(CAltitude(22000, CAltitude::AboveGround, CLengthUnit::ft())).getCoverage() == CCloudLayer::Scattered, "Failed to parse cloud layer in 22000 ft");
        QVERIFY2(cloudLayers.findByCeiling(CAltitude(30000, CAltitude::AboveGround, CLengthUnit::ft())).getCoverage() == CCloudLayer::Scattered, "Failed to parse cloud layer in 30000 ft");

        CMetar metar2 = metarDecoder.decode("EDDM 241753Z 20009G11KT 9000NDV FEW045 SCT220 SCT300 ///// Q1013");
        QVERIFY2(metar2.getAirportIcaoCode() == CAirportIcaoCode("EDDM"), "Failed to parse airport code");
        QVERIFY2(metar2.getDay() == 24, "Failed to parse day of report");
        QVERIFY2(metar2.getTime() == CTime(17, 53, 0), "Failed to parse time of report");
        QVERIFY2(metar2.getWindLayer().getDirection() == CAngle(200, CAngleUnit::deg()), "Failed to parse wind direction");
        QVERIFY2(metar2.getWindLayer().getSpeed() == CSpeed(9, CSpeedUnit::kts()), "Failed to parse wind speed");
        QVERIFY2(metar2.getWindLayer().getGustSpeed() == CSpeed(11, CSpeedUnit::kts()), "Wind gust speed should be null");
        QVERIFY2(metar2.getVisibility() == CLength(9000, CLengthUnit::m()), "Failed to parse visibility");
        QVERIFY2(metar2.getTemperature() == CTemperature(0, CTemperatureUnit::C()), "Failed to parse temperature");
        QVERIFY2(metar2.getDewPoint() == CTemperature(0, CTemperatureUnit::C()), "Failed to parse dew point");
        QVERIFY2(metar2.getAltimeter() == CPressure(1013, CPressureUnit::hPa()), "Failed to parse altimeter");

        auto presentWeatherList2 = metar2.getPresentWeather();
        QVERIFY2(presentWeatherList2.size() == 0, "Present weather has an incorrect size");

        auto cloudLayers2 = metar2.getCloudLayers();
        QVERIFY2(cloudLayers2.containsCeiling(CAltitude(4500, CAltitude::AboveGround, CLengthUnit::ft())), "Cloud layer 4500 ft missing");
        QVERIFY2(cloudLayers2.containsCeiling(CAltitude(22000, CAltitude::AboveGround, CLengthUnit::ft())), "Cloud layer 22000 ft missing");
        QVERIFY2(cloudLayers2.containsCeiling(CAltitude(30000, CAltitude::AboveGround, CLengthUnit::ft())), "Cloud layer 30000 ft missing");
        QVERIFY2(cloudLayers2.findByCeiling(CAltitude(4500, CAltitude::AboveGround, CLengthUnit::ft())).getCoverage() == CCloudLayer::Few, "Failed to parse cloud layer in 4500 ft");
        QVERIFY2(cloudLayers2.findByCeiling(CAltitude(22000, CAltitude::AboveGround, CLengthUnit::ft())).getCoverage() == CCloudLayer::Scattered, "Failed to parse cloud layer in 22000 ft");
        QVERIFY2(cloudLayers2.findByCeiling(CAltitude(30000, CAltitude::AboveGround, CLengthUnit::ft())).getCoverage() == CCloudLayer::Scattered, "Failed to parse cloud layer in 30000 ft");
    }

} // namespace

//! \endcond
