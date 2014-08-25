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
        CAltitude::CAltitude(const QString &altitudeAsString, BlackMisc::PhysicalQuantities::CPqString::SeparatorMode mode) : BlackMisc::PhysicalQuantities::CLength(0, BlackMisc::PhysicalQuantities::CLengthUnit::m()), m_datum(MeanSeaLevel)
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
         * Marshall to DBus
         */
        void CAltitude::marshallToDbus(QDBusArgument &argument) const
        {
            this->CLength::marshallToDbus(argument);
            argument << qint32(this->m_datum);
        }

        /*
         * Unmarshall from DBus
         */
        void CAltitude::unmarshallFromDbus(const QDBusArgument &argument)
        {
            this->CLength::unmarshallFromDbus(argument);
            qint32 datum;
            argument >> datum;
            this->m_datum = static_cast<ReferenceDatum>(datum);
        }

        /*
         * Equal?
         */
        bool CAltitude::operator ==(const CAltitude &other) const
        {
            return other.m_datum == this->m_datum && this->CLength::operator ==(other);
        }

        /*
         * Unequal?
         */
        bool CAltitude::operator !=(const CAltitude &other) const
        {
            return !((*this) == other);
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
         * metaTypeId
         */
        int CAltitude::getMetaTypeId() const
        {
            return qMetaTypeId<CAltitude>();
        }

        /*
         * is a
         */
        bool CAltitude::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CAltitude>()) { return true; }

            return this->CLength::isA(metaTypeId);
        }

        /*
         * Compare
         */
        int CAltitude::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CAltitude &>(otherBase);

            if (this->isMeanSeaLevel() && other.isAboveGroundLevel()) { return 1; }
            if (this->isAboveGroundLevel() && other.isMeanSeaLevel()) { return -1; }
            if (*this < other) { return -1; }
            if (*this > other) { return 1; }
            return 0;
        }

        /*
         * Register metadata
         */
        void CAltitude::registerMetadata()
        {
            qRegisterMetaType<CAltitude>();
            qDBusRegisterMetaType<CAltitude>();
        }

        /*
         * To JSON
         */
        QJsonObject CAltitude::toJson() const
        {
            QJsonObject json = BlackMisc::serializeJson(CAltitude::jsonMembers(), TupleConverter<CAltitude>::toTuple(*this));
            return BlackMisc::Json::appendJsonObject(json, CLength::toJson());
        }

        /*
         * From JSON
         */
        void CAltitude::convertFromJson(const QJsonObject &json)
        {
            CLength::convertFromJson(json);
            BlackMisc::deserializeJson(json, CAltitude::jsonMembers(), TupleConverter<CAltitude>::toTuple(*this));
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
            return BlackMisc::CIconList::iconForIndex(CIcons::GeoPosition);
        }

        /*
         * Members
         */
        const QStringList &CAltitude::jsonMembers()
        {
            return TupleConverter<CAltitude>::jsonMembers();
        }

    } // namespace
} // namespace
