/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_FSCOMMON_AIRCRAFTCFGENTRY_H
#define BLACKMISC_SIMULATION_FSCOMMON_AIRCRAFTCFGENTRY_H

#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {
            /*!
             * Set of aircraft.cfg entries representing an aircraft (FSX)
             * \remarks an entry in the aircraft.cfg is title, atc type, ... This class already bundles
             *          relevant entries, hence the class is named Entries (plural)
             */
            class CAircraftCfgEntries: public BlackMisc::CValueObject<CAircraftCfgEntries>
            {
            public:
                //! Properties by index
                enum ColumnIndex
                {
                    IndexEntryIndex = BlackMisc::CPropertyIndex::GlobalIndexCAircraftCfgEntries,
                    IndexFileName,
                    IndexTitle,
                    IndexAtcType,
                    IndexAtcModel,
                    IndexParkingCode,
                    IndexDescription,
                    IndexTexture
                };

                //! Default constructor
                CAircraftCfgEntries() = default;

                //! Entries representing an aircraft
                CAircraftCfgEntries(const QString &filePath, int index, const QString &title, const QString &atcType, const QString &atcModel, const QString &atcParkingCode, const QString &description);

                //! File name
                QString getFileName() const { return this->m_fileName; }

                //! Directory of entry
                QString getFileDirectory() const;

                //! Title
                QString getTitle() const { return this->m_title; }

                //! Index
                int getIndex() const { return this->m_index; }

                //! ATC model
                QString getAtcModel() const { return this->m_atcModel; }

                //! ATC type
                QString getAtcType() const { return this->m_atcType; }

                //! Description
                QString getDescription() const { return this->m_description; }

                //! ATC parking code
                QString getAtcParkingCode() const { return this->m_atcParkingCode; }

                //! UI type (e.g. A321-231 IAE)
                QString getUiType() const { return this->m_uiType; }

                //! UI manufacturer (e.g. Airbus)
                QString getUiManufacturer() const { return this->m_uiManufacturer; }

                //! Texture
                QString getTexture() const { return this->m_texture; }

                //! Manufacturer + type
                QString getUiCombinedDescription() const;

                //! Filepath
                void setFileName(const QString &filePath) { this->m_fileName = filePath; }

                //! Title
                void setTitle(const QString &title) { this->m_title = title; }

                //! Index
                void setIndex(int index) { this->m_index = index; }

                //! ATC model
                void setAtcModel(const QString &atcModel) { this->m_atcModel = atcModel.trimmed(); }

                //! ATC type
                void setAtcType(const QString &atcType) { this->m_atcType = atcType.trimmed(); }

                //! Parking code
                void setAtcParkingCode(const QString &parkingCode) { this->m_atcParkingCode = parkingCode.trimmed(); }

                //! Description
                void setDescription(const QString &description) { this->m_description = description.trimmed(); }

                //! Texture
                void setTexture(const QString &texture) { this->m_texture = texture; }

                //! UI type (e.g. A321-231 IAE)
                void setUiType(const QString &type) { this->m_uiType = type.trimmed(); }

                //! UI manufacturer (e.g. Airbus)
                void setUiManufacturer(const QString &manufacturer) { this->m_uiManufacturer = manufacturer.trimmed(); }

                //! To aircraft model
                BlackMisc::Simulation::CAircraftModel toAircraftModel() const;

                //! Thumbnail.jpg path if possible
                QString getThumbnailFileName() const;

                //! \copydoc CValueObject::propertyByIndex
                virtual BlackMisc::CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

                //! \copydoc CValueObject::setPropertyByIndex
                virtual void setPropertyByIndex(const BlackMisc::CVariant &variant, const BlackMisc::CPropertyIndex &index) override;

            protected:
                //! \copydoc CValueObject::convertToQString
                virtual QString convertToQString(bool i18n = false) const override;

            private:
                BLACK_ENABLE_TUPLE_CONVERSION(CAircraftCfgEntries)
                int m_index;           //!< current index in given config
                QString m_fileName;    //!< file name of aircraft.cfg
                QString m_title;       //!< Title in aircraft.cfg
                QString m_atcType;     //!< ATC type
                QString m_atcModel;    //!< ATC model
                QString m_atcParkingCode; //!< ATC parking code
                QString m_description; //!< descriptive text
                QString m_uiType;      //!< e.g. A321-231 IAE
                QString m_uiManufacturer; //!< e.g. Airbus
                QString m_texture;     //!< texture, needed to identify thumbnail.jpg
            };
        }
    }
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Simulation::FsCommon::CAircraftCfgEntries, (o.m_index, o.m_fileName, o.m_title, o.m_atcType, o.m_atcModel, o.m_atcParkingCode))
Q_DECLARE_METATYPE(BlackMisc::Simulation::FsCommon::CAircraftCfgEntries)

#endif // guard
