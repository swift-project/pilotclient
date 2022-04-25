/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/pq/measurementunit.h"
#include "blackmisc/pq/constants.h"
#include "blackmisc/pq/pqstring.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/iconlist.h"
#include "blackmisc/icons.h"

#include <Qt>
#include <QtGlobal>
#include <QStringBuilder>

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Math;

namespace BlackMisc::Aviation
{
    namespace Private
    {
        QVector<CAltitude::MetricTuple> initMetricValues()
        {
            QVector<CAltitude::MetricTuple> v;
            const QString f = CFileUtils::appendFilePaths(CSwiftDirectories::shareMiscDirectory(), "Metric Altitudes.csv");
            const QString ma = CFileUtils::readFileToString(f);
            const QStringList values = splitLines(ma);
            for (const QString &value : values)
            {
                const QStringList alts = value.split(";");
                int v1 = -1, v2 = -1, v3 = -1;
                if (alts.size() >= 1) { v1 = alts[0].toInt(); }
                if (alts.size() >= 2) { v2 = alts[1].toInt(); }
                if (alts.size() >= 3) { v3 = alts[2].toInt(); }
                const CAltitude::MetricTuple m(v1, v2, v3);
                v.push_back(m);
            }
            return v;
        }
    }

    void CAltitude::registerMetadata()
    {
        Mixin::MetaType<CAltitude>::registerMetadata();
        qRegisterMetaType<CAltitude::ReferenceDatum>();
        qRegisterMetaType<CAltitude::AltitudeType>();
    }

    const QVector<CAltitude::MetricTuple> &CAltitude::metricTuples()
    {
        static const QVector<MetricTuple> v = Private::initMetricValues();
        return v;
    }

    CAltitude::CAltitude(const QString &altitudeAsString, CPqString::SeparatorMode mode) : CLength(0, CLengthUnit::m()), m_datum(MeanSeaLevel)
    {
        this->parseFromString(altitudeAsString, mode);
    }

    CAltitude CAltitude::withOffset(const CLength &offset) const
    {
        if (this->isNull()) { return CAltitude::null(); }
        CAltitude copy(*this);
        if (!offset.isNull() && !offset.isZeroEpsilonConsidered())
        {
            copy += offset.switchedUnit(this->getUnit());
        }
        return copy;
    }

    void CAltitude::addOffset(const CLength &offset)
    {
        *this = this->withOffset(offset);
    }

    CAltitude &CAltitude::switchUnit(const CLengthUnit &newUnit)
    {
        if (newUnit.isNull() || this->getUnit().isNull() || this->getUnit() == newUnit) { return *this; }
        CLength::switchUnit(newUnit);
        return *this;
    }

    CAltitude CAltitude::switchedUnit(const CLengthUnit &newUnit) const
    {
        if (newUnit.isNull() || this->getUnit().isNull() || this->getUnit() == newUnit) { return *this; }
        CAltitude copy(*this);
        copy.switchUnit(newUnit);
        return copy;
    }

    QString CAltitude::convertToQString(bool i18n) const
    {
        static const QString n("null");
        if (this->isNull()) { return n; }

        if (m_datum == FlightLevel)
        {
            const int fl = qRound(this->CLength::value(CLengthUnit::ft()) / 100.0);
            return QStringLiteral("FL%1").arg(fl);
        }
        else
        {
            return this->CLength::valueRoundedWithUnit(1, i18n) %
                    (this->isMeanSeaLevel() ? u" MSL" : u" AGL");
        }
    }

    int CAltitude::comparePropertyByIndex(CPropertyIndexRef index, const CAltitude &compareValue) const
    {
        if (index.isMyself()) { return this->compare(compareValue); }
        return CLength::comparePropertyByIndex(index, compareValue);
    }

    bool CAltitude::toFlightLevel()
    {
        if (m_datum != MeanSeaLevel && m_datum != FlightLevel) { return false; }
        m_datum = FlightLevel;
        return true;
    }

    bool CAltitude::toMeanSeaLevel()
    {
        if (m_datum != MeanSeaLevel && m_datum != FlightLevel) { return false; }
        m_datum = MeanSeaLevel;
        return true;
    }

    void CAltitude::convertToPressureAltitude(const CPressure &seaLevelPressure)
    {
        if (m_altitudeType == PressureAltitude) { return; }
        if (this->isNull()) { return; }
        const CPressure deltaPressure = standardISASeaLevelPressure() - seaLevelPressure;
        const double deltaPressureV = deltaPressure.value(CPressureUnit::mbar());
        const double deltaAltitudeV = deltaPressureV * 30.0; // 30.0 ft per mbar
        CLength deltaAltitude(deltaAltitudeV, CLengthUnit::ft());
        *this += deltaAltitude;
        m_altitudeType = PressureAltitude;
    }

    CAltitude CAltitude::toPressureAltitude(const CPressure &seaLevelPressure) const
    {
        if (seaLevelPressure.isNull()) { return CAltitude::null(); }
        if (this->isNull()) { return CAltitude::null(); }
        CAltitude other(*this);
        other.convertToPressureAltitude(seaLevelPressure);
        return other;
    }

    void CAltitude::parseFromString(const QString &value)
    {
        this->parseFromString(value, CPqString::SeparatorBestGuess);
    }

    void CAltitude::parseFromString(const QString &value, CPqString::SeparatorMode mode)
    {
        QString v = value.trimmed();

        // special case FL
        if (v.contains("FL", Qt::CaseInsensitive) || v.startsWith("F"))
        {
            v = char09OnlyString(value);
            bool ok = false;
            const int dv = v.toInt(&ok) * 100;
            const CAltitude a(ok ? dv : 0.0, FlightLevel, ok ? CLengthUnit::ft() : nullptr);
            *this = a;
            return;
        }

        // special case A (altitude
        if (v.contains("ALT", Qt::CaseInsensitive) || v.startsWith("A"))
        {
            v = char09OnlyString(value);
            bool ok = false;
            const int dv = v.toInt(&ok) * 100;
            const CAltitude a(ok ? dv : 0.0, MeanSeaLevel, ok ? CLengthUnit::ft() : nullptr);
            *this = a;
            return;
        }

        // normal altitude, AGL/MSL
        ReferenceDatum rd = MeanSeaLevel;
        if (v.contains("MSL", Qt::CaseInsensitive))
        {
            v = v.replace("MSL", "", Qt::CaseInsensitive).trimmed();
            rd = MeanSeaLevel;
        }
        else if (v.contains("AGL"))
        {
            v = v.replace("AGL", "", Qt::CaseInsensitive).trimmed();
            rd = AboveGround;
        }

        const CLength l = CPqString::parse<CLength>(v, mode);
        *this = CAltitude(l, rd);
    }

    bool CAltitude::parseFromFpAltitudeString(const QString &value, CStatusMessageList *msgs)
    {
        QString v(value.trimmed()); // do not convert case because of units
        if (v.startsWith("VFR", Qt::CaseInsensitive))
        {
            // we set a more or less meaningful value
            *this = CAltitude(5000, MeanSeaLevel, CLengthUnit::ft());
            return true;
        }

        this->setNull();
        if (v.length() < 3)
        {
            if (msgs) { msgs->push_back(CStatusMessage(this).validationError(u"Altitude empty or too short")); }
            return false;
        }

        if (!fpAltitudeRegExp().globalMatch(v).hasNext())
        {
            if (msgs) { msgs->push_back(CStatusMessage(this).validationError(u"Altitude '%1' needs to match format '%2'") << v << fpAltitudeExamples()); }
            return false;
        }

        // normalize characters to upper/lower
        // in same step get numeric value only
        bool beforeDigit = true;
        QString numericPart;
        for (int i = 0; i < v.length(); i++)
        {
            const QChar c = v[i];
            if (c.isDigit())
            {
                beforeDigit = false;
                numericPart.push_back(c);
                continue;
            }
            v[i] = beforeDigit ? c.toUpper() : c.toLower();
        }

        if (numericPart.isEmpty())
        {
            if (msgs) { msgs->push_back(CStatusMessage(this).validationError(u"Altitude '%1' has no numeric part")); }
            return false;
        }

        bool ok;
        if (v.startsWith("F", Qt::CaseInsensitive))
        {
            this->setUnit(CLengthUnit::ft());
            this->setValueSameUnit(numericPart.toInt(&ok) * 100);
            m_datum = FlightLevel;
        }
        else if (v.startsWith("S", Qt::CaseInsensitive))
        {
            this->setUnit(CLengthUnit::m());
            this->setValueSameUnit(numericPart.toInt(&ok) * 10);
            m_datum = FlightLevel;
        }
        else if (v.startsWith("A", Qt::CaseInsensitive))
        {
            this->setUnit(CLengthUnit::ft());
            this->setValueSameUnit(numericPart.toInt(&ok) * 100);
            m_datum = MeanSeaLevel;
        }
        else if (v.startsWith("M", Qt::CaseInsensitive))
        {
            this->setUnit(CLengthUnit::m());
            this->setValueSameUnit(numericPart.toInt(&ok) * 10);
            m_datum = MeanSeaLevel;
        }
        else if (v.endsWith(CLengthUnit::m().getSymbol()))
        {
            this->setUnit(CLengthUnit::m());
            this->setValueSameUnit(numericPart.toInt(&ok));
            m_datum = MeanSeaLevel;
        }
        else if (v.endsWith(CLengthUnit::ft().getSymbol()))
        {
            this->setUnit(CLengthUnit::ft());
            this->setValueSameUnit(numericPart.toInt(&ok));
            m_datum = MeanSeaLevel;
        }
        else
        {
            if (msgs) { msgs->push_back(CStatusMessage(this).validationError(u"Altitude '%1' needs to match format '%2'") << v << fpAltitudeExamples()); }
            return false;
        }

        // further checks
        const bool valid = this->isValidFpAltitude(msgs);
        if (!valid) { this->setNull(); }
        return valid;
    }

    bool CAltitude::isValidFpAltitude(CStatusMessageList *msgs) const
    {
        if (this->isNull())
        {
            if (msgs) { msgs->push_back(CStatusMessage(this).validationError(u"Altitude NULL value")); }
            return false;
        }
        if (!(this->getReferenceDatum() == FlightLevel || this->getReferenceDatum() == MeanSeaLevel))
        {
            if (msgs) { msgs->push_back(CStatusMessage(this).validationError(u"Altitude, must be FL or MSL")); }
            return false;
        }
        if (!(this->getUnit() == CLengthUnit::m() || this->getUnit() == CLengthUnit::ft()))
        {
            if (msgs) { msgs->push_back(CStatusMessage(this).validationError(u"Altitude, valid unit must be 'ft' or 'm'")); }
            return false;
        }
        if (this->isNegativeWithEpsilonConsidered())
        {
            if (msgs) { msgs->push_back(CStatusMessage(this).validationError(u"Altitude must be positive")); }
            return false;
        }
        if (this->isFlightLevel())
        {
            if (!this->isInteger())
            {
                if (msgs) { msgs->push_back(CStatusMessage(this).validationError(u"FL needs to be positive integer")); }
                return false;
            }

            if (this->getUnit() == CLengthUnit::ft())
            {
                const int flInt = this->valueInteger() / 100; // internally represented as ft: FL10->1000
                if (flInt < 10 || flInt >= 1000)
                {
                    if (msgs) { msgs->push_back(CStatusMessage(this).validationError(u"FL range is 10-999")); }
                    return false;
                }
                if (!CMathUtils::epsilonZero(fmod(flInt, 5)))
                {
                    if (msgs) { msgs->push_back(CStatusMessage(this).validationWarning(u"FL should end with 0 or 5")); }
                }
            }
        }
        return true;
    }

    QString CAltitude::asFpICAOAltitudeString() const
    {
        if (this->isNull()) { return {}; }
        if (this->isFlightLevel())
        {
            if (this->getUnit() == CLengthUnit::m())
            {
                int m = this->valueInteger() / 10;
                return QStringLiteral("S%1").arg(m, 4, 10, QChar('0'));
            }
            int ft = this->valueInteger(CLengthUnit::ft()) / 100;
            return QStringLiteral("FL%1").arg(ft, 3, 10, QChar('0'));
        }

        if (this->getUnit() == CLengthUnit::m())
        {
            int m = this->valueInteger() / 10;
            return QStringLiteral("M%1").arg(m, 4, 10, QChar('0'));
        }
        int ft = this->valueInteger(CLengthUnit::ft()) / 100;
        return QStringLiteral("A%1").arg(ft, 3, 10, QChar('0'));
    }

    QString CAltitude::asFpVatsimAltitudeString() const
    {
        if (this->isFlightLevel()) { return this->asFpICAOAltitudeString(); }
        // the M/A formats are not supported by VATSIM, means by other clients

        // as feed, as none of the other clients
        const CAltitude a = this->roundedToNearest100ft(false);
        // return a.valueRoundedWithUnit(CLengthUnit::ft(), 0);
        return a.valueIntegerAsString(CLengthUnit::ft());
    }

    const QRegularExpression &CAltitude::fpAltitudeRegExp()
    {
        thread_local const QRegularExpression re("([Ff][Ll]?\\d{2,3})|([Ss]\\d{2,4})|([Aa]\\d{2,3})|([Mm]\\d{2,4})|(\\d{3,5}(ft|m))");
        return re;
    }

    QString CAltitude::fpAltitudeInfo(const QString &separator)
    {
        // remark use arg %01 to avoid clash with numbers, see https://stackoverflow.com/questions/35517025/qstringarg-with-number-after-placeholder
        static const QString e("FL085, F85 flight level in hecto feets%1S0150 metric level in tens of meters%1A055 altitude in hundreds of feet%012000ft altitude in ft%1M0610 altitude in tens of meters%016100m altitude in meters");
        return e.arg(separator);
    }

    QString CAltitude::fpAltitudeExamples()
    {
        static const QString e("FL085, F85, S0150, A055, 1200ft, M0610, 6100m");
        return e;
    }

    CIcons::IconIndex CAltitude::toIcon() const
    {
        return CIcons::GeoPosition;
    }

    int CAltitude::compare(const CAltitude &otherAltitude) const
    {
        if (this->getReferenceDatum() != otherAltitude.getReferenceDatum())
        {
            return Compare::compare(static_cast<int>(this->getReferenceDatum()), static_cast<int>(otherAltitude.getReferenceDatum()));
        }
        return CLength::compare(*this, otherAltitude);
    }

    CAltitude CAltitude::roundedToNearest100ft(bool roundDown) const
    {
        // 23453 => 234.53
        CAltitude a = this->switchedUnit(CLengthUnit::ft());
        const double ft = a.value(CLengthUnit::ft()) / 100.0;
        const int ftR = roundDown ? static_cast<int>(floor(ft)) * 100 : qRound(ft) * 100;
        a.setValueSameUnit(ftR);
        return a;
    }

    int CAltitude::findMetricAltitude(int feet)
    {
        // m/ft/FL
        for (const MetricTuple &m : metricTuples())
        {
            if (std::get<1>(m) == feet) return std::get<0>(m);
        }
        return -1;
    }

    int CAltitude::findAltitudeForMetricAltitude(int metric)
    {
        // m/ft/FL
        for (const MetricTuple &m : metricTuples())
        {
            if (std::get<0>(m) == metric) return std::get<1>(m);
        }
        return -1;
    }

    const CAltitude &CAltitude::null()
    {
        static const CAltitude null(0, CAltitude::MeanSeaLevel, CLengthUnit::nullUnit());
        return null;
    }

    CLengthUnit CAltitude::defaultUnit()
    {
        return CLengthUnit::ft();
    }

    const CPressure &CAltitude::standardISASeaLevelPressure()
    {
        // Average sea-level pressure is 1013.25mbar or 1013.25hPa
        static const CPressure standardPressure(CPhysicalQuantitiesConstants::ISASeaLevelPressure());
        return standardPressure;
    }
} // namespace
