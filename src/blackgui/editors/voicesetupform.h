// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_VOICESETUPFORM_H
#define BLACKGUI_COMPONENTS_VOICESETUPFORM_H

#include "blackmisc/audio/voicesetup.h"
#include "blackmisc/statusmessagelist.h"
#include "blackgui/editors/form.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CVoiceSetupForm;
}
namespace BlackGui::Editors
{
    //! Voice form
    class CVoiceSetupForm : public CForm
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CVoiceSetupForm(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CVoiceSetupForm() override;

        //! Voice setup from GUI
        BlackMisc::Audio::CVoiceSetup getValue() const;

        //! Voice setup when disabled
        const BlackMisc::Audio::CVoiceSetup &getDisabledValue() const;

        //! Set to GUI
        void setValue(const BlackMisc::Audio::CVoiceSetup &setup);

        //! Enabled?
        bool isVoiceSetupEnabled() const;

        //! Set enabled / disabled
        void setVoiceSetupEnabled(bool enabled);

        //! Allow override even in read only mode
        void setAlwaysAllowOverride(bool allow);

        //! Show the enable info
        void showEnableInfo(bool visible);

        //! Set default values
        void resetToDefaultValues();

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        virtual BlackMisc::CStatusMessageList validate(bool nested = false) const override;
        //! @}

    private:
        //! Enable / disable
        void enabledToggled(bool enabled);

        //! Show / hide visible "enable" info
        void visibleEnableInfo(bool visible);

        QScopedPointer<Ui::CVoiceSetupForm> ui;
        bool m_alwaysAllowOverride = false;
        bool m_visibleEnableInfo = true;
    };
} // ns

#endif // guard
