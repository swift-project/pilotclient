/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/fscommon/aircraftcfgentries.h"
#include "blackmisc/variant.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

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
                s = s.
                    arg(this->m_fileName).arg(this->m_index).
                    arg(this->m_title, this->m_atcModel, this->m_atcType, this->m_atcParkingCode);
                return s;
            }

            CAircraftCfgEntries::CAircraftCfgEntries(const QString &fileName, int index) :
                m_index(index), m_fileName(fileName)
            { }

            QString CAircraftCfgEntries::getFileDirectory() const
            {
                if (this->m_fileName.isEmpty()) { return ""; }
                const QFileInfo fileInfo(this->m_fileName);
                return fileInfo.absolutePath();
            }

            QString CAircraftCfgEntries::getUiCombinedDescription() const
            {
                QString d(this->m_uiManufacturer);
                if (!this->getUiType().isEmpty())
                {
                    d += " ";
                    d += this->getUiType();
                }
                if (!this->getUiVariation().isEmpty())
                {
                    d += " ";
                    d += this->getUiVariation();
                }
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
                // creates raw, unconsolidated data
                CAircraftModel model(this->getTitle(), CAircraftModel::TypeOwnSimulatorModel);
                model.setDescription(this->getUiCombinedDescription()); // Manufacturer, variation, type
                model.setFileName(this->getFileName());
                model.setName(this->getSimName());
                Q_ASSERT_X(this->m_timestampMSecsSinceEpoch >= 0, Q_FUNC_INFO, "Missing file timestamp");
                model.setMSecsSinceEpoch(this->m_timestampMSecsSinceEpoch); // aircraft.cfg file timestamp
                model.setFileTimestamp(this->m_timestampMSecsSinceEpoch);
                model.setIconPath(this->getThumbnailFileNameChecked());

                const QString designator(CAircraftIcaoCode::normalizeDesignator(this->getAtcModel()));
                CAircraftIcaoCode aircraft(
                    CAircraftIcaoCode::isValidDesignator(designator) ?
                    designator :
                    CAircraftIcaoCode::getUnassignedDesignator());
                aircraft.setManufacturer(this->getUiManufacturer());
                model.setAircraftIcaoCode(aircraft);

                // livery
                CLivery livery;
                livery.setCombinedCode(this->getTexture());

                CAirlineIcaoCode airline;
                airline.setName(this->getAtcAirline()); // descriptive name like "BATAVIA"
                const QString airlineDesignator(this->getAtcParkingCode());
                if (CAirlineIcaoCode::isValidAirlineDesignator(airlineDesignator))
                {
                    airline.setDesignator(airlineDesignator);
                }
                livery.setAirlineIcaoCode(airline);
                model.setLivery(livery);

                // distributor
                const CDistributor distributor(this->getCreatedBy());
                model.setDistributor(distributor);
                return model;
            }

            QString CAircraftCfgEntries::getThumbnailFileNameGuess() const
            {
                if (this->m_texture.isEmpty()) { return ""; }
                if (this->m_fileName.isEmpty()) { return ""; }
                QString fn = QDir::cleanPath(this->getFileDirectory() + QDir::separator() + "texture." + this->m_texture + QDir::separator() + "thumbnail.jpg");
                return fn;
            }

            QString CAircraftCfgEntries::getThumbnailFileNameChecked() const
            {
                const QString f(getThumbnailFileNameGuess());
                if (f.isEmpty()) { return ""; }
                if (QFile(f).exists()) { return f; }
                return "";
            }

            CVariant CAircraftCfgEntries::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
            {
                if (index.isMyself()) { return CVariant::from(*this); }
                if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }
                ColumnIndex i = index.frontCasted<ColumnIndex>();
                switch (i)
                {
                case IndexEntryIndex: return CVariant::from(this->m_index);
                case IndexFileName: return CVariant::from(this->m_fileName);
                case IndexTitle: return CVariant::from(this->m_title);
                case IndexAirline: return CVariant::from(this->m_atcAirline);
                case IndexAtcType: return CVariant::from(this->m_atcType);
                case IndexAtcModel: return CVariant::from(this->m_atcModel);
                case IndexAtcIdColor: return CVariant::from(this->m_atcIdColor);
                case IndexParkingCode: return CVariant::from(this->m_atcParkingCode);
                case IndexDescription: return CVariant::from(this->m_description);
                case IndexUiType: return CVariant::from(this->m_uiType);
                case IndexUiManufacturer: return CVariant::from(this->m_uiManufacturer);
                case IndexUiVariation: return CVariant::from(this->m_uiVariation);
                case IndexTexture: return CVariant::from(this->m_texture);
                case IndexSimulatorName: return CVariant::from(this->m_simName);
                case IndexCreatedBy: return CVariant::from(this->m_createdBy);
                case IndexRotorcraft: return CVariant::from(this->m_rotorcraft);
                default: return CValueObject::propertyByIndex(index);
                }
            }

            void CAircraftCfgEntries::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
            {
                if (index.isMyself()) { (*this) = variant.to<CAircraftCfgEntries>(); return; }
                if (ITimestampBased::canHandleIndex(index)) { ITimestampBased::setPropertyByIndex(index, variant); return; }
                ColumnIndex i = index.frontCasted<ColumnIndex>();
                switch (i)
                {
                case IndexEntryIndex: this->setIndex(variant.toInt()); break;
                case IndexFileName: this->setFileName(variant.toQString()); break;
                case IndexTitle: this->setTitle(variant.toQString()); break;
                case IndexAirline: this->setTitle(this->m_atcAirline); break;
                case IndexAtcType: this->setAtcType(variant.toQString()); break;
                case IndexAtcModel: this->setAtcModel(variant.toQString()); break;
                case IndexAtcIdColor: this->setAtcIdColor(variant.toQString()); break;
                case IndexParkingCode: this->setAtcParkingCode(variant.toQString()); break;
                case IndexDescription: this->setDescription(variant.toQString()); break;
                case IndexUiType: this->setUiType(variant.toQString()); break;
                case IndexUiVariation: this->setUiVariation(variant.toQString()); break;
                case IndexUiManufacturer: this->setUiManufacturer(variant.toQString()); break;
                case IndexTexture: this->setTexture(variant.toQString()); break;
                case IndexSimulatorName: this->setSimName(variant.toQString()); break;
                case IndexCreatedBy: this->setCreatedBy(variant.toQString()); break;
                case IndexRotorcraft: this->setRotorcraft(variant.toBool()); break;
                default:
                    CValueObject::setPropertyByIndex(index, variant);
                    break;
                }
            }
        } // namespace
    } // namespace
} // namespace
