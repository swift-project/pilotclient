// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_FSXCOMMON_SIMULATORFSXCONFIGWINDOW_H
#define BLACKSIMPLUGIN_FSXCOMMON_SIMULATORFSXCONFIGWINDOW_H

#include "plugins/simulator/fsxcommon/fsxcommonexport.h"
#include "blackgui/pluginconfigwindow.h"
#include "misc/simulation/simulatorinfo.h"
#include <QScopedPointer>

namespace Ui
{
    class CSimulatorFsxConfigWindow;
}
namespace BlackSimPlugin::FsxCommon
{
    /*!
     * A window that lets user set up the FSX plugin.
     */
    class FSXCOMMON_EXPORT CSimulatorFsxConfigWindow : public BlackGui::CPluginConfigWindow
    {
        Q_OBJECT

    public:
        //! Ctor.
        CSimulatorFsxConfigWindow(const QString &simulator, QWidget *parent);

        //! Dtor.
        virtual ~CSimulatorFsxConfigWindow();

        //! Related simulator, i.e. "P3D" or "FSX"
        const swift::misc::simulation::CSimulatorInfo &getSimulator() const { return m_simulator; }

    private:
        const swift::misc::simulation::CSimulatorInfo m_simulator { "FSX" };
        QScopedPointer<Ui::CSimulatorFsxConfigWindow> ui;
    };
} // ns

#endif // guard
