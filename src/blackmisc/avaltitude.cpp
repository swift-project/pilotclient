/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "avaltitude.h"
#include "pqstring.h"
#include "iconlist.h"

using BlackMisc::PhysicalQuantities::CLength;
using BlackMisc::PhysicalQuantities::CLengthUnit;

namespace BlackMisc
{
    namespace Aviation
    {

        /*
         * Constructor
         */
        CAltitude::CAltitude(const QString &altitudeAsString, BlackMisc::PhysicalQuantities::CPqString::SeparatorMode mode) : CValueObject(0, BlackMisc::PhysicalQuantities::CLengthUnit::m()), m_datum(MeanSeaLevel)
        {
            this->parseFromString(altitudeAsString, mode);
        }

        /*
         * Own implementation for streaming
         */
        QString CAltitude::convertToQString(bool i18n) const
        {
            if (this->m_datum == FlightLevel)
            {
                int fl = qRound(this->CLength::value(CLengthUnit::ft()) / 100.0);
                return QString("FL%1").arg(fl);
            }
            else
            {
                QString s = this->CLength::valueRoundedWithUnit(4, i18n);
                if (this->getUnit() != CLengthUnit::ft())
                {
                    s.append(" (").append(this->valueRoundedWithUnit(CLengthUnit::ft(), 4, i18n)).append(")");
                }
                return s.append(this->isMeanSeaLevel() ? " MSL" : " AGL");
            }
        }

        /*
         * To FL
         */
        void CAltitude::toFlightLevel()
        {
            Q_ASSERT(this->m_datum == MeanSeaLevel || this->m_datum == FlightLevel);
            this->m_datum = FlightLevel;
        }

        /*
         * To MSL
         */
        void CAltitude::toMeanSeaLevel()
        {
            Q_ASSERT(this->m_datum == MeanSeaLevel || this->m_datum == FlightLevel);
            this->m_datum = MeanSeaLevel;
        }

        /*
         * Parse value
         */
        void CAltitude::parseFromString(const QString &value)
        {
            this->parseFromString(value, BlackMisc::PhysicalQuantities::CPqString::SeparatorsCLocale);
        }

        /*
         * Parse value
         */
        void CAltitude::parseFromString(const QString &value, BlackMisc::PhysicalQuantities::CPqString::SeparatorMode mode)
        {
            QString v = value.trimmed();

            // special case FL
            if (v.contains("FL", Qt::CaseInsensitive))
            {
                v = v.replace("FL", "", Qt::CaseInsensitive).trimmed();
                bool ok = false;
                double dv = v.toDouble(&ok) * 100.0;
                CAltitude a(ok ? dv : 0.0, FlightLevel,
                            ok ? CLengthUnit::ft() : CLengthUnit::nullUnit());
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

            CLength l = BlackMisc::PhysicalQuantities::CPqString::parse<CLength>(v, mode);
            *this = CAltitude(l, rd);
        }

        /*
         * Icon
         */
        CIcon CAltitude::toIcon() const
        {
            return BlackMisc::CIconList::iconByIndex(CIcons::GeoPosition);
        }

    } // namespace
} // namespace
