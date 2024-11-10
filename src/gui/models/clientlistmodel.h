// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_CLIENTLISTMODEL_H
#define SWIFT_GUI_MODELS_CLIENTLISTMODEL_H

#include "gui/swiftguiexport.h"
#include "gui/models/listmodelbase.h"
#include "misc/network/clientlist.h"

#include <QVariant>

class QModelIndex;
class QObject;

namespace swift::gui::models
{
    //! Client list model
    class SWIFT_GUI_EXPORT CClientListModel : public CListModelBase<swift::misc::network::CClientList>
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
