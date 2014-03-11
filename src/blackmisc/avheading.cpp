/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmiscfreefunctions.h"
#include "avheading.h"

using BlackMisc::PhysicalQuantities::CAngle;
using BlackMisc::PhysicalQuantities::CAngleUnit;

namespace BlackMisc
{
    namespace Aviation
    {

        /*
         * Own implementation for streaming
         */
        QString CHeading::convertToQString(bool i18n) const
        {
            QString s = CAngle::convertToQString(i18n).append(" ");
            if (i18n)
            {
                return s.append(this->isMagneticHeading() ?
                                QCoreApplication::translate("Aviation", "magnetic") :
                                QCoreApplication::translate("Aviation", "true"));
            }
            else
            {
                return s.append(this->isMagneticHeading() ? "magnetic" : "true");
            }
        }

        /*
         * Marshall
         */
        void CHeading::marshallToDbus(QDBusArgument &argument) const
        {
            CAngle::marshallToDbus(argument);
            argument << TupleConverter<CHeading>::toTuple(*this);
        }

        /*
         * Unmarshall
         */
        void CHeading::unmarshallFromDbus(const QDBusArgument &argument)
        {
            CAngle::unmarshallFromDbus(argument);
            argument >> TupleConverter<CHeading>::toTuple(*this);
        }

        /*
         * Hash
         */
        uint CHeading::getValueHash() const
        {
            QList<uint> hashs;
            hashs << CAngle::getValueHash();
            hashs << qHash(TupleConverter<CHeading>::toTuple(*this));
            return BlackMisc::calculateHash(hashs, "CHeading");
        }

        /*
         * Compare
         */
        int CHeading::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CHeading &>(otherBase);
            int result = compare(TupleConverter<CHeading>::toTuple(*this), TupleConverter<CHeading>::toTuple(other));
            return result == 0 ? CAngle::compareImpl(otherBase) : result;
        }

        bool CHeading::operator ==(const CHeading &other) const
        {
            if (this == &other) return true;
            if (!CAngle::operator ==(other)) return false;
            return TupleConverter<CHeading>::toTuple(*this) == TupleConverter<CHeading>::toTuple(other);
        }

        /*
         * Unequal?
         */
        bool CHeading::operator !=(const CHeading &other) const
        {
            return !((*this) == other);
        }

        /*!
         * \brief Register metadata of unit and quantity
         */
        void CHeading::registerMetadata()
        {
            qRegisterMetaType<CHeading>();
            qDBusRegisterMetaType<CHeading>();
        }

    } // namespace
} // namespace
