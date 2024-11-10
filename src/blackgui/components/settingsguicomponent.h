// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSGUICOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSGUICOMPONENT_H

#include "blackgui/singleapplicationui.h"
#include "blackgui/settings/guisettings.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CSettingsGuiComponent;
}
namespace BlackGui::Components
{
    /*!
     * General GUI settings
     */
    class CSettingsGuiComponent :
        public QFrame,
        public BlackGui::CSingleApplicationUi
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSettingsGuiComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSettingsGuiComponent();

        //! Hide opacity elements
        void hideOpacity(bool hide);

        //! GUI Opacity 0-100%
        void setGuiOpacity(double value);

    signals:
        //! Change the windows opacity 0..100
        void changedWindowsOpacity(int opacity);

    private:
        //! Selection radio buttons changed
        void selectionChanged();

        //! GUI settings changed
        void guiSettingsChanged();

        //! Widget style has changed
        void widgetStyleChanged(int index);

        QScopedPointer<Ui::CSettingsGuiComponent> ui;
        swift::misc::CSetting<Settings::TGeneralGui> m_guiSettings { this, &CSettingsGuiComponent::guiSettingsChanged };
    };
} // ns
#endif // guard
