/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSTEXTMESSAGEINLINECOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSTEXTMESSAGEINLINECOMPONENT_H


#include "blackgui/settings/textmessagesettings.h"
#include <QScopedPointer>
#include <QFrame>

namespace Ui { class CSettingsTextMessageInlineComponent; }
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
        BlackMisc::CSetting<BlackGui::Settings::TextMessageSettings> m_settings { this, &CSettingsTextMessageInlineComponent::settingsChanged }; //!< settings changed
    };
} // ns

#endif // guard
