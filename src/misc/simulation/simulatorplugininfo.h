// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_SIMULATORPLUGININFO_H
#define SWIFT_MISC_SIMULATION_SIMULATORPLUGININFO_H

#include <QJsonObject>
#include <QMetaType>
#include <QString>

#include "misc/metaclass.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::simulation, CSimulatorPluginInfo)

namespace swift::misc::simulation
{
    //! Describing a simulator plugin
    class SWIFT_MISC_EXPORT CSimulatorPluginInfo : public CValueObject<CSimulatorPluginInfo>
    {
    public:
        //! Default constructor
        CSimulatorPluginInfo() = default;

        //! Constructor (used with unit tests)
        CSimulatorPluginInfo(const QString &identifier, const QString &name, const QString &simulator,
                             const QString &description, bool valid);

        //! \copydoc swift::misc::CValueObject::convertFromJson
        void convertFromJson(const QJsonObject &json);

        //! Unspecified simulator
        bool isUnspecified() const;

        //! Check if the provided plugin metadata is valid.
        //! Simulator plugin (driver) has to meet the following requirements:
        //!  * implements org.swift-project.swift_core.simulatorinterface;
        //!  * provides plugin name;
        //!  * specifies simulator it handles.
        //! Unspecified sim is considered as invalid.
        bool isValid() const { return m_valid; }

        //! Identifier
        const QString &getIdentifier() const { return m_identifier; }

        //! Name
        const QString &getName() const { return m_name; }

        //! Simulator
        const QString &getSimulator() const { return m_simulator; }

        //! Simulator info object
        const CSimulatorInfo &getSimulatorInfo() const { return m_info; }

        //! Is this the emulated driver?
        bool isEmulatedPlugin() const;

        //! Description
        const QString &getDescription() const { return m_description; }

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Simulator info to plugin indentifier, empty string if not single simulator
        static const QString &identifierFromSimulatorInfo(const CSimulatorInfo &);

        //! Plugin identifier (FSX)
        static const QString &fsxPluginIdentifier();

        //! Plugin identifier (P3D)
        static const QString &p3dPluginIdentifier();

        //! Plugin identifier (FS9)
        static const QString &fs9PluginIdentifier();

        //! Plugin identifier (XPlane)
        static const QString &xplanePluginIdentifier();

        //! Plugin identifier (FlightGear)
        static const QString &fgPluginIdentifier();

        //! Plugin identifier (MSFS 2020)
        static const QString &msfsPluginIdentifier();

        //! Plugin identifier (MSFS 2024)
        static const QString &msfs2024PluginIdentifier();

        //! Plugin identifier (emulated simulator plugin)
        static const QString &emulatedPluginIdentifier();

        //! All valid identifiers
        static const QStringList &allIdentifiers();

        //! Guess default plugins
        static QStringList guessDefaultPlugins();

    private:
        QString m_identifier;
        QString m_name;
        QString m_simulator;
        QString m_description;
        CSimulatorInfo m_info;
        bool m_valid { false };

        SWIFT_METACLASS(
            CSimulatorPluginInfo,
            SWIFT_METAMEMBER(identifier, 0, CaseInsensitiveComparison),
            SWIFT_METAMEMBER(name, 0, DisabledForComparison | DisabledForHashing),
            SWIFT_METAMEMBER(simulator, 0, DisabledForComparison | DisabledForHashing),
            SWIFT_METAMEMBER(description, 0, DisabledForComparison | DisabledForHashing),
            SWIFT_METAMEMBER(info, 0, DisabledForComparison | DisabledForHashing),
            SWIFT_METAMEMBER(valid, 0, DisabledForComparison | DisabledForHashing));
    };
} // namespace swift::misc::simulation

Q_DECLARE_METATYPE(swift::misc::simulation::CSimulatorPluginInfo)

#endif // guard
