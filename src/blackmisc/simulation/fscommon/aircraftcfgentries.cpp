/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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
                return QStringLiteral("{%1, %2, %3, %4, %5, %6}").arg(m_fileName).arg(m_index).
                       arg(m_title, m_atcModel, m_atcType, m_atcParkingCode);
            }

            CAircraftCfgEntries::CAircraftCfgEntries(const QString &fileName, int index) :
                m_index(index), m_fileName(fileName)
            { }

            QString CAircraftCfgEntries::getFileDirectory() const
            {
                if (m_fileName.isEmpty()) { return {}; }
                const QFileInfo fileInfo(m_fileName);
                return fileInfo.absolutePath();
            }

            QString CAircraftCfgEntries::getUiCombinedDescription() const
            {
                QString d(m_uiManufacturer);
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
                m_fileName = filePath.trimmed();
            }

            void CAircraftCfgEntries::setTitle(const QString &title)
            {
                m_title = title.trimmed();
            }

            void CAircraftCfgEntries::setAtcModel(const QString &atcModel)
            {
                m_atcModel = atcModel.trimmed();
            }

            void CAircraftCfgEntries::setAtcType(const QString &atcType)
            {
                m_atcType = atcType.trimmed();
            }

            void CAircraftCfgEntries::setAtcParkingCode(const QString &parkingCode)
            {
                m_atcParkingCode = parkingCode.trimmed();
            }

            void CAircraftCfgEntries::setAtcAirline(const QString &airline)
            {
                m_atcAirline = airline.trimmed();
            }

            void CAircraftCfgEntries::setSimName(const QString &simName)
            {
                m_simName = simName.trimmed();
            }

            void CAircraftCfgEntries::setDescription(const QString &description)
            {
                m_description = description.trimmed();
            }

            void CAircraftCfgEntries::setCreatedBy(const QString &createdBy)
            {
                m_createdBy = createdBy.trimmed();
            }

            void CAircraftCfgEntries::setTexture(const QString &texture)
            {
                m_texture = texture.trimmed();
            }

            void CAircraftCfgEntries::setUiType(const QString &type)
            {
                m_uiType = type.trimmed();
            }

            CAircraftModel CAircraftCfgEntries::toAircraftModel() const
            {
                // creates raw, unconsolidated data
                Q_ASSERT_X(m_timestampMSecsSinceEpoch >= 0, Q_FUNC_INFO, "Missing file timestamp");
                CAircraftModel model(this->getTitle(), CAircraftModel::TypeOwnSimulatorModel);
                model.setDescription(this->getUiCombinedDescription()); // Manufacturer, variation, type
                model.setFileName(this->getFileName());
                model.setName(this->getSimName());
                model.setMSecsSinceEpoch(m_timestampMSecsSinceEpoch); // aircraft.cfg file timestamp
                model.setFileTimestamp(m_timestampMSecsSinceEpoch);
                model.setIconFile(this->getThumbnailFileNameChecked());

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
                if (m_texture.isEmpty()) { return {}; }
                if (m_fileName.isEmpty()) { return {}; }
                QString fn = QDir::cleanPath(this->getFileDirectory() + QDir::separator() + "texture." + m_texture + QDir::separator() + "thumbnail.jpg");
                return fn;
            }

            QString CAircraftCfgEntries::getThumbnailFileNameChecked() const
            {
                const QString f(getThumbnailFileNameGuess());
                if (f.isEmpty()) { return {}; }
                if (QFile(f).exists()) { return f; }
                return {};
            }

            CVariant CAircraftCfgEntries::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
            {
                if (index.isMyself()) { return CVariant::from(*this); }
                if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }
                ColumnIndex i = index.frontCasted<ColumnIndex>();
                switch (i)
                {
                case IndexEntryIndex: return CVariant::from(m_index);
                case IndexFileName: return CVariant::from(m_fileName);
                case IndexTitle: return CVariant::from(m_title);
                case IndexAirline: return CVariant::from(m_atcAirline);
                case IndexAtcType: return CVariant::from(m_atcType);
                case IndexAtcModel: return CVariant::from(m_atcModel);
                case IndexAtcIdColor: return CVariant::from(m_atcIdColor);
                case IndexParkingCode: return CVariant::from(m_atcParkingCode);
                case IndexDescription: return CVariant::from(m_description);
                case IndexUiType: return CVariant::from(m_uiType);
                case IndexUiManufacturer: return CVariant::from(m_uiManufacturer);
                case IndexUiVariation: return CVariant::from(m_uiVariation);
                case IndexTexture: return CVariant::from(m_texture);
                case IndexSimulatorName: return CVariant::from(m_simName);
                case IndexCreatedBy: return CVariant::from(m_createdBy);
                case IndexRotorcraft: return CVariant::from(m_rotorcraft);
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
                case IndexAirline: this->setTitle(m_atcAirline); break;
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
