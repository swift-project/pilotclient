// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_FSCOMMON_AIRCRAFTCFGENTRY_H
#define SWIFT_MISC_SIMULATION_FSCOMMON_AIRCRAFTCFGENTRY_H

#include <QMetaType>
#include <QString>

#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/swiftmiscexport.h"
#include "misc/timestampbased.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::simulation::fscommon, CAircraftCfgEntries)

namespace swift::misc::simulation::fscommon
{
    //! Set of aircraft.cfg entries representing an aircraft (FSX)
    //! \remarks an entry in the aircraft.cfg is title, atc type, ...
    //!          This class already bundles relevant entries, hence the class is named Entries (plural)
    class SWIFT_MISC_EXPORT CAircraftCfgEntries :
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
        swift::misc::simulation::CAircraftModel toAircraftModel() const;

        //! Thumbnail.jpg path if possible
        QString getThumbnailFileNameGuess() const;

        //! Thumbnail.jpg path if possible, and checked if file exists
        //! \remark checks file existence, consider I/O load
        QString getThumbnailFileNameChecked() const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        int m_index; //!< current index in given config
        QString m_fileName; //!< file name of .cfg
        QString m_title; //!< Title in .cfg
        QString m_atcType; //!< ATC type
        QString m_atcModel; //!< ATC model
        QString m_atcAirline; //!< ATC airline
        QString m_atcParkingCode; //!< ATC parking code
        QString m_atcIdColor; //!< e.g. 0xffffffff (not for all aircrafts)
        QString m_description; //!< descriptive text
        QString m_uiType; //!< e.g. A321-231 IAE
        QString m_uiManufacturer; //!< e.g. Airbus
        QString m_uiVariation; //!< e.g. White,Green
        QString m_texture; //!< texture, needed to identify thumbnail.jpg
        QString m_simName; //!< name in simulator
        QString m_createdBy; //!< created by, "distributor"
        bool m_rotorcraft = false; //!< hint if rotorcraft

        SWIFT_METACLASS(
            CAircraftCfgEntries,
            SWIFT_METAMEMBER(index),
            SWIFT_METAMEMBER(fileName),
            SWIFT_METAMEMBER(title),
            SWIFT_METAMEMBER(atcType),
            SWIFT_METAMEMBER(atcModel),
            SWIFT_METAMEMBER(atcParkingCode),
            SWIFT_METAMEMBER(atcIdColor),
            SWIFT_METAMEMBER(description),
            SWIFT_METAMEMBER(uiType),
            SWIFT_METAMEMBER(uiManufacturer),
            SWIFT_METAMEMBER(uiVariation),
            SWIFT_METAMEMBER(texture),
            SWIFT_METAMEMBER(simName),
            SWIFT_METAMEMBER(createdBy),
            SWIFT_METAMEMBER(rotorcraft),
            SWIFT_METAMEMBER(timestampMSecsSinceEpoch));
    };
} // namespace swift::misc::simulation::fscommon

Q_DECLARE_METATYPE(swift::misc::simulation::fscommon::CAircraftCfgEntries)

#endif // guard
