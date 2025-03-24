// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SETTINGSGUICOMPONENT_H
#define SWIFT_GUI_COMPONENTS_SETTINGSGUICOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

#include "gui/settings/guisettings.h"

namespace Ui
{
    class CSettingsGuiComponent;
}
namespace swift::gui::components
{
    /*!
     * General GUI settings
     */
    class CSettingsGuiComponent : public QFrame
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
        swift::misc::CSetting<settings::TGeneralGui> m_guiSettings { this, &CSettingsGuiComponent::guiSettingsChanged };
    };
} // namespace swift::gui::components
#endif // SWIFT_GUI_COMPONENTS_SETTINGSGUICOMPONENT_H
