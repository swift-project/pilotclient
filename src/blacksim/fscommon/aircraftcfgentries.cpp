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
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;

namespace BlackSim
{
    namespace FsCommon
    {

        /*
         * Constructor
         */
        CAircraftCfgEntries::CAircraftCfgEntries(const QString &filePath, qint32 index, const QString &title, const QString &atcType, const QString &atcModel, const QString &atcParkingCode, const QString &description) :
            m_index(index), m_fileName(filePath), m_title(title.trimmed()), m_atcType(atcType.trimmed()),
            m_atcModel(atcModel.trimmed()), m_atcParkingCode(atcParkingCode.trimmed()), m_description(description.trimmed())
        {
            // void
        }

        /*
         * String representation
         */
        QString CAircraftCfgEntries::convertToQString(bool) const
        {
            QString s = "{%1, %2, %3, %4, %5, %6}";
            s = s.arg(this->m_fileName).arg(this->m_index).arg(this->m_title)
                .arg(this->m_atcModel).arg(this->m_atcType).arg(this->m_atcParkingCode);
            return s;
        }

        /*
         * Aircraft model
         */
        QString CAircraftCfgEntries::getFileDirectory() const
        {
            if (this->m_fileName.isEmpty()) { return ""; }
            QFileInfo fi(this->m_fileName);
            return fi.absolutePath();
        }

        QString CAircraftCfgEntries::getUiCombinedDescription() const
        {
            QString d(this->m_uiManufacturer);
            if (m_uiType.isEmpty()) { return d; }
            if (d.isEmpty()) { return m_uiType; }
            d += " ";
            d += m_uiType;
            return d;
        }

        /*
         * Convert
         */
        CAircraftModel CAircraftCfgEntries::toAircraftModel() const
        {
            CAircraftModel model(this->getTitle(), CAircraftModel::TypeModelMapping);
            model.setDescription(this->getUiCombinedDescription());
            model.setFileName(this->getFileName());
            return model;
        }

        QString CAircraftCfgEntries::getThumbnailFileName() const
        {
            if (this->m_texture.isEmpty()) { return ""; }
            if (this->m_fileName.isEmpty()) { return ""; }
            QString fn = QDir::cleanPath(this->getFileDirectory() + QDir::separator() + "texture." + this->m_texture + QDir::separator() + "thumbnail.jpg");
            return fn;
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
            case IndexFileName:
                return CVariant::from(this->m_fileName);
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
            case IndexTexture:
                return CVariant::from(this->m_texture);
            case IndexDescription:
                return CVariant::from(this->m_description);
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
            case IndexFileName:
                this->setFileName(variant.toQString());
                break;
            case IndexParkingCode:
                this->setAtcParkingCode(variant.toQString());
                break;
            case IndexTitle:
                this->setTitle(variant.toQString());
                break;
            case IndexDescription:
                this->setDescription(variant.toQString());
                break;
            case IndexTexture:
                this->setTexture(variant.toQString());
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

    } // namespace
} // namespace
