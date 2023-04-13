/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_NETWORKSERVERFORM_H
#define BLACKGUI_COMPONENTS_NETWORKSERVERFORM_H

#include "blackgui/blackguiexport.h"
#include "blackgui/editors/form.h"
#include "blackmisc/network/server.h"
#include "blackmisc/statusmessagelist.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

namespace Ui
{
    class CNetworkServerForm;
}
namespace BlackGui::Editors
{
    //! Server form
    class BLACKGUI_EXPORT CServerForm : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CServerForm(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CServerForm() override;

        //! Set server
        void setServer(const BlackMisc::Network::CServer &server);

        //! Get server
        BlackMisc::Network::CServer getServer() const;

        //! Get currently selected server type
        BlackMisc::Network::CServer::ServerType getServerType() const;

        //! Reset to 1st tab
        void resetToFirstTab();

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        virtual BlackMisc::CStatusMessageList validate(bool nested = false) const override;
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
