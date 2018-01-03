/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/iconlist.h"
#include "blackmisc/icons.h"
#include "blackmisc/pq/measurementunit.h"
#include "blackmisc/pq/pqstring.h"

#include <Qt>
#include <QtGlobal>

using BlackMisc::PhysicalQuantities::CLength;
using BlackMisc::PhysicalQuantities::CLengthUnit;

namespace BlackMisc
{
    namespace Aviation
    {
        CAltitude::CAltitude(const QString &altitudeAsString, PhysicalQuantities::CPqString::SeparatorMode mode) : CLength(0, BlackMisc::PhysicalQuantities::CLengthUnit::m()), m_datum(MeanSeaLevel)
        {
            this->parseFromString(altitudeAsString, mode);
        }

        QString CAltitude::convertToQString(bool i18n) const
        {
            if (this->m_datum == FlightLevel)
            {
                int fl = qRound(this->CLength::value(CLengthUnit::ft()) / 100.0);
                return QString("FL%1").arg(fl);
            }
            else
            {
                QString s;
                if (this->getUnit() == CLengthUnit::m())
                {
                    s = this->CLength::valueRoundedWithUnit(1, i18n);
                }
                else
                {
                    s = this->CLength::valueRoundedWithUnit(CLengthUnit::ft(), 0, i18n);
                }
                return s.append(this->isMeanSeaLevel() ? " MSL" : " AGL");
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

        void CAltitude::parseFromString(const QString &value)
        {
            this->parseFromString(value, PhysicalQuantities::CPqString::SeparatorsCLocale);
        }

        void CAltitude::parseFromString(const QString &value, PhysicalQuantities::CPqString::SeparatorMode mode)
        {
            QString v = value.trimmed();

            // special case FL
            if (v.contains("FL", Qt::CaseInsensitive))
            {
                v = v.replace("FL", "", Qt::CaseInsensitive).trimmed();
                bool ok = false;
                double dv = v.toDouble(&ok) * 100.0;
                CAltitude a(ok ? dv : 0.0, FlightLevel,
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

            CLength l = BlackMisc::PhysicalQuantities::CPqString::parse<CLength>(v, mode);
            *this = CAltitude(l, rd);
        }

        CIcon CAltitude::toIcon() const
        {
            return BlackMisc::CIcon::iconByIndex(CIcons::GeoPosition);
        }

        const CAltitude &CAltitude::null()
        {
            static const CAltitude null(0, CAltitude::MeanSeaLevel, CLengthUnit::nullUnit());
            return null;
        }
    } // namespace
} // namespace
