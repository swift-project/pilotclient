/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKGUI_SERVERLISTMODEL_H
#define BLACKGUI_SERVERLISTMODEL_H

#include "blackmisc/nwserverlist.h"
#include "blackgui/listmodelbase.h"
#include <QAbstractItemModel>
#include <QDBusConnection>

namespace BlackGui
{
    /*!
     * \brief Server list model
     */
    class CServerListModel : public CListModelBase<BlackMisc::Network::CServer, BlackMisc::Network::CServerList>
    {

    private:
        BlackMisc::Network::CServer m_selectedServer;

    public:

        /*!
         * \brief Constructor
         * \param parent
         */
        explicit CServerListModel(QObject *parent = nullptr);

        /*!
         * \brief Destructor
         */
        virtual ~CServerListModel() {}

        /*!
         * \brief Has selected server?
         * \return
         */
        bool hasSelectedServer() const
        {
            return this->m_selectedServer.isValidForLogin();
        }

        /*!
         * \brief Get selected server
         * \return
         */
        const BlackMisc::Network::CServer &getSelectedServer() const
        {
            return this->m_selectedServer;
        }

        /*!
         * \brief SetSelectedServer
         * \param selectedServer
         */
        void setSelectedServer(const BlackMisc::Network::CServer &selectedServer)
        {
            this->m_selectedServer = selectedServer;
        }

        /*!
         * \brief data
         * \param index
         * \param role
         * \return
         */
        virtual QVariant data(const QModelIndex &index, int role) const;

    };
}
#endif // guard
