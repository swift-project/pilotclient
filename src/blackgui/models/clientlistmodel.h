// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_CLIENTLISTMODEL_H
#define BLACKGUI_MODELS_CLIENTLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodelbase.h"
#include "misc/network/clientlist.h"

#include <QVariant>

class QModelIndex;
class QObject;

namespace BlackGui::Models
{
    //! Client list model
    class BLACKGUI_EXPORT CClientListModel : public CListModelBase<swift::misc::network::CClientList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CClientListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CClientListModel() {}

        //! \copydoc QAbstractListModel::data()
        virtual QVariant data(const QModelIndex &index, int role) const override;
    };
} // namespace
#endif // guard
