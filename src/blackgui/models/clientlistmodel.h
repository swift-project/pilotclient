/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_CLIENTLISTMODEL_H
#define BLACKGUI_CLIENTLISTMODEL_H

#include <QAbstractItemModel>
#include "blackmisc/nwclientlist.h"
#include "blackgui/models/listmodelbase.h"

namespace BlackGui
{
    namespace Models
    {
        /*!
         * Server list model
         */
        class CClientListModel : public CListModelBase<BlackMisc::Network::CClient, BlackMisc::Network::CClientList>
        {

        public:
            //! Constructor
            explicit CClientListModel(QObject *parent = nullptr);

            //! Destructor
            virtual ~CClientListModel() {}

            //! \copydoc CListModelBase::data
            QVariant data(const QModelIndex &modelIndex, int role = Qt::DisplayRole) const;

        };
    }
}
#endif // guard
