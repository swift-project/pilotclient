/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_NETWORKSETUPFORM_H
#define BLACKGUI_EDITORS_NETWORKSETUPFORM_H

#include "blackgui/editors/form.h"
#include <QScopedPointer>

namespace Ui { class CNetworkSetupForm; }
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
