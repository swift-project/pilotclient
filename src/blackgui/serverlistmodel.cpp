/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "serverlistmodel.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QMetaProperty>
#include <QBrush>

using namespace BlackMisc::Network;

namespace BlackGui
{
    /*
     * Constructor
     */
    CServerListModel::CServerListModel(QObject *parent) :
        CListModelBase("ViewServerList", parent)
    {
        this->m_columns.addColumn(CServer::IndexName, "name");
        this->m_columns.addColumn(CServer::IndexDescription, "description");
        this->m_columns.addColumn(CServer::IndexAddress, "address");
        this->m_columns.addColumn(CServer::IndexPort, "port");
        this->m_columns.addColumn(CServer::IndexUserRealname, "username");
        this->m_columns.addColumn(CServer::IndexUserId, "userid");

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ViewServerList", "name");
        (void)QT_TRANSLATE_NOOP("ViewServerList", "description");
        (void)QT_TRANSLATE_NOOP("ViewServerList", "address");
        (void)QT_TRANSLATE_NOOP("ViewServerList", "port");
        (void)QT_TRANSLATE_NOOP("ViewServerList", "username");
        (void)QT_TRANSLATE_NOOP("ViewServerList", "userid");
    }

    /*
     * Special functions
     */
    QVariant CServerListModel::data(const QModelIndex &index, int role) const
    {
        if (role == Qt::BackgroundRole)
        {
            if (!this->hasSelectedServer()) return QVariant();
            CServer currentRow = this->at(index);
            if (currentRow == this->getSelectedServer())
            {
                QBrush background(Qt::green);
                return background;
            }
            else
            {
                return QVariant();
            }
        }
        else
        {
            return CListModelBase::data(index, role);
        }
    }
}
