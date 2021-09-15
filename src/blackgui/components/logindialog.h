/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_LOGINDIALOG_H
#define BLACKGUI_COMPONENTS_LOGINDIALOG_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/airport.h"
#include "blackmisc/network/connectionstatus.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui { class CLoginDialog; }
namespace BlackGui::Components
{
    //! Login dialog
    class BLACKGUI_EXPORT CLoginDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CLoginDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CLoginDialog() override;

        //! Set auto logoff
        void setAutoLogoff(bool logoff);

        //! Init and show
        void show();

    signals:
        //! Request server settings
        void requestNetworkSettings();

    private:
        void init();
        void onLoginOrLogoffCancelled();
        void onLoginOrLogoffSuccessful();
        void onRequestNetworkSettings();
        void onNetworkStatusChanged(const BlackMisc::Network::CConnectionStatus &from, const BlackMisc::Network::CConnectionStatus &to);

        QScopedPointer<Ui::CLoginDialog> ui;
    };
}// ns

#endif // guard
