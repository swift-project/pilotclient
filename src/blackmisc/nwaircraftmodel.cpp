/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "nwaircraftmodel.h"
#include <QString>

namespace BlackMisc
{
    namespace Network
    {
        /*
         * Convert to string
         */
        QString CAircraftModel::convertToQString(bool /** i18n **/) const
        {
            QString s = this->m_modelString;
            if (!s.isEmpty()) s.append(' ');
            s.append(this->m_queriedModelStringFlag ? "queried" : "mapped");
            return s;
        }

        /*
         * Property by index
         */
        QVariant CAircraftModel::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toQVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModelString:
                return QVariant(this->m_modelString);
                break;
            case IndexIsQueriedModelString:
                return QVariant(this->m_queriedModelStringFlag);
                break;
            default:
                return CValueObject::propertyByIndex(index);
                break;
            }
        }

        /*
         * Set property as index
         */
        void CAircraftModel::setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromQVariant(variant);
                return;
            }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModelString:
                this->m_modelString = variant.toString();
                break;
            case IndexIsQueriedModelString:
                this->m_queriedModelStringFlag = variant.toBool();
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

        /*
         * Matches string?
         */
        bool CAircraftModel::matchesModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            if (sensitivity == Qt::CaseSensitive)
                return modelString == this->m_modelString;
            else
                return this->m_modelString.indexOf(modelString) == 0;
        }

    } // namespace
} // namespace
