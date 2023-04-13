/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SERVERLISTSELECTOR_H
#define BLACKGUI_COMPONENTS_SERVERLISTSELECTOR_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/network/data/lastserver.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/country.h"
#include "blackmisc/datacache.h"

#include <QComboBox>
#include <QObject>
#include <QStringList>

namespace BlackGui::Components
{
    //! List of servers
    class BLACKGUI_EXPORT CServerListSelector : public QComboBox
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CServerListSelector(QWidget *parent = nullptr);

        //! Set the servers
        void setServers(const BlackMisc::Network::CServerList &servers, bool nameIsCountry = false);

        //! Get the current server
        BlackMisc::Network::CServer currentServer() const;

        //! Preselect
        bool preSelect(const QString &name);

    signals:
        //! Server has been changed
        void serverChanged(const BlackMisc::Network::CServer &server);

    private:
        //! Build the item string descriptions
        void setServerItems(const BlackMisc::Network::CServerList &servers, bool nameToCountry);

        //! Server index has been changed
        void onServerTextChanged(const QString &text);

        //! Do we know all countries?
        static bool knowsAllCountries();

        //! Turn server name into country (for VATSIM servers)
        static BlackMisc::CCountry findCountry(const BlackMisc::Network::CServer &server);

        BlackMisc::Network::CServerList m_servers; //!< corresponding servers
        QStringList m_items; //!< items strings
        QString m_pendingPreselect; //!< pending preselect value
        BlackMisc::CData<BlackMisc::Network::Data::TLastServer> m_lastServer { this }; //!< recently used server (VATSIM, other)
    };
} // ns

#endif // guard
