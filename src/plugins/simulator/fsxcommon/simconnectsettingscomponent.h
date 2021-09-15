/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_FSXCOMMON_SIMCONNECTSETTINGSCOMPONENT_H
#define BLACKSIMPLUGIN_FSXCOMMON_SIMCONNECTSETTINGSCOMPONENT_H

#include "plugins/simulator/fsxcommon/fsxcommonexport.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include "blackmisc/simulation/simulatorinfo.h"

#include <QScopedPointer>

namespace Ui { class CSimConnectSettingsComponent; }
namespace BlackSimPlugin::FsxCommon
{
    /*!
     * A component that gathers all SimConnect related settings.
     */
    class FSXCOMMON_EXPORT CSimConnectSettingsComponent : public BlackGui::COverlayMessagesFrame
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CSimConnectSettingsComponent(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CSimConnectSettingsComponent();

    private:
        //! Open simConnect.cfg using default application
        void openSwiftSimConnectCfgFile();

        //! Open simConnect.cfg using default application
        void openUserSimConnectCfgFile();

        //! Delete simConnect.cfg file
        void deleteSwiftSimConnectCfgFile();

        //! Check whether the simConnect.cfg file exists
        void checkSwiftSimConnectCfgFile();

        //! Test the SimConnect connectivity
        void testSwiftSimConnectConnection();

        //! Save a SimConnect.cfg file for FSX/P3D
        void saveSimConnectCfgFile();

        //! Save dialog for a SimConnect.ini file
        void saveSimConnectIniFileDialog();

        //! Set the simconnect info
        void setSimConnectInfo();

        //! The the value in the combobox
        void setComboBox(const QString &value);

        //! The P3D version has been changed
        void onP3DVersionChanged(const QString &version);

        BlackMisc::Simulation::CSimulatorInfo m_simulator { BlackMisc::Simulation::CSimulatorInfo::FSX };
        BlackMisc::CSetting<BlackMisc::Simulation::Settings::TP3DVersion> m_p3dVersion { this };
        QScopedPointer<Ui::CSimConnectSettingsComponent> ui;
        bool m_p3d64bit = false;
    };
} // ns

#endif // guard
