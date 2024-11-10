// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SETTINGSGENERALCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_SETTINGSGENERALCOMPONENT_H

#include "gui/swiftguiexport.h"
#include "core/application/applicationsettings.h"
#include "misc/settingscache.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CSettingsAdvancedComponent;
}
namespace swift::gui::components
{
    //! Configure general settings
    class SWIFT_GUI_EXPORT CSettingsAdvancedComponent : public QFrame
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
        swift::misc::CSetting<swift::core::application::TCrashDumpUploadEnabled> m_crashDumpUploadEnabled { this };
    };
} // ns

#endif // guard
