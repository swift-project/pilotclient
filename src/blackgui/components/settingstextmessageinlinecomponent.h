// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSTEXTMESSAGEINLINECOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSTEXTMESSAGEINLINECOMPONENT_H

#include "blackgui/settings/textmessagesettings.h"
#include <QScopedPointer>
#include <QFrame>

namespace Ui
{
    class CSettingsTextMessageInlineComponent;
}
namespace BlackGui::Components
{
    /*!
     * Settings for text messages
     */
    class CSettingsTextMessageInlineComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSettingsTextMessageInlineComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSettingsTextMessageInlineComponent();

    private:
        //! Settings have been changed
        void settingsChanged();

        //! Change the settings
        void changeSettings();

        //! Disable all overlay messages
        void disableAllOverlayMessages();

        //! Reset all overlay messages
        void resetOverlayMessages();

        QScopedPointer<Ui::CSettingsTextMessageInlineComponent> ui;
        swift::misc::CSetting<BlackGui::Settings::TextMessageSettings> m_settings { this, &CSettingsTextMessageInlineComponent::settingsChanged }; //!< settings changed
    };
} // ns

#endif // guard
