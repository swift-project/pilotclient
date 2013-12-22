/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avtrack.h"

using BlackMisc::PhysicalQuantities::CAngle;
using BlackMisc::PhysicalQuantities::CAngleUnit;

namespace BlackMisc
{
    namespace Aviation
    {
        /*
         * Own implementation for streaming
         */
        QString CTrack::convertToQString(bool i18n) const
        {
            QString s = CAngle::convertToQString(i18n).append(" ");
            if (i18n)
            {
                return s.append(this->isMagneticTrack() ?
                                QCoreApplication::translate("Aviation", "magnetic") :
                                QCoreApplication::translate("Aviation", "true"));
            }
            else
            {
                return s.append(this->isMagneticTrack() ? "magnetic" : "true");
            }
        }

        /*
         * Marshall to DBus
         */
        void CTrack::marshallToDbus(QDBusArgument &argument) const
        {
            this->CAngle::marshallToDbus(argument);
            argument << qint32(this->m_north);
        }

        /*
         * Unmarshall from DBus
         */
        void CTrack::unmarshallFromDbus(const QDBusArgument &argument)
        {
            this->CAngle::unmarshallFromDbus(argument);
            qint32 north;
            argument >> north;
            this->m_north = static_cast<ReferenceNorth>(north);
        }

        /*
         * Equal?
         */
        bool CTrack::operator ==(const CTrack &other) const
        {
            return other.m_north == this->m_north && this->CAngle::operator ==(other);
        }

        /*
         * Unequal?
         */
        bool CTrack::operator !=(const CTrack &other) const
        {
            return !((*this) == other);
        }

        /*!
         * \brief Register metadata of unit and quantity
         */
        void CTrack::registerMetadata()
        {
            qRegisterMetaType<CTrack>();
            qDBusRegisterMetaType<CTrack>();
        }

    } // namespace
} // namespace
