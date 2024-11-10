// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_NETWORKSERVERFORM_H
#define SWIFT_GUI_COMPONENTS_NETWORKSERVERFORM_H

#include "gui/swiftguiexport.h"
#include "gui/editors/form.h"
#include "misc/network/server.h"
#include "misc/statusmessagelist.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

namespace Ui
{
    class CNetworkServerForm;
}
namespace swift::gui::editors
{
    //! Server form
    class SWIFT_GUI_EXPORT CServerForm : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CServerForm(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CServerForm() override;

        //! Set server
        void setServer(const swift::misc::network::CServer &server);

        //! Get server
        swift::misc::network::CServer getServer() const;

        //! Get currently selected server type
        swift::misc::network::CServer::ServerType getServerType() const;

        //! Reset to 1st tab
        void resetToFirstTab();

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        virtual swift::misc::CStatusMessageList validate(bool nested = false) const override;
        //! @}

        //! Show the password field
        void showPasswordField(bool show);

    private:
        void initServerTypes();
        void onChangedServerType(const QString &text);
        void onChangedEcoSystem(const QString &text);
        void tempUnhidePassword();

        QScopedPointer<Ui::CNetworkServerForm> ui;
        QString m_passwordNameLabel;
    };
} // ns

#endif // guard
