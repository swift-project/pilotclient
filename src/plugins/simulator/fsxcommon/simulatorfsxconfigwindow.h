/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_FSXCOMMON_SIMULATORFSXCONFIGWINDOW_H
#define BLACKSIMPLUGIN_FSXCOMMON_SIMULATORFSXCONFIGWINDOW_H

#include "plugins/simulator/fsxcommon/fsxcommonexport.h"
#include "blackgui/pluginconfigwindow.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include <QScopedPointer>

namespace Ui { class CSimulatorFsxConfigWindow; }
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
        const BlackMisc::Simulation::CSimulatorInfo &getSimulator() const { return m_simulator; }

    private:
        const BlackMisc::Simulation::CSimulatorInfo m_simulator { "FSX" };
        QScopedPointer<Ui::CSimulatorFsxConfigWindow> ui;
    };
} // ns

#endif // guard
