/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKGUI_SERVERLISTMODEL_H
#define BLACKGUI_SERVERLISTMODEL_H

//! \file

#include "blackmisc/nwserverlist.h"
#include "blackgui/models/listmodelbase.h"
#include <QAbstractItemModel>

namespace BlackGui
{
    namespace Models
    {
        //! Server list model
        class CServerListModel : public CListModelBase<BlackMisc::Network::CServer, BlackMisc::Network::CServerList>
        {

        private:
            BlackMisc::Network::CServer m_selectedServer;

        public:

            //! Constructor
            explicit CServerListModel(QObject *parent = nullptr);

            //! Destructor
            virtual ~CServerListModel() {}

            //! Has selected server?
            bool hasSelectedServer() const
            {
                return this->m_selectedServer.isValidForLogin();
            }

            //! Get selected server
            const BlackMisc::Network::CServer &getSelectedServer() const
            {
                return this->m_selectedServer;
            }

            //! Set selected server
            void setSelectedServer(const BlackMisc::Network::CServer &selectedServer)
            {
                this->m_selectedServer = selectedServer;
            }

            //! \copydoc CListModelBase::data
            virtual QVariant data(const QModelIndex &index, int role) const;

        };
    }
}
#endif // guard
