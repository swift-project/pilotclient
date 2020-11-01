/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_FSCOMMON_AIRCRAFTCFGENTRY_H
#define BLACKMISC_SIMULATION_FSCOMMON_AIRCRAFTCFGENTRY_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/timestampbased.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {
            //! Set of aircraft.cfg entries representing an aircraft (FSX)
            //! \remarks an entry in the aircraft.cfg is title, atc type, ...
            //!          This class already bundles relevant entries, hence the class is named Entries (plural)
            class BLACKMISC_EXPORT CAircraftCfgEntries:
                public CValueObject<CAircraftCfgEntries>,
                public ITimestampBased
            {
            public:
                //! Properties by index
                enum ColumnIndex
                {
                    IndexEntryIndex = CPropertyIndexRef::GlobalIndexCAircraftCfgEntries,
                    IndexFileName,
                    IndexTitle,
                    IndexAirline,
                    IndexAtcType,
                    IndexAtcModel,
                    IndexAtcIdColor,
                    IndexParkingCode,
                    IndexDescription,
                    IndexUiType,
                    IndexUiVariation,
                    IndexUiManufacturer,
                    IndexTexture,
                    IndexSimulatorName,
                    IndexCreatedBy,
                    IndexRotorcraft
                };

                //! Default constructor
                CAircraftCfgEntries() = default;

                //! Entries representing an aircraft
                CAircraftCfgEntries(const QString &fileName, int index);

                //! Entries representing an aircraft
                CAircraftCfgEntries(const QString &filePath, int index, const QString &title, const QString &atcType, const QString &atcModel, const QString &atcParkingCode, const QString &description);

                //! File name
                const QString &getFileName() const { return m_fileName; }

                //! Directory of entry
                QString getFileDirectory() const;

                //! Title
                const QString &getTitle() const { return m_title; }

                //! Index
                int getIndex() const { return m_index; }

                //! ATC model
                const QString &getAtcModel() const { return m_atcModel; }

                //! ATC type
                const QString &getAtcType() const { return m_atcType; }

                //! ATC airline
                const QString &getAtcAirline() const { return m_atcAirline; }

                //! ATC id color (e.g. 0xffffffff)
                const QString &getAtcIdColor() const { return m_atcIdColor; }

                //! Sim name
                const QString &getSimName() const { return m_simName; }

                //! Description
                const QString &getDescription() const { return m_description; }

                //! ATC parking code
                const QString &getAtcParkingCode() const { return m_atcParkingCode; }

                //! UI type (e.g. A321-231 IAE)
                const QString &getUiType() const { return m_uiType; }

                //! UI variation (e.g. White,Green)
                const QString &getUiVariation() const { return m_uiVariation; }

                //! UI manufacturer (e.g. Airbus)
                const QString &getUiManufacturer() const { return m_uiManufacturer; }

                //! Texture
                const QString &getTexture() const { return m_texture; }

                //! Created by
                const QString &getCreatedBy() const { return m_createdBy; }

                //! Is Rotorcraft?
                bool isRotorcraft() const { return m_rotorcraft; }

                //! Manufacturer + type
                QString getUiCombinedDescription() const;

                //! Filepath
                void setFileName(const QString &filePath);

                //! Title
                void setTitle(const QString &title);

                //! Index
                void setIndex(int index) { m_index = index; }

                //! ATC model
                void setAtcModel(const QString &atcModel);

                //! ATC type
                void setAtcType(const QString &atcType);

                //! Parking code
                void setAtcParkingCode(const QString &parkingCode);

                //! Airline
                void setAtcAirline(const QString &airline);

                //! ATC color (e.g. 0xffffffff)
                void setAtcIdColor(const QString &color) { m_atcIdColor = color.trimmed(); }

                //! Simulator name
                void setSimName(const QString &simName);

                //! Description
                void setDescription(const QString &description);

                //! Created by
                void setCreatedBy(const QString &createdBy);

                //! Texture
                void setTexture(const QString &texture);

                //! UI type (e.g. A321-231 IAE)
                void setUiType(const QString &type);

                //! UI variation (e.g. White,Green)
                void setUiVariation(const QString &variation) { m_uiVariation = variation.trimmed(); }

                //! UI manufacturer (e.g. Airbus)
                void setUiManufacturer(const QString &manufacturer) { m_uiManufacturer = manufacturer.trimmed(); }

                //! Is Rotorcraft?
                void setRotorcraft(bool isRotorcraft) { m_rotorcraft = isRotorcraft; }

                //! To aircraft model
                BlackMisc::Simulation::CAircraftModel toAircraftModel() const;

                //! Thumbnail.jpg path if possible
                QString getThumbnailFileNameGuess() const;

                //! Thumbnail.jpg path if possible, and checked if file exists
                //! \remark checks file existence, consider I/O load
                QString getThumbnailFileNameChecked() const;

                //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
                QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

                //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
                void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

                //! \copydoc BlackMisc::Mixin::String::toQString
                QString convertToQString(bool i18n = false) const;

            private:
                int m_index;               //!< current index in given config
                QString m_fileName;        //!< file name of .cfg
                QString m_title;           //!< Title in .cfg
                QString m_atcType;         //!< ATC type
                QString m_atcModel;        //!< ATC model
                QString m_atcAirline;      //!< ATC airline
                QString m_atcParkingCode;  //!< ATC parking code
                QString m_atcIdColor;      //!< e.g. 0xffffffff (not for all aircrafts)
                QString m_description;     //!< descriptive text
                QString m_uiType;          //!< e.g. A321-231 IAE
                QString m_uiManufacturer;  //!< e.g. Airbus
                QString m_uiVariation;     //!< e.g. White,Green
                QString m_texture;         //!< texture, needed to identify thumbnail.jpg
                QString m_simName;         //!< name in simulator
                QString m_createdBy;       //!< created by, "distributor"
                bool m_rotorcraft = false; //!< hint if rotorcraft

                BLACK_METACLASS(
                    CAircraftCfgEntries,
                    BLACK_METAMEMBER(index),
                    BLACK_METAMEMBER(fileName),
                    BLACK_METAMEMBER(title),
                    BLACK_METAMEMBER(atcType),
                    BLACK_METAMEMBER(atcModel),
                    BLACK_METAMEMBER(atcParkingCode),
                    BLACK_METAMEMBER(atcIdColor),
                    BLACK_METAMEMBER(description),
                    BLACK_METAMEMBER(uiType),
                    BLACK_METAMEMBER(uiManufacturer),
                    BLACK_METAMEMBER(uiVariation),
                    BLACK_METAMEMBER(texture),
                    BLACK_METAMEMBER(simName),
                    BLACK_METAMEMBER(createdBy),
                    BLACK_METAMEMBER(rotorcraft),
                    BLACK_METAMEMBER(timestampMSecsSinceEpoch)
                );
            };
        } // ns
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Simulation::FsCommon::CAircraftCfgEntries)

#endif // guard
