// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_USERCOMPONENT_H
#define BLACKGUI_USERCOMPONENT_H

#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/blackguiexport.h"
#include "blackgui/settings/viewupdatesettings.h"
#include "misc/aviation/callsign.h"
#include "misc/network/connectionstatus.h"

#include <QObject>
#include <QScopedPointer>
#include <QTabWidget>
#include <QtGlobal>
#include <QTimer>

namespace Ui
{
    class CUserComponent;
}
namespace BlackGui::Components
{
    //! User componenet (users, clients)
    class BLACKGUI_EXPORT CUserComponent :
        public QTabWidget,
        public CEnableForDockWidgetInfoArea
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CUserComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CUserComponent() override;

        //! Number of clients
        int countClients() const;

        //! Number of users
        int countUsers() const;

        //! Update users
        void update();

    signals:
        //! Request a text message
        void requestTextMessageWidget(const swift::misc::aviation::CCallsign &callsign);

    private:
        //! Number of elements changed
        void onCountChanged(int count, bool withFilter);

        //! Connection status
        void onConnectionStatusChanged(const swift::misc::network::CConnectionStatus &from, const swift::misc::network::CConnectionStatus &to);

        //! Settings have been changed
        void onSettingsChanged();

        QScopedPointer<Ui::CUserComponent> ui;
        QTimer m_updateTimer;
        swift::misc::CSettingReadOnly<BlackGui::Settings::TViewUpdateSettings> m_settings { this, &CUserComponent::onSettingsChanged };
    };
} // ns
#endif // guard
