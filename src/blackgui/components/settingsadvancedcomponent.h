// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSGENERALCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSGENERALCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackcore/application/applicationsettings.h"
#include "misc/settingscache.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CSettingsAdvancedComponent;
}
namespace BlackGui::Components
{
    //! Configure general settings
    class BLACKGUI_EXPORT CSettingsAdvancedComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        CSettingsAdvancedComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSettingsAdvancedComponent();

    private:
        void crashDumpUploadEnabledChanged(int state);

        QScopedPointer<Ui::CSettingsAdvancedComponent> ui;
        swift::misc::CSetting<BlackCore::Application::TCrashDumpUploadEnabled> m_crashDumpUploadEnabled { this };
    };
} // ns

#endif // guard
