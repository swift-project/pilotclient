/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_SIMULATORPLUGININFO_H
#define BLACKMISC_SIMULATION_SIMULATORPLUGININFO_H

#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/valueobject.h"

#include <QJsonObject>
#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Simulation
    {
        //! Describing a simulator plugin
        class BLACKMISC_EXPORT CSimulatorPluginInfo : public CValueObject<CSimulatorPluginInfo>
        {
        public:
            //! Default constructor
            CSimulatorPluginInfo() = default;

            //! Constructor (used with unit tests)
            CSimulatorPluginInfo(const QString &identifier, const QString &name,
                                 const QString &simulator, const QString &description, bool valid);

            //! \copydoc BlackMisc::CValueObject::convertFromJson
            void convertFromJson(const QJsonObject &json);

            //! Unspecified simulator
            bool isUnspecified() const;

            //! Check if the provided plugin metadata is valid.
            //! Simulator plugin (driver) has to meet the following requirements:
            //!  * implements org.swift-project.blackcore.simulatorinterface;
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

            //! \copydoc BlackMisc::Mixin::String::toQString
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

            BLACK_METACLASS(
                CSimulatorPluginInfo,
                BLACK_METAMEMBER(identifier, 0, CaseInsensitiveComparison),
                BLACK_METAMEMBER(name, 0, DisabledForComparison | DisabledForHashing),
                BLACK_METAMEMBER(simulator, 0, DisabledForComparison | DisabledForHashing),
                BLACK_METAMEMBER(description, 0, DisabledForComparison | DisabledForHashing),
                BLACK_METAMEMBER(info, 0, DisabledForComparison | DisabledForHashing),
                BLACK_METAMEMBER(valid, 0, DisabledForComparison | DisabledForHashing)
            );
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Simulation::CSimulatorPluginInfo)

#endif // guard
