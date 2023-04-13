/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_FSDSETUPCOMPONENT_H
#define BLACKGUI_EDITORS_FSDSETUPCOMPONENT_H

#include "blackgui/editors/form.h"
#include <QFrame>
#include <QScopedPointer>
#include "blackmisc/network/fsdsetup.h"

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
        BlackMisc::Network::CFsdSetup getValue() const;

        //! FSD setup when disabled
        const BlackMisc::Network::CFsdSetup &getDisabledValue() const;

        //! Set to GUI
        void setValue(const BlackMisc::Network::CFsdSetup &setup);

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
        virtual BlackMisc::CStatusMessageList validate(bool nested = false) const override;
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
