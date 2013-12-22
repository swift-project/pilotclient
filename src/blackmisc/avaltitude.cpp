/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avaltitude.h"

using BlackMisc::PhysicalQuantities::CLength;
using BlackMisc::PhysicalQuantities::CLengthUnit;

namespace BlackMisc
{
    namespace Aviation
    {

        /*
         * Own implementation for streaming
         */
        QString CAltitude::convertToQString(bool /* i18n */) const
        {
            QString s = this->CLength::convertToQString();
            return s.append(this->isMeanSeaLevel() ? " MSL" : " AGL");
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
         * Compare
         */
        int CAltitude::compare(const QVariant &qv) const
        {
            Q_ASSERT(qv.canConvert<CAltitude>() || qv.canConvert<CLength>());
            Q_ASSERT(qv.isValid() && !qv.isNull());
            if (qv.canConvert<CAltitude>())
            {
                CAltitude other = qv.value<CAltitude>();
                if (this->isMeanSeaLevel() && other.isAboveGroundLevel()) return 1;
                if (this->isAboveGroundLevel() && other.isMeanSeaLevel()) return -1;
                return this->compare(other);
            }
            else if (qv.canConvert<CLength>())
            {
                return this->compare(qv.value<CLength>());
            }
            qFatal("Invalid comparison");
            return 0; // just for compiler
        }

        /*
         * Register metadata
         */
        void CAltitude::registerMetadata()
        {
            qRegisterMetaType<CAltitude>();
            qDBusRegisterMetaType<CAltitude>();
        }

    } // namespace
} // namespace
