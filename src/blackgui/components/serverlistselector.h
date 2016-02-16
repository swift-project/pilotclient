/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SERVERLISTSELECTOR_H
#define BLACKGUI_COMPONENTS_SERVERLISTSELECTOR_H

#include "blackgui/blackguiexport.h"
#include <QComboBox>
#include <QStringList>

#include "blackmisc/network/serverlist.h"

namespace BlackGui
{
    namespace Components
    {
        //! List of servers
        class BLACKGUI_EXPORT CServerListSelector : public QComboBox
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CServerListSelector(QWidget *parent = nullptr);

            //! Set the servers
            void setServers(const BlackMisc::Network::CServerList &servers);

            //! Get the current server
            BlackMisc::Network::CServer currentServer() const;

        private:
            //! Build the item string descriptions
            void setItemStrings(const BlackMisc::Network::CServerList &servers);

            BlackMisc::Network::CServerList m_servers; //!< corresponding servers
            QStringList m_items;                       //!< items strings
        };
    } // ns
} // ns

#endif // guard
