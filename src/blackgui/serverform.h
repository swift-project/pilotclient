/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_NETWORKSERVERFORM_H
#define BLACKGUI_NETWORKSERVERFORM_H

#include "blackmisc/nwserver.h"
#include "blackmisc/statusmessagelist.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CNetworkServerForm; }

namespace BlackGui
{

    //! Server form
    class CServerForm : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CServerForm(QWidget *parent = nullptr);

        //! Destructor
        ~CServerForm();

        //! Set server
        void setServer(const BlackMisc::Network::CServer &server);

        //! Get server
        BlackMisc::Network::CServer getServer() const;

        //! Set read only
        void setReadOnly(bool readOnly);

        //! Show the password field
        void showPasswordField(bool show);

        //! Validate and provide messages (empty means OK)
        BlackMisc::CStatusMessageList validate() const;

    private:
        QScopedPointer<Ui::CNetworkServerForm> ui;
    };
}

#endif // guard
