// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_EMULATED_SIMULATOREMULATEDCONFIGWIDGET_H
#define BLACKSIMPLUGIN_EMULATED_SIMULATOREMULATEDCONFIGWIDGET_H

#include "gui/pluginconfigwindow.h"
#include <QScopedPointer>

namespace Ui
{
    class CSimulatorEmulatedConfigWidget;
}
namespace BlackSimPlugin::Emulated
{
    /*!
     * Configuration window for CSimulatorEmulatedConfig
     */
    class Q_DECL_EXPORT CSimulatorEmulatedConfigWidget : public swift::gui::CPluginConfigWindow
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CSimulatorEmulatedConfigWidget(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CSimulatorEmulatedConfigWidget();

    private:
        QScopedPointer<Ui::CSimulatorEmulatedConfigWidget> ui;
    };
} // ns

#endif // guard
