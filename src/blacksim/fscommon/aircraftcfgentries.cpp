/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftcfgentries.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/variant.h"

using namespace BlackMisc;

namespace BlackSim
{
    namespace FsCommon
    {

        /*
         * Constructor
         */
        CAircraftCfgEntries::CAircraftCfgEntries(const QString &filePath, qint32 index, const QString &title, const QString &atcType, const QString &atcModel, const QString &atcParkingCode) :
            m_index(index), m_filePath(filePath), m_title(title), m_atcType(atcType),
            m_atcModel(atcModel), m_atcParkingCode(atcParkingCode)
        {
            // void
        }

        /*
         * String representation
         */
        QString CAircraftCfgEntries::convertToQString(bool) const
        {
            QString s = "{%1, %2, %3, %4, %5, %6}";
            s = s.arg(this->m_filePath).arg(this->m_index).arg(this->m_title)
                .arg(this->m_atcModel).arg(this->m_atcType).arg(this->m_atcParkingCode);
            return s;
        }

        /*
         * Get particular column
         */
        CVariant CAircraftCfgEntries::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toCVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexFilePath:
                return CVariant::from(this->m_filePath);
            case IndexTitle:
                return CVariant::from(this->m_title);
            case IndexAtcType:
                return CVariant::from(this->m_atcType);
            case IndexAtcModel:
                return CVariant::from(this->m_atcModel);
            case IndexParkingCode:
                return CVariant::from(this->m_atcParkingCode);
            case IndexEntryIndex:
                return CVariant::from(this->m_index);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        /*
         * Set property as index
         */
        void CAircraftCfgEntries::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself()) { this->convertFromCVariant(variant); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAtcModel:
                this->setAtcModel(variant.toQString());
                break;
            case IndexAtcType:
                this->setAtcType(variant.toQString());
                break;
            case IndexEntryIndex:
                this->setIndex(variant.toInt());
                break;
            case IndexFilePath:
                this->setFilePath(variant.toQString());
                break;
            case IndexParkingCode:
                this->setAtcParkingCode(variant.toQString());
                break;
            case IndexTitle:
                this->setTitle(variant.toQString());
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

    } // namespace
} // namespace
