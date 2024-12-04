// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_LOGINMODEBUTTONS_H
#define SWIFT_GUI_LOGINMODEBUTTONS_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"
#include "misc/network/connectionstatus.h"
#include "misc/network/loginmode.h"

namespace Ui
{
    class CLoginModeButtons;
}
namespace swift::gui
{
    //! Display login modes (normal, stealth, ...)
    class SWIFT_GUI_EXPORT CLoginModeButtons : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CLoginModeButtons(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CLoginModeButtons() override;

        //! Get login mode, \sa swift::core::INetwork::LoginMode
        swift::misc::network::CLoginMode getLoginMode() const;

        //! Set login mode
        void setLoginMode(swift::misc::network::CLoginMode mode);

        //! Set to read only
        void setReadOnly(bool readonly);

    private:
        void configureLoginModes();

        QScopedPointer<Ui::CLoginModeButtons> ui;
    };
} // namespace swift::gui

#endif // SWIFT_GUI_LOGINMODEBUTTONS_H
