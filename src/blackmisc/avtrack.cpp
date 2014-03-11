/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmiscfreefunctions.h"
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
         * Marshall
         */
        void CTrack::marshallToDbus(QDBusArgument &argument) const
        {
            CAngle::marshallToDbus(argument);
            argument << TupleConverter<CTrack>::toTuple(*this);
        }

        /*
         * Unmarshall
         */
        void CTrack::unmarshallFromDbus(const QDBusArgument &argument)
        {
            CAngle::unmarshallFromDbus(argument);
            argument >> TupleConverter<CTrack>::toTuple(*this);
        }

        /*
         * Hash
         */
        uint CTrack::getValueHash() const
        {
            QList<uint> hashs;
            hashs << CAngle::getValueHash();
            hashs << qHash(TupleConverter<CTrack>::toTuple(*this));
            return BlackMisc::calculateHash(hashs, "CTrack");
        }

        /*
         * Compare
         */
        int CTrack::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CTrack &>(otherBase);
            int result = compare(TupleConverter<CTrack>::toTuple(*this), TupleConverter<CTrack>::toTuple(other));
            return result == 0 ? CAngle::compareImpl(otherBase) : result;
        }

        bool CTrack::operator ==(const CTrack &other) const
        {
            if (this == &other) return true;
            if (!CAngle::operator ==(other)) return false;
            return TupleConverter<CTrack>::toTuple(*this) == TupleConverter<CTrack>::toTuple(other);
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
