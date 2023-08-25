// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_EDITORS_NETWORKSETUPFORM_H
#define BLACKGUI_EDITORS_NETWORKSETUPFORM_H

#include "blackgui/editors/form.h"
#include <QScopedPointer>

namespace Ui
{
    class CNetworkSetupForm;
}
namespace BlackGui::Editors
{
    //! Setup form
    class CNetworkSetupForm : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CNetworkSetupForm(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CNetworkSetupForm() override;

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        //! @}

    private:
        QScopedPointer<Ui::CNetworkSetupForm> ui;
    };
} // ns

#endif // guard
