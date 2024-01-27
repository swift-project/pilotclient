// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_LOGINDIALOG_H
#define BLACKGUI_COMPONENTS_LOGINDIALOG_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/airport.h"
#include "blackmisc/network/connectionstatus.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CLoginDialog;
}
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
} // ns

#endif // guard
