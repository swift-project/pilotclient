// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_EDITORS_FSDSETUPCOMPONENT_H
#define BLACKGUI_EDITORS_FSDSETUPCOMPONENT_H

#include "blackgui/editors/form.h"
#include <QFrame>
#include <QScopedPointer>
#include "misc/network/fsdsetup.h"

namespace Ui
{
    class CFsdSetupForm;
}
namespace BlackGui::Editors
{
    /*!
     * Setup for FSD
     */
    class CFsdSetupForm : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CFsdSetupForm(QWidget *parent = nullptr);

        //! Constructor
        virtual ~CFsdSetupForm() override;

        //! FSD setup from GUI
        swift::misc::network::CFsdSetup getValue() const;

        //! FSD setup when disabled
        const swift::misc::network::CFsdSetup &getDisabledValue() const;

        //! Set to GUI
        void setValue(const swift::misc::network::CFsdSetup &setup);

        //! Allow override even in read only mode
        void setAlwaysAllowOverride(bool allow);

        //! Enabled?
        bool isFsdSetupEnabled() const;

        //! Set enabled / disabled
        void setFsdSetupEnabled(bool enabled);

        //! Show the enable info
        void showEnableInfo(bool visible);

        //! Set default values
        void resetToDefaultValues();

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        virtual swift::misc::CStatusMessageList validate(bool nested = false) const override;
        //! @}

    private:
        //! Enable / disable
        void enabledToggled(bool enabled);

        //! Show / hide visible "enable" info
        void visibleEnableInfo(bool visible);

        QScopedPointer<Ui::CFsdSetupForm> ui;
        bool m_visibleEnableInfo = true;
        bool m_alwaysAllowOverride = false;
    };
} // ns

#endif // guard
