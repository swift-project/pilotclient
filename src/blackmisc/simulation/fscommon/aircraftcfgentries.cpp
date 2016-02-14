/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftcfgentries.h"
#include "blackmisc/variant.h"
#include <QDir>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {

            CAircraftCfgEntries::CAircraftCfgEntries(const QString &fileName, int index, const QString &title, const QString &atcType, const QString &atcModel, const QString &atcParkingCode, const QString &description) :
                m_index(index), m_fileName(fileName), m_title(title.trimmed()), m_atcType(atcType.trimmed()),
                m_atcModel(atcModel.trimmed()), m_atcParkingCode(atcParkingCode.trimmed()), m_description(description.trimmed())
            {
                // void
            }

            QString CAircraftCfgEntries::convertToQString(bool) const
            {
                QString s = "{%1, %2, %3, %4, %5, %6}";
                s = s.arg(this->m_fileName).arg(this->m_index).arg(this->m_title)
                    .arg(this->m_atcModel).arg(this->m_atcType).arg(this->m_atcParkingCode);
                return s;
            }

            CAircraftCfgEntries::CAircraftCfgEntries(const QString &fileName, int index) :
                m_index(index), m_fileName(fileName)
            { }

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

            void CAircraftCfgEntries::setFileName(const QString &filePath)
            {
                this->m_fileName = filePath.trimmed();
            }

            void CAircraftCfgEntries::setTitle(const QString &title)
            {
                this->m_title = title.trimmed();
            }

            void CAircraftCfgEntries::setAtcModel(const QString &atcModel)
            {
                this->m_atcModel = atcModel.trimmed();
            }

            void CAircraftCfgEntries::setAtcType(const QString &atcType)
            {
                this->m_atcType = atcType.trimmed();
            }

            void CAircraftCfgEntries::setAtcParkingCode(const QString &parkingCode)
            {
                this->m_atcParkingCode = parkingCode.trimmed();
            }

            void CAircraftCfgEntries::setAtcAirline(const QString &airline)
            {
                this->m_atcAirline = airline.trimmed();
            }

            void CAircraftCfgEntries::setSimName(const QString &simName)
            {
                this->m_simName = simName.trimmed();
            }

            void CAircraftCfgEntries::setDescription(const QString &description)
            {
                this->m_description = description.trimmed();
            }

            void CAircraftCfgEntries::setCreatedBy(const QString &createdBy)
            {
                this->m_createdBy = createdBy.trimmed();
            }

            void CAircraftCfgEntries::setTexture(const QString &texture)
            {
                this->m_texture = texture.trimmed();
            }

            void CAircraftCfgEntries::setUiType(const QString &type)
            {
                this->m_uiType = type.trimmed();
            }

            CAircraftModel CAircraftCfgEntries::toAircraftModel() const
            {
                CAircraftModel model(this->getTitle(), CAircraftModel::TypeOwnSimulatorModel);
                model.setDescription(this->getUiCombinedDescription()); // Manufacturer and type
                model.setFileName(this->getFileName());
                model.setName(this->getSimName());

                const QString designator(CAircraftIcaoCode::normalizeDesignator(getAtcModel()));
                CAircraftIcaoCode aircraft(
                    CAircraftIcaoCode::isValidDesignator(designator) ?
                    designator :
                    CAircraftIcaoCode::getUnassignedDesignator());
                aircraft.setManufacturer(this->getUiManufacturer());
                model.setAircraftIcaoCode(aircraft);

                CLivery livery;
                livery.setCombinedCode(this->getTexture());
                CAirlineIcaoCode airline;
                airline.setName(this->getAtcAirline());
                livery.setAirlineIcaoCode(airline);
                model.setLivery(livery);

                CDistributor distributor(this->getCreatedBy());
                model.setDistributor(distributor);

                return model;
            }

            QString CAircraftCfgEntries::getThumbnailFileName() const
            {
                if (this->m_texture.isEmpty()) { return ""; }
                if (this->m_fileName.isEmpty()) { return ""; }
                QString fn = QDir::cleanPath(this->getFileDirectory() + QDir::separator() + "texture." + this->m_texture + QDir::separator() + "thumbnail.jpg");
                return fn;
            }

            CVariant CAircraftCfgEntries::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
            {
                if (index.isMyself()) { return CVariant::from(*this); }
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

            void CAircraftCfgEntries::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
            {
                if (index.isMyself()) { (*this) = variant.to<CAircraftCfgEntries>(); return; }
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
} // namespace
