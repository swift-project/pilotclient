/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_SERVERLISTMODEL_H
#define BLACKGUI_SERVERLISTMODEL_H

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
            virtual QVariant data(const QModelIndex &index, int role) const override;
        };
    }
}
#endif // guard
