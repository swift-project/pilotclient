/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "metardecoder.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/weather/presentweather.h"
#include <QRegularExpression>
#include <QDebug>

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Weather
    {

        // Implementation based on the following websites:
        // http://meteocentre.com/doc/metar.html
        // http://www.sigmet.de/key.php
        // http://wx.erau.edu/reference/text/metar_code_format.pdf

        class IMetarDecoderPart
        {
        protected:
            virtual bool isRepeatable() const { return false; }
            virtual QString getRegExp() const = 0;
            virtual bool validateAndSet(const QRegularExpressionMatch &match, CMetar &metar) const = 0;
            virtual bool isMandatory() const = 0;
            virtual QString getDecoderType() const = 0;

        public:
            bool parse(QString &metarString, CMetar &metar)
            {
                bool isValid = false;
                QRegularExpression re(getRegExp());
                Q_ASSERT(re.isValid());
                // Loop stop condition:
                // - Invalid data
                // - One match found and token not repeatable
                do
                {
                    QRegularExpressionMatch match = re.match(metarString);
                    if (match.hasMatch())
                    {
                        // If invalid data, we return straight away
                        if (!validateAndSet(match, metar)) return false;
                        // Remove the captured part
                        metarString.replace(re, QString());
                        isValid = true;
                    }
                    else
                    {
                        // No (more) match found.
                        if (!isMandatory()) { isValid = true; }
                        break;
                    }
                }
                while (isRepeatable());

                if (!isValid) { CLogMessage(this).debug() << "Failed to match" << getDecoderType() << "in remaining metar:" << metarString; }
                return isValid;
            }
        };

        class CMetarDecoderReportType : public IMetarDecoderPart
        {
        protected:
            QString getRegExp() const override { return QStringLiteral("^(?<reporttype>METAR|SPECI)? "); }

            bool validateAndSet(const QRegularExpressionMatch &match, CMetar &metar) const override
            {
                QString reportTypeAsString = match.captured("reporttype");
                if (reportTypeAsString.isEmpty() || !getReportTypeHash().contains(reportTypeAsString)) { return false; }

                metar.setReportType(getReportTypeHash().value(reportTypeAsString));
                return true;
            }

            virtual bool isMandatory() const override { return false; }
            virtual QString getDecoderType() const override { return "ReportType"; }

        private:
            const QHash<QString, CMetar::ReportType> &getReportTypeHash() const
            {
                static const QHash<QString, CMetar::ReportType> hash =
                {
                    { "METAR", CMetar::METAR },
                    { "SPECI", CMetar::SPECI }
                };
                return hash;
            }
        };

        class CMetarDecoderAirport : public IMetarDecoderPart
        {
        protected:
            QString getRegExp() const override { return QStringLiteral("^(?<airport>\\w{4}) "); }

            bool validateAndSet(const QRegularExpressionMatch &match, CMetar &metar) const override
            {
                QString airportAsString = match.captured("airport");
                Q_ASSERT(!airportAsString.isEmpty());
                metar.setAirportIcaoCode(CAirportIcaoCode(airportAsString));
                return true;
            }

            virtual bool isMandatory() const override { return true; }
            virtual QString getDecoderType() const override { return "Airport"; }
        };

        class CMetarDecoderDayTime : public IMetarDecoderPart
        {
        protected:
            QString getRegExp() const override { return QStringLiteral("^(?<day>\\d{2})(?<hour>\\d{2})(?<minute>\\d{2})Z "); }

            bool validateAndSet(const QRegularExpressionMatch &match, CMetar &metar) const override
            {
                bool ok = false;
                int day = match.captured("day").toInt(&ok);
                int hour = match.captured("hour").toInt(&ok);
                int minute = match.captured("minute").toInt(&ok);
                if (!ok) return false;

                if (day < 1 || day > 31) return false;
                if (hour < 0 || hour > 23) return false;
                if (minute < 0 || minute > 59) return false;

                BlackMisc::PhysicalQuantities::CTime time(hour, minute, 0);
                metar.setDayTime(day, time);
                return true;
            }

            virtual bool isMandatory() const override { return true; }
            virtual QString getDecoderType() const override { return "DayTime"; }
        };

        class CMetarDecoderStatus : public IMetarDecoderPart
        {
        protected:
            // Possible matches:
            // * (AUTO) - Automatic Station Indicator
            // * (NIL) - NO METAR
            // * (BBB) - Correction Indicator
            QString getRegExp() const override { return QStringLiteral("^([A-Z]+) "); }

            bool validateAndSet(const QRegularExpressionMatch &match, CMetar &metar) const override
            {
                if (match.captured(1) == "AUTO") { metar.setAutomated(true); return true; }
                else if (match.captured(1) == "NIL") { /* todo */ return true; }
                else if (match.captured(1).size() == 3) { /* todo */ return true; }
                else { return false; }
            }

            virtual bool isMandatory() const override { return false; }
            virtual QString getDecoderType() const override { return "Status"; }
        };

        class CMetarDecoderWind : public IMetarDecoderPart
        {
        protected:

            const QHash<QString, CSpeedUnit> &getWindUnitHash() const
            {
                static const QHash<QString, CSpeedUnit> hash =
                {
                    { "KT", CSpeedUnit::kts() },
                    { "MPS", CSpeedUnit::m_s() },
                    { "KPH", CSpeedUnit::km_h() },
                    { "KMH", CSpeedUnit::km_h() }
                };
                return hash;
            }

            QString getRegExp() const override
            {
                // Wind direction in three digits, 'VRB' or /// if no info available
                static const QString direction = QStringLiteral("(?<direction>\\d{3}|VRB|/{3})");
                // Wind speed in two digits (or three digits if required)
                static const QString speed = QStringLiteral("(?<speed>\\d{2,3}|/{2})");
                // Optional: Gust in two digits (or three digits if required)
                static const QString gustSpeed = QStringLiteral("(G(?<gustSpeed>\\d{2,3}))?");
                // Unit
                static const QString unit = QStringLiteral("(?<unit>") + QStringList(getWindUnitHash().keys()).join('|') + ")";
                // Regexp
                static const QString regexp = "^" + direction + speed + gustSpeed + unit + " ?";
                return regexp;
            }

            bool validateAndSet(const QRegularExpressionMatch &match, CMetar &metar) const override
            {
                bool ok = false;
                QString directionAsString = match.captured("direction");
                if (directionAsString == "///") return true;
                int direction = 0;
                bool directionVariable = false;
                if (directionAsString == "VRB")
                {
                    directionVariable = true;
                }
                else
                {
                    direction = directionAsString.toInt(&ok);
                    if (!ok) return false;
                }

                QString speedAsString = match.captured("speed");
                if (speedAsString == "//") return true;
                int speed = speedAsString.toInt(&ok);
                if (!ok) return false;
                QString gustAsString = match.captured("gustSpeed");
                int gustSpeed = 0;
                if (!gustAsString.isEmpty())
                {
                    gustSpeed = gustAsString.toInt(&ok);
                    if (!ok) return false;
                }
                QString unitAsString = match.captured("unit");
                if (!getWindUnitHash().contains(unitAsString)) return false;

                CWindLayer windLayer(CAltitude(0, CAltitude::AboveGround, CLengthUnit::ft()), CAngle(direction, CAngleUnit::deg()), CSpeed(speed, getWindUnitHash().value(unitAsString)),
                                     CSpeed(gustSpeed, getWindUnitHash().value(unitAsString)));
                windLayer.setDirectionVariable(directionVariable);
                metar.setWindLayer(windLayer);
                return true;
            }

            virtual bool isMandatory() const override { return false; }
            virtual QString getDecoderType() const override { return "Wind"; }
        };

        class CMetarDecoderVariationsWindDirection : public IMetarDecoderPart
        {
        protected:
            QString getRegExp() const override
            {
                // <from>V in degrees
                static const QString directionFrom("(?<direction_from>\\d{3})V");
                // <to> in degrees
                static const QString directionTo("(?<direction_to>\\d{3})");
                // Add space at the end
                static const QString regexp = "^" + directionFrom + directionTo + " ";
                return regexp;
            }

            bool validateAndSet(const QRegularExpressionMatch &match, CMetar &metar) const override
            {
                QString directionFromAsString = match.captured("direction_from");
                QString directionToAsString = match.captured("direction_to");

                int directionFrom = 0;
                int directionTo = 0;

                bool ok = false;
                directionFrom = directionFromAsString.toInt(&ok);
                directionTo = directionToAsString.toInt(&ok);
                if (!ok) return false;

                auto windLayer = metar.getWindLayer();
                windLayer.setDirection(CAngle(directionFrom, CAngleUnit::deg()), CAngle(directionTo, CAngleUnit::deg()));
                metar.setWindLayer(windLayer);
                return true;
            }

            virtual bool isMandatory() const override { return false; }
            virtual QString getDecoderType() const override { return "WindDirection"; }
        };

        class CMetarDecoderVisibility : public IMetarDecoderPart
        {
        protected:

            const QHash<QString, QString> &getCardinalDirections() const
            {
                static const QHash<QString, QString> hash =
                {
                    { "N", "north" },
                    { "NE", "north-east" },
                    { "E", "east" },
                    { "SE", "south-east" },
                    { "S", "south" },
                    { "SW", "south-west" },
                    { "W", "west" },
                    { "NW", "north-west" },
                };
                return hash;
            }

            QString getRegExp() const override
            {

                // CAVOK
                static const QString cavok = QStringLiteral("(?<cavok>CAVOK)");
                // European version:
                // Visibility of 4 digits in meter
                // Cardinal directions N, NE etc.
                // "////" in case no info is available
                // NDV = No Directional Variation
                static const QString visibility_eu = QStringLiteral("(?<visibility>\\d{4}|/{4})(NDV)?") + "(" + QStringList(getCardinalDirections().keys()).join('|') + ")?";
                // US/Canada version:
                // Surface visibility reported in statute miles.
                // A space divides whole miles and fractions.
                // Group ends with SM to indicate statute miles. For example,
                // 1 1/2SM.
                // Auto only: M prefixed to value < 1/4 mile, e.g., M1/4S
                static const QString visibility_us = QStringLiteral("(?<distance>\\d{0,2}) ?M?((?<numerator>\\d)/(?<denominator>\\d))?(?<unit>SM|KM)");

                static const QString regexp = "^(" + cavok + "|" + visibility_eu + "|" +  visibility_us + ") ";
                return regexp;
            }

            bool validateAndSet(const QRegularExpressionMatch &match, CMetar &metar) const override
            {
                bool ok = false;
                if (!match.captured("cavok").isEmpty()) { metar.setCavok(); return true; }
                QString visibilityAsString = match.captured("visibility");
                if (visibilityAsString == "////") return true;

                double visibility = 0;
                if (!visibilityAsString.isEmpty())
                {
                    visibility = visibilityAsString.toDouble(&ok);
                    if (!ok) return false;
                    metar.setVisibility(CLength(visibility, CLengthUnit::m()));
                    return true;
                }

                QString distanceAsString = match.captured("distance");
                if (!distanceAsString.isEmpty())
                {
                    visibility += distanceAsString.toDouble(&ok);
                    if (!ok) return false;
                }
                QString numeratorAsString = match.captured("numerator");
                QString denominatorAsString = match.captured("denominator");
                if (!numeratorAsString.isEmpty() && !denominatorAsString.isEmpty())
                {

                    double numerator = numeratorAsString.toDouble(&ok);
                    if (!ok) return false;

                    double denominator = denominatorAsString.toDouble(&ok);
                    if (!ok) return false;
                    if (denominator < 1 || numerator < 1) return false;
                    visibility += (numerator / denominator);
                }

                QString unitAsString = match.captured("unit");
                CLengthUnit unit = CLengthUnit::SM();
                if (unitAsString == "KM") unit = CLengthUnit::km();

                metar.setVisibility(CLength(visibility, unit));

                return true;
            }

            virtual bool isMandatory() const override { return false; }
            virtual QString getDecoderType() const override { return "Visibility"; }
        };

        class CMetarDecoderRunwayVisualRange : public IMetarDecoderPart
        {
        protected:
            QString getRegExp() const override
            {
                // 10 Minute RVR value: Reported in hundreds of feet if visibility is ≤ one statute mile
                // or RVR is ≤ 6000 feet. Group ends with FT to indicate feet. For example, R06L/2000FT.
                // The RVR value is prefixed with either M or P to indicate the value is lower or higher
                // than the RVR reportable values, e.g., R06L/P6000FT. If the RVR is variable during the 10
                // minute evaluation period, the variability is reported, e.g., R06L/2000V4000FT

                // Runway
                static const QString runway = QStringLiteral("R(?<runway>\\d{2}[LCR]*)");
                // Visibility
                static const QString visibility = QStringLiteral("/[PM]?(?<rwy_visibility>\\d{4})");
                // Variability
                static const QString variability = QStringLiteral("V?(?<variability>\\d{4})?");
                // Unit
                static const QString unit = QStringLiteral("(?<unit>FT)?");
                // Trend
                static const QString trend = QStringLiteral("/?(?<trend>[DNU])?");

                static const QString regexp = "^" + runway + visibility + variability + unit + trend + " ";
                return regexp;
            }

            bool validateAndSet(const QRegularExpressionMatch &match, CMetar &metar) const override
            {
                QString runway = match.captured("runway");
                QString runwayVisibilityAsString = match.captured("rwy_visibility");
                Q_ASSERT(!runway.isEmpty() && !runwayVisibilityAsString.isEmpty());

                bool ok = false;
                double runwayVisibility = runwayVisibilityAsString.toDouble(&ok);
                if (!ok) return false;
                CLengthUnit lengthUnit = CLengthUnit::m();
                if (match.captured("unit") == "FT") lengthUnit = CLengthUnit::ft();
                // Ignore for now until we make use of it.
                Q_UNUSED(metar);
                Q_UNUSED(runwayVisibility);
                Q_UNUSED(lengthUnit);
                return true;
            }

            virtual bool isMandatory() const override { return false; }
            virtual QString getDecoderType() const override { return "RunwayVisualRange"; }
        };

        class CMetarDecoderPresentWeather : public IMetarDecoderPart
        {
        protected:
            const QHash<QString, CPresentWeather::Intensity> &getIntensityHash() const
            {
                static const QHash<QString, CPresentWeather::Intensity> hash =
                {
                    { "-", CPresentWeather::Light },
                    { "+", CPresentWeather::Heavy },
                    { "VC", CPresentWeather::InVincinity }
                };
                return hash;
            }

            const QHash<QString, CPresentWeather::Descriptor> &getDescriptorHash() const
            {
                static const QHash<QString, CPresentWeather::Descriptor> hash =
                {
                    { "MI", CPresentWeather::Shallow },
                    { "BC", CPresentWeather::Patches },
                    { "PR", CPresentWeather::Partial },
                    { "DR", CPresentWeather::Drifting },
                    { "BL", CPresentWeather::Blowing },
                    { "SH", CPresentWeather::Showers },
                    { "TS", CPresentWeather::Thunderstorm },
                    { "FR", CPresentWeather::Freezing },
                };
                return hash;
            }

            const QHash<QString, CPresentWeather::WeatherPhenomenon> &getWeatherPhenomenaHash() const
            {
                static const QHash<QString, CPresentWeather::WeatherPhenomenon> hash =
                {
                    { "DZ", CPresentWeather::Drizzle },
                    { "RA", CPresentWeather::Rain },
                    { "SN", CPresentWeather::Snow },
                    { "SG", CPresentWeather::SnowGrains },
                    { "IC", CPresentWeather::IceCrystals },
                    { "PC", CPresentWeather::IcePellets },
                    { "GR", CPresentWeather::Hail },
                    { "GS", CPresentWeather::SnowPellets },
                    { "UP", CPresentWeather::Unknown },
                    { "BR", CPresentWeather::Mist },
                    { "FG", CPresentWeather::Fog },
                    { "FU", CPresentWeather::Smoke },
                    { "VA", CPresentWeather::VolcanicAsh },
                    { "DU", CPresentWeather::Dust },
                    { "SA", CPresentWeather::Sand },
                    { "HZ", CPresentWeather::Haze },
                    { "PO", CPresentWeather::DustSandWhirls },
                    { "SQ", CPresentWeather::Squalls },
                    { "FC", CPresentWeather::TornadoOrWaterspout },
                    { "FC", CPresentWeather::FunnelCloud },
                    { "SS", CPresentWeather::Sandstorm },
                    { "DS", CPresentWeather::Duststorm },
                    { "//", {} },
                };
                return hash;
            }

            virtual bool isRepeatable() const override { return true; }

            // w'w' represents present weather, coded in accordance with WMO Code Table 4678.
            // As many groups as necessary are included, with each group containing from 2 to 9 characters.
            // * Weather phenomena are preceded by one or two qualifiers
            // * No w'w' group has more than one descriptor.
            QString getRegExp() const override
            {
                // Qualifier intensity. (-) light (no sign) moderate (+) heavy or VC
                static const QString qualifier_intensity("(?<intensity>[-+]|VC)?");
                // Descriptor, if any
                static const QString qualifier_descriptor = "(?<descriptor>" + QStringList(getDescriptorHash().keys()).join('|') + ")?";
                static const QString weatherPhenomenaJoined = QStringList(getWeatherPhenomenaHash().keys()).join('|');
                static const QString weather_phenomina1 = "(?<wp1>" + weatherPhenomenaJoined + ")?";
                static const QString weather_phenomina2 = "(?<wp2>" + weatherPhenomenaJoined + ")?";
                static const QString weather_phenomina3 = "(?<wp3>" + weatherPhenomenaJoined + ")?";
                static const QString weather_phenomina4 = "(?<wp4>" + weatherPhenomenaJoined + ")?";
                static const QString regexp = "^(" + qualifier_intensity + qualifier_descriptor + weather_phenomina1 + weather_phenomina2 + weather_phenomina3 + weather_phenomina4 + ") ";
                return regexp;
            }

            bool validateAndSet(const QRegularExpressionMatch &match, CMetar &metar) const override
            {
                QString intensityAsString = match.captured("intensity");
                CPresentWeather::Intensity itensity = CPresentWeather::Moderate;
                if (!intensityAsString.isEmpty()) { itensity = getIntensityHash().value(intensityAsString); }

                QString descriptorAsString = match.captured("descriptor");
                CPresentWeather::Descriptor descriptor = CPresentWeather::None;
                if (!descriptorAsString.isEmpty()) { descriptor = getDescriptorHash().value(descriptorAsString); }

                int weatherPhenomena = 0;
                QString wp1AsString = match.captured("wp1");
                if (!wp1AsString.isEmpty()) { weatherPhenomena |= getWeatherPhenomenaHash().value(wp1AsString); }

                QString wp2AsString = match.captured("wp2");
                if (!wp2AsString.isEmpty()) { weatherPhenomena |= getWeatherPhenomenaHash().value(wp2AsString); }

                CPresentWeather presentWeather(itensity, descriptor, weatherPhenomena);
                metar.addPresentWeather(presentWeather);
                return true;
            }

            virtual bool isMandatory() const override { return false; }
            virtual QString getDecoderType() const override { return "PresentWeather"; }
        };

        class CMetarDecoderCloud : public IMetarDecoderPart
        {
        protected:
            const QStringList &getClearSkyTokens() const
            {
                static const QStringList list =
                {
                    "SKC",
                    "NSC",
                    "CLR",
                    "NCD"
                };
                return list;
            }

            const QHash<QString, CCloudLayer::Coverage> &getCoverage() const
            {
                static const QHash<QString, CCloudLayer::Coverage> hash =
                {
                    { "///", CCloudLayer::None },
                    { "FEW", CCloudLayer::Few },
                    { "SCT", CCloudLayer::Scattered },
                    { "BKN", CCloudLayer::Broken },
                    { "OVC", CCloudLayer::Overcast }
                };
                return hash;
            }

            virtual bool isRepeatable() const override { return true; }

            QString getRegExp() const override
            {
                // Clear sky
                static const QString clearSky = QString("(?<clear_sky>") + getClearSkyTokens().join('|') + QString(")");
                // Cloud coverage.
                static const QString coverage = QString("(?<coverage>") + QStringList(getCoverage().keys()).join('|') + QString(")");
                // Cloud base
                static const QString base = QStringLiteral("(?<base>\\d{3}|///)");
                // CB (Cumulonimbus) or TCU (Towering Cumulus) are appended to the cloud group without a space
                static const QString extra = QStringLiteral("(?<cb_tcu>CB|TCU|///)?");
                // Add space at the end
                static const QString regexp = QString("^(") + clearSky + '|' + coverage + base + extra + QString(") ");
                return regexp;
            }

            bool validateAndSet(const QRegularExpressionMatch &match, CMetar &metar) const override
            {
                QString noClouds = match.captured("clear_sky");
                if (!noClouds.isEmpty())
                {
                    metar.removeAllClouds();
                    return true;
                }

                QString coverageAsString = match.captured("coverage");
                QString baseAsString = match.captured("base");
                Q_ASSERT(!coverageAsString.isEmpty() && !baseAsString.isEmpty());
                Q_ASSERT(getCoverage().contains(coverageAsString));
                if (baseAsString == "///") return true;

                bool ok = false;
                int base = baseAsString.toInt(&ok);
                // Factor 100
                base *= 100;
                if (!ok) return false;

                CCloudLayer cloudLayer(CAltitude(base, CAltitude::AboveGround, CLengthUnit::ft()), {}, getCoverage().value(coverageAsString));
                metar.addCloudLayer(cloudLayer);
                QString cb_tcu = match.captured("cb_tcu");
                if (!cb_tcu.isEmpty()) { }
                return true;
            }

            virtual bool isMandatory() const override { return false; }
            virtual QString getDecoderType() const override { return "Cloud"; }
        };

        class CMetarDecoderVerticalVisibility : public IMetarDecoderPart
        {
        protected:
            QString getRegExp() const override
            {
                // Vertical visibility
                static const QString verticalVisibility = QStringLiteral("VV(?<vertical_visibility>\\d{3}|///)");

                static const QString regexp = "^" + verticalVisibility + " ";
                return regexp;
            }

            bool validateAndSet(const QRegularExpressionMatch &/* match */, CMetar &/* metar */) const override
            {
                // todo
                return true;
            }

            virtual bool isMandatory() const override { return false; }
            virtual QString getDecoderType() const override { return "VerticalVisibility"; }
        };

        class CMetarDecoderTemperature : public IMetarDecoderPart
        {
        protected:
            QString getRegExp() const override
            {
                // Tmperature
                static const QString temperature = QStringLiteral("(?<temperature>M?\\d{2}|//)");
                // Separator
                static const QString separator = "/";
                // Dew point
                static const QString dewPoint = QStringLiteral("(?<dew_point>M?\\d{2}|//)");
                // Add space at the end
                static const QString regexp = "^" + temperature + separator + dewPoint + " ?";
                return regexp;
            }

            bool validateAndSet(const QRegularExpressionMatch &match, CMetar &metar) const override
            {
                QString temperatureAsString = match.captured("temperature");
                if (temperatureAsString.isEmpty()) return false;
                QString dewPointAsString = match.captured("dew_point");
                if (dewPointAsString.isEmpty()) return false;

                if (temperatureAsString == "//" || dewPointAsString == "//") return true;

                bool temperatureNegative = false;
                if (temperatureAsString.startsWith('M'))
                {
                    temperatureNegative = true;
                    temperatureAsString.remove('M');
                }

                bool dewPointNegative = false;
                if (dewPointAsString.startsWith('M'))
                {
                    dewPointNegative = true;
                    dewPointAsString.remove('M');
                }

                int temperature = temperatureAsString.toInt();
                if (temperatureNegative) { temperature *= -1; }
                metar.setTemperature(CTemperature(temperature, CTemperatureUnit::C()));

                int dewPoint = dewPointAsString.toInt();
                if (dewPointNegative) { dewPoint *= -1; }
                metar.setDewPoint(CTemperature(dewPoint, CTemperatureUnit::C()));

                return true;
            }

            virtual bool isMandatory() const override { return false; }
            virtual QString getDecoderType() const override { return "Temperature"; }
        };

        class CMetarDecoderPressure : public IMetarDecoderPart
        {
        protected:

            const QHash<QString, CPressureUnit> &getPressureUnits() const
            {
                static const QHash<QString, CPressureUnit> hash =
                {
                    { "Q", CPressureUnit::hPa() },
                    { "A", CPressureUnit::inHg() }
                };
                return hash;
            }

            QString getRegExp() const override
            {
                // Q => QNH comes in hPa
                // A => QNH comes in inches of Mercury
                static const QString unit = QStringLiteral("((?<unit>Q|A)");
                // Pressure
                static const QString pressure = QStringLiteral("(?<pressure>\\d{4}|////) ?)");
                // QFE
                static const QString qfe = QStringLiteral("(QFE (?<qfe>\\d+).\\d");

                static const QString regexp = "^" + unit + pressure + "|" + qfe + " ?)";
                return regexp;
            }

            bool validateAndSet(const QRegularExpressionMatch &match, CMetar &metar) const override
            {
                QString unitAsString = match.captured("unit");
                QString pressureAsString = match.captured("pressure");
                QString qfeAsString = match.captured("qfe");
                if ((unitAsString.isEmpty() || pressureAsString.isEmpty()) && qfeAsString.isEmpty()) return false;

                // In case no value is defined
                if (pressureAsString == "////") return true;

                if (!unitAsString.isEmpty() && !pressureAsString.isEmpty())
                {
                    Q_ASSERT(getPressureUnits().contains(unitAsString));
                    bool ok = false;
                    double pressure = pressureAsString.toDouble(&ok);
                    if (!ok) return false;
                    CPressureUnit pressureUnit = getPressureUnits().value(unitAsString);
                    if (pressureUnit == CPressureUnit::inHg()) pressure /= 100;
                    metar.setAltimeter(CPressure(pressure, pressureUnit));
                    return true;
                }

                if (!qfeAsString.isEmpty())
                {
                    // todo QFE
                    return true;
                }
                return false;
            }

            virtual bool isMandatory() const override { return false; }
            virtual QString getDecoderType() const override { return "Pressure"; }
        };

        class CMetarDecoderRecentWeather : public IMetarDecoderPart
        {
        protected:
            QString getRegExp() const override
            {
                // Qualifier intensity. (-) light (no sign) moderate (+) heavy or VC
                static const QString qualifier_intensity("(?<intensity>[-+]|VC)?");
                // Descriptor, if any
                static const QString qualifier_descriptor = "(?<descriptor>" + m_descriptor.join('|') + ")?";
                static const QString weather_phenomina1 = "(?<wp1>" + m_phenomina.join('|') + ")?";
                static const QString weather_phenomina2 = "(?<wp2>" + m_phenomina.join('|') + ")?";
                static const QString weather_phenomina3 = "(?<wp3>" + m_phenomina.join('|') + ")?";
                static const QString weather_phenomina4 = "(?<wp4>" + m_phenomina.join('|') + ")?";

                static const QString regexp = "^RE" + qualifier_intensity + qualifier_descriptor + weather_phenomina1 + weather_phenomina2 + weather_phenomina3 + weather_phenomina4 + " ";
                return regexp;
            }

            bool validateAndSet(const QRegularExpressionMatch &/** match **/, CMetar &/** metar **/) const override
            {
                // Ignore for now
                return true;
            }

            virtual bool isMandatory() const override { return false; }
            virtual QString getDecoderType() const override { return "RecentWeather"; }

        private:
            const QStringList m_descriptor = QStringList { "MI", "BC", "PR", "DR", "BL", "SH", "TS", "FZ" };
            const QStringList m_phenomina = QStringList { "DZ", "RA", "SN", "SG", "IC", "PE", "GR", "GS",
                                                                "BR", "FG", "FU", "VA", "IC", "DU", "SA", "HZ",
                                                                "PY", "PO", "SQ", "FC", "SS", "DS"
                                                              };
        };

        class CMetarDecoderWindShear : public IMetarDecoderPart
        {
        protected:
            QString getRegExp() const override
            {

                // Wind shear on all runways
                static const QString wsAllRwy = QStringLiteral("WS ALL RWY");
                // RWY designator
                static const QString runway = QStringLiteral("RW?Y?(?<runway>\\d{2}[LCR]*)");

                static const QString regexp = "^WS (" + wsAllRwy + "|" + runway + ") ";
                return regexp;
            }

            bool validateAndSet(const QRegularExpressionMatch &match, CMetar &metar) const override
            {
                QString runwayAsString = match.captured("runway");
                if (!runwayAsString.isEmpty())
                {
                    // Ignore for now until we make use of it.
                    Q_UNUSED(runwayAsString);
                    Q_UNUSED(metar);
                }

                return true;
            }

            virtual bool isMandatory() const override { return false; }
            virtual QString getDecoderType() const override { return "WindShear"; }
        };

        CMetarDecoder::CMetarDecoder()
        {
            allocateDecoders();
        }

        CMetarDecoder::~CMetarDecoder()
        {
        }

        CMetar CMetarDecoder::decode(const QString &metarString) const
        {
            CMetar metar;
            QString metarStringCopy = metarString.simplified();

            for (const auto &decoder : m_decoders)
            {
                if (!decoder->parse(metarStringCopy, metar))
                {
                    CLogMessage(this).debug() << "Invalid metar:" << metarString;
                    return CMetar();
                }
            }

            metar.setMessage(metarString);
            return metar;
        }

        void CMetarDecoder::allocateDecoders()
        {
            m_decoders.clear();
            m_decoders.push_back(std::make_unique<CMetarDecoderReportType>());
            m_decoders.push_back(std::make_unique<CMetarDecoderAirport>());
            m_decoders.push_back(std::make_unique<CMetarDecoderDayTime>());
            m_decoders.push_back(std::make_unique<CMetarDecoderStatus>());
            m_decoders.push_back(std::make_unique<CMetarDecoderWind>());
            m_decoders.push_back(std::make_unique<CMetarDecoderVariationsWindDirection>());
            m_decoders.push_back(std::make_unique<CMetarDecoderVisibility>());
            m_decoders.push_back(std::make_unique<CMetarDecoderRunwayVisualRange>());
            m_decoders.push_back(std::make_unique<CMetarDecoderPresentWeather>());
            m_decoders.push_back(std::make_unique<CMetarDecoderCloud>());
            m_decoders.push_back(std::make_unique<CMetarDecoderVerticalVisibility>());
            m_decoders.push_back(std::make_unique<CMetarDecoderTemperature>());
            m_decoders.push_back(std::make_unique<CMetarDecoderPressure>());
            m_decoders.push_back(std::make_unique<CMetarDecoderRecentWeather>());
            m_decoders.push_back(std::make_unique<CMetarDecoderWindShear>());
        }

    } // namespace
} // namespace

//! \endcond
