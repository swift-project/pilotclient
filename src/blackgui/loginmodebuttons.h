// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_LOGINMODEBUTTONS_H
#define BLACKGUI_LOGINMODEBUTTONS_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/network/connectionstatus.h"
#include "blackmisc/network/loginmode.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CLoginModeButtons;
}
namespace BlackGui
{
    //! Display login modes (normal, stealth, ...)
    class BLACKGUI_EXPORT CLoginModeButtons : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CLoginModeButtons(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CLoginModeButtons() override;

        //! Get login mode, \sa BlackCore::INetwork::LoginMode
        BlackMisc::Network::CLoginMode getLoginMode() const;

        //! Set login mode
        void setLoginMode(BlackMisc::Network::CLoginMode mode);

        //! Set to read only
        void setReadOnly(bool readonly);

    private:
        void configureLoginModes();

        QScopedPointer<Ui::CLoginModeButtons> ui;
    };
} // ns

#endif // guard
