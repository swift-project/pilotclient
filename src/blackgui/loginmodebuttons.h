// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_LOGINMODEBUTTONS_H
#define BLACKGUI_LOGINMODEBUTTONS_H

#include "blackgui/blackguiexport.h"
#include "misc/network/connectionstatus.h"
#include "misc/network/loginmode.h"

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
        swift::misc::network::CLoginMode getLoginMode() const;

        //! Set login mode
        void setLoginMode(swift::misc::network::CLoginMode mode);

        //! Set to read only
        void setReadOnly(bool readonly);

    private:
        void configureLoginModes();

        QScopedPointer<Ui::CLoginModeButtons> ui;
    };
} // ns

#endif // guard
