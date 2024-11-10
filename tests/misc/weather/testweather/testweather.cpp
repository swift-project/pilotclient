// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testmisc

#include "misc/aviation/airporticaocode.h"
#include "misc/aviation/altitude.h"
#include "misc/mixin/mixincompare.h"
#include "misc/pq/angle.h"
#include "misc/pq/length.h"
#include "misc/pq/physicalquantity.h"
#include "misc/pq/pressure.h"
#include "misc/pq/speed.h"
#include "misc/pq/temperature.h"
#include "misc/pq/time.h"
#include "misc/pq/units.h"
#include "misc/weather/cloudlayer.h"
#include "misc/weather/cloudlayerlist.h"
#include "misc/weather/metar.h"
#include "misc/weather/metardecoder.h"
#include "misc/weather/presentweather.h"
#include "misc/weather/presentweatherlist.h"
#include "misc/weather/windlayer.h"
#include "misc/weather/windlayerlist.h"
#include "test.h"

#include <QTest>

using namespace swift::misc::weather;
using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;

namespace MiscTest
{
    //! Weather unit tests
    class CTestWeather : public QObject
    {
        Q_OBJECT

    private slots:
        //! Testing CCloudLayer/CCloudLayerList
        void cloudLayer();

        //! Testing CWindLayer/CWindLayerList
        void windLayer();

        //! Testing METAR decoder
        void metarDecoder();
    };

    void CTestWeather::cloudLayer()
    {
        const CAltitude base1 { 0, CAltitude::AboveGround, CLengthUnit::ft() };
        const CAltitude top1 { 5000, CAltitude::AboveGround, CLengthUnit::ft() };

        CCloudLayer cl1(base1, top1, CCloudLayer::Scattered);
        QVERIFY(cl1.getBase() == base1);
        QVERIFY(cl1.getTop() == top1);
        QCOMPARE(cl1.getPrecipitationRate(), 0.0);
        QVERIFY(cl1.getPrecipitation() == CCloudLayer::NoPrecipitation);
        QVERIFY(cl1.getClouds() == CCloudLayer::NoClouds);
        QVERIFY(cl1.getCoverage() == CCloudLayer::Scattered);

        const CAltitude base2 { 15000, CAltitude::AboveGround, CLengthUnit::ft() };
        const CAltitude top2 { 35000, CAltitude::AboveGround, CLengthUnit::ft() };

        CCloudLayer cl2 { base2, top2, 5, CCloudLayer::Rain, CCloudLayer::Cirrus, CCloudLayer::Scattered };
        QVERIFY(cl2.getBase() == base2);
        QVERIFY(cl2.getTop() == top2);
        QCOMPARE(cl2.getPrecipitationRate(), 5.0);
        QVERIFY(cl2.getPrecipitation() == CCloudLayer::Rain);
        QVERIFY(cl2.getClouds() == CCloudLayer::Cirrus);
        QVERIFY(cl2.getCoverage() == CCloudLayer::Scattered);

        const CAltitude base3 { 25000, CAltitude::AboveGround, CLengthUnit::ft() };
        CCloudLayerList cll { cl1, cl2 };
        QVERIFY(cll.containsBase(base1));
        QVERIFY(cll.containsBase(base2));
        QVERIFY(!cll.containsBase(base3));
        QVERIFY(cll.findByBase(base1) != CCloudLayer());
        QVERIFY(cll.findByBase(base2) != CCloudLayer());
        QVERIFY(cll.findByBase(base3) == CCloudLayer());
    }

    void CTestWeather::windLayer()
    {
        const CAltitude level1 { 0, CAltitude::AboveGround, CLengthUnit::ft() };
        const CAngle direction1 { 25, CAngleUnit::deg() };
        const CSpeed speed1 { 10, CSpeedUnit::kts() };
        const CSpeed gustSpeed1 { 20, CSpeedUnit::kts() };

        CWindLayer wl1(level1, direction1, speed1, gustSpeed1);
        QVERIFY(wl1.getLevel() == level1);
        QVERIFY(wl1.getDirection() == direction1);
        QVERIFY(wl1.getSpeed() == speed1);
        QVERIFY(wl1.getGustSpeed() == gustSpeed1);

        CAltitude level2 { 25000, CAltitude::AboveGround, CLengthUnit::ft() };
        CWindLayerList wll { wl1 };
        QVERIFY(wll.containsLevel(level1));
        QVERIFY(!wll.containsLevel(level2));
        QVERIFY(wll.findByLevel(level1) != CWindLayer());
        QVERIFY(wll.findByLevel(level2) == CWindLayer());
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
        QVERIFY2(cloudLayers.containsBase(CAltitude(4500, CAltitude::AboveGround, CLengthUnit::ft())), "Cloud layer 4500 ft missing");
        QVERIFY2(cloudLayers.containsBase(CAltitude(22000, CAltitude::AboveGround, CLengthUnit::ft())), "Cloud layer 22000 ft missing");
        QVERIFY2(cloudLayers.containsBase(CAltitude(30000, CAltitude::AboveGround, CLengthUnit::ft())), "Cloud layer 30000 ft missing");
        QVERIFY2(cloudLayers.findByBase(CAltitude(4500, CAltitude::AboveGround, CLengthUnit::ft())).getCoverage() == CCloudLayer::Few, "Failed to parse cloud layer in 4500 ft");
        QVERIFY2(cloudLayers.findByBase(CAltitude(22000, CAltitude::AboveGround, CLengthUnit::ft())).getCoverage() == CCloudLayer::Scattered, "Failed to parse cloud layer in 22000 ft");
        QVERIFY2(cloudLayers.findByBase(CAltitude(30000, CAltitude::AboveGround, CLengthUnit::ft())).getCoverage() == CCloudLayer::Scattered, "Failed to parse cloud layer in 30000 ft");

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
        QVERIFY2(cloudLayers2.containsBase(CAltitude(4500, CAltitude::AboveGround, CLengthUnit::ft())), "Cloud layer 4500 ft missing");
        QVERIFY2(cloudLayers2.containsBase(CAltitude(22000, CAltitude::AboveGround, CLengthUnit::ft())), "Cloud layer 22000 ft missing");
        QVERIFY2(cloudLayers2.containsBase(CAltitude(30000, CAltitude::AboveGround, CLengthUnit::ft())), "Cloud layer 30000 ft missing");
        QVERIFY2(cloudLayers2.findByBase(CAltitude(4500, CAltitude::AboveGround, CLengthUnit::ft())).getCoverage() == CCloudLayer::Few, "Failed to parse cloud layer in 4500 ft");
        QVERIFY2(cloudLayers2.findByBase(CAltitude(22000, CAltitude::AboveGround, CLengthUnit::ft())).getCoverage() == CCloudLayer::Scattered, "Failed to parse cloud layer in 22000 ft");
        QVERIFY2(cloudLayers2.findByBase(CAltitude(30000, CAltitude::AboveGround, CLengthUnit::ft())).getCoverage() == CCloudLayer::Scattered, "Failed to parse cloud layer in 30000 ft");
    }

} // namespace

//! main
BLACKTEST_APPLESS_MAIN(MiscTest::CTestWeather);

#include "testweather.moc"

//! \endcond
