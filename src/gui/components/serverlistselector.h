// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SERVERLISTSELECTOR_H
#define SWIFT_GUI_COMPONENTS_SERVERLISTSELECTOR_H

#include "gui/swiftguiexport.h"
#include "misc/network/data/lastserver.h"
#include "misc/network/serverlist.h"
#include "misc/datacache.h"

#include <QComboBox>
#include <QObject>
#include <QStringList>

namespace swift::gui::components
{
    //! List of servers
    class SWIFT_GUI_EXPORT CServerListSelector : public QComboBox
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CServerListSelector(QWidget *parent = nullptr);

        //! Set the servers
        void setServers(const swift::misc::network::CServerList &servers);

        //! Get the current server
        swift::misc::network::CServer currentServer() const;

        //! Preselect
        bool preSelect(const QString &name);

    signals:
        //! Server has been changed
        void serverChanged(const swift::misc::network::CServer &server);

    private:
        //! Build the item string descriptions
        void setServerItems(const swift::misc::network::CServerList &servers);

        //! Server index has been changed
        void onServerTextChanged(const QString &text);

        swift::misc::network::CServerList m_servers; //!< corresponding servers
        QStringList m_items; //!< items strings
        QString m_pendingPreselect; //!< pending preselect value
        swift::misc::CData<swift::misc::network::data::TLastServer> m_lastServer { this }; //!< recently used server (VATSIM, other)
    };
} // ns

#endif // guard
