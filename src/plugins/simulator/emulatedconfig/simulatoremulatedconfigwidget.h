/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_EMULATED_SIMULATOREMULATEDCONFIGWIDGET_H
#define BLACKSIMPLUGIN_EMULATED_SIMULATOREMULATEDCONFIGWIDGET_H

#include <blackgui/pluginconfigwindow.h>
#include <QScopedPointer>

namespace Ui { class CSimulatorEmulatedConfigWidget; }
namespace BlackSimPlugin::Emulated
{
    /**
     * Configuration window for CSimulatorEmulatedConfig
     */
    class Q_DECL_EXPORT CSimulatorEmulatedConfigWidget : public BlackGui::CPluginConfigWindow
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CSimulatorEmulatedConfigWidget(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CSimulatorEmulatedConfigWidget();

    private:
        QScopedPointer <Ui::CSimulatorEmulatedConfigWidget> ui;
    };
} // ns

#endif // guard
