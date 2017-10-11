/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
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
        class BLACKMISC_EXPORT CSimulatorPluginInfo : public BlackMisc::CValueObject<CSimulatorPluginInfo>
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
            BlackMisc::Simulation::CSimulatorInfo getSimulatorInfo() const;

            //! Is this the emulated driver?
            bool isEmulatedPlugin() const;

            //! Description
            const QString &getDescription() const { return m_description; }

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! Plugin identifier (FSX)
            static const QString &fsxPluginIndentifier();

            //! Plugin identifier (P3D)
            static const QString &p3dPluginIndentifier();

            //! Plugin identifier (FS9)
            static const QString &fs9PluginIndentifier();

            //! Plugin identifier (XPlane)
            static const QString &xplanePluginIndentifier();

            //! Plugin identifier (emulated simulator plugin)
            static const QString &emulatedPluginIndentifier();

            //! All valid identifiers
            static const QStringList &allIdentifiers();

            //! Guess default plugins
            static QStringList guessDefaultPlugins();

        private:
            QString m_identifier;
            QString m_name;
            QString m_simulator;
            QString m_description;
            bool m_valid { false };

            BLACK_METACLASS(
                CSimulatorPluginInfo,
                BLACK_METAMEMBER(identifier, 0, CaseInsensitiveComparison),
                BLACK_METAMEMBER(name, 0, DisabledForComparison | DisabledForHashing),
                BLACK_METAMEMBER(simulator, 0, DisabledForComparison | DisabledForHashing),
                BLACK_METAMEMBER(description, 0, DisabledForComparison | DisabledForHashing),
                BLACK_METAMEMBER(valid, 0, DisabledForComparison | DisabledForHashing)
            );
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Simulation::CSimulatorPluginInfo)

#endif // guard
