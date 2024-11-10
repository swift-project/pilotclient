// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_FSXCOMMON_SIMULATORFSXCONFIGWINDOW_H
#define SWIFT_SIMPLUGIN_FSXCOMMON_SIMULATORFSXCONFIGWINDOW_H

#include "plugins/simulator/fsxcommon/fsxcommonexport.h"
#include "gui/pluginconfigwindow.h"
#include "misc/simulation/simulatorinfo.h"
#include <QScopedPointer>

namespace Ui
{
    class CSimulatorFsxConfigWindow;
}
namespace swift::simplugin::fsxcommon
{
    /*!
     * A window that lets user set up the FSX plugin.
     */
    class FSXCOMMON_EXPORT CSimulatorFsxConfigWindow : public swift::gui::CPluginConfigWindow
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
