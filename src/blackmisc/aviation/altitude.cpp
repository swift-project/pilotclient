/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/pq/measurementunit.h"
#include "blackmisc/pq/constants.h"
#include "blackmisc/pq/pqstring.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/iconlist.h"
#include "blackmisc/icons.h"

#include <Qt>
#include <QtGlobal>
#include <QStringBuilder>

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Aviation
    {
        void CAltitude::registerMetadata()
        {
            Mixin::MetaType<CAltitude>::registerMetadata();
            qRegisterMetaType<CAltitude::ReferenceDatum>();
            qRegisterMetaType<CAltitude::AltitudeType>();
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

            if (this->m_datum == FlightLevel)
            {
                static const QString fls("FL%1");
                const int fl = qRound(this->CLength::value(CLengthUnit::ft()) / 100.0);
                return fls.arg(fl);
            }
            else
            {
                return this->CLength::valueRoundedWithUnit(1, i18n) %
                       (this->isMeanSeaLevel() ? QStringLiteral(" MSL") : QStringLiteral(" AGL"));
            }
        }

        void CAltitude::toFlightLevel()
        {
            Q_ASSERT(this->m_datum == MeanSeaLevel || this->m_datum == FlightLevel);
            this->m_datum = FlightLevel;
        }

        void CAltitude::toMeanSeaLevel()
        {
            Q_ASSERT(this->m_datum == MeanSeaLevel || this->m_datum == FlightLevel);
            this->m_datum = MeanSeaLevel;
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
            this->parseFromString(value, CPqString::SeparatorsCLocale);
        }

        void CAltitude::parseFromString(const QString &value, CPqString::SeparatorMode mode)
        {
            QString v = value.trimmed();

            // special case FL
            if (v.contains("FL", Qt::CaseInsensitive))
            {
                v = v.replace("FL", "", Qt::CaseInsensitive).trimmed();
                bool ok = false;
                double dv = v.toDouble(&ok) * 100.0;
                const CAltitude a(ok ? dv : 0.0, FlightLevel,
                                  ok ? CLengthUnit::ft() : nullptr);
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
            QString v(value.trimmed());
            this->setNull();
            if (v.isEmpty() || v.length() < 3)
            {
                if (msgs) { msgs->push_back(CStatusMessage(this).validationError("Altitude empty or too short")); }
                return false;
            }

            if (!fpAltitudeRegExp().globalMatch(v).hasNext())
            {
                if (msgs) { msgs->push_back(CStatusMessage(this).validationError("Altitude '%1' needs to match format '%2'") << v << fpAltitudeExamples()); }
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
                if (msgs) { msgs->push_back(CStatusMessage(this).validationError("Altitude '%1' has no numeric part")); }
                return false;
            }

            bool ok;
            if (v.startsWith("F"))
            {
                this->setUnit(CLengthUnit::ft());
                this->setValueSameUnit(numericPart.toInt(&ok) * 100);
                m_datum = FlightLevel;
            }
            else if (v.startsWith("S"))
            {
                this->setUnit(CLengthUnit::m());
                this->setValueSameUnit(numericPart.toInt(&ok) * 10);
                m_datum = FlightLevel;
            }
            else if (v.startsWith("A"))
            {
                this->setUnit(CLengthUnit::ft());
                this->setValueSameUnit(numericPart.toInt(&ok) * 100);
                m_datum = MeanSeaLevel;
            }
            else if (v.startsWith("M"))
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
                if (msgs) { msgs->push_back(CStatusMessage(this).validationError("Altitude '%1' needs to match format '%2'") << v << fpAltitudeExamples()); }
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
                if (msgs) { msgs->push_back(CStatusMessage(this).validationError("Altitude NULL value")); }
                return false;
            }
            if (!(this->getReferenceDatum() == FlightLevel || this->getReferenceDatum() == MeanSeaLevel))
            {
                if (msgs) { msgs->push_back(CStatusMessage(this).validationError("Altitude, must be FL or MSL")); }
                return false;
            }
            if (!(this->getUnit() == CLengthUnit::m() || this->getUnit() == CLengthUnit::ft()))
            {
                if (msgs) { msgs->push_back(CStatusMessage(this).validationError("Altitude, valid unit must be 'ft' or 'm'")); }
                return false;
            }
            if (this->isNegativeWithEpsilonConsidered())
            {
                if (msgs) { msgs->push_back(CStatusMessage(this).validationError("Altitude must be positive")); }
                return false;
            }
            if (this->isFlightLevel())
            {
                if (!this->isInteger())
                {
                    if (msgs) { msgs->push_back(CStatusMessage(this).validationError("FL needs to be positive integer")); }
                    return false;
                }

                if (this->getUnit() == CLengthUnit::ft())
                {
                    const int flInt = this->valueInteger() / 100; // internally represented as ft: FL10->1000
                    if (flInt < 10 || flInt >= 1000)
                    {
                        if (msgs) { msgs->push_back(CStatusMessage(this).validationError("FL range is 10-999")); }
                        return false;
                    }
                    if (fmod(flInt, 5) != 0)
                    {
                        if (msgs) { msgs->push_back(CStatusMessage(this).validationError("FL needs to end with 0 or 5")); }
                        return false;
                    }
                }
            }
            return true;
        }

        QString CAltitude::asFpAltitudeString() const
        {
            if (this->isNull()) { return QStringLiteral(""); }


            if (this->isFlightLevel())
            {
                if (this->getUnit() == CLengthUnit::m())
                {
                    int m = this->valueInteger() / 10;
                    return QString("S%1").arg(m, 4, 10, QChar('0'));
                }
                int ft = this->valueInteger(CLengthUnit::ft()) / 100;
                return QString("FL%1").arg(ft, 3, 10, QChar('0'));
            }

            if (this->getUnit() == CLengthUnit::m())
            {
                int m = this->valueInteger() / 10;
                return QString("M%1").arg(m, 4, 10, QChar('0'));
            }
            int ft = this->valueInteger(CLengthUnit::ft()) / 100;
            return QString("A%1").arg(ft, 3, 10, QChar('0'));
        }

        QString CAltitude::asFpAltitudeSimpleVatsimString() const
        {
            CAltitude copy(*this);
            copy.switchUnit(CLengthUnit::ft());
            if (copy.isFlightLevel()) { return copy.asFpAltitudeString(); }
            return QString::number(copy.valueInteger()); // ft altitude without unit
        }

        const QRegularExpression &CAltitude::fpAltitudeRegExp()
        {
            thread_local const QRegularExpression re("((FL|F)\\d{2,3})|(S\\d{2,4})|(A\\d{2,3})|(M\\d{2,4})|(\\d{3,5}(ft|m))");
            return re;
        }

        QString CAltitude::fpAltitudeInfo(const QString &separator)
        {
            static const QString e("FL085, F85 flight level%1S0150 metric level in tens of metres%1A055 altitude in hundreds of feet%12000ft altitude in ft%1M0610 altitude in tens of metres%16100m altitude in meters");
            return e.arg(separator);
        }

        QString CAltitude::fpAltitudeExamples()
        {
            static const QString e("FL085, F85, S0150, A055, 1200ft, M0610, 6100m");
            return e;
        }

        CIcon CAltitude::toIcon() const
        {
            return BlackMisc::CIcon::iconByIndex(CIcons::GeoPosition);
        }

        int CAltitude::compare(const CAltitude &otherAltitude) const
        {
            if (this->getReferenceDatum() != otherAltitude.getReferenceDatum())
            {
                return Compare::compare(static_cast<int>(this->getReferenceDatum()), static_cast<int>(otherAltitude.getReferenceDatum()));
            }
            return CLength::compare(*this, otherAltitude);
        }

        const CAltitude &CAltitude::null()
        {
            static const CAltitude null(0, CAltitude::MeanSeaLevel, CLengthUnit::nullUnit());
            return null;
        }

        const CLengthUnit &CAltitude::defaultUnit()
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
} // namespace
