/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
