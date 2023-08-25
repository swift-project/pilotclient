// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_ACTIONMODEL_H
#define BLACKGUI_MODELS_ACTIONMODEL_H

#include "blackgui/blackguiexport.h"

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QObject>
#include <QScopedPointer>
#include <QVariant>
#include <Qt>

namespace BlackGui::Models
{
    class CActionItem;

    /*!
     * Action tree model, used with hotkey actions
     * \remark data from CInputManager
     */
    class BLACKGUI_EXPORT CActionModel : public QAbstractItemModel
    {
        Q_OBJECT

    public:
        //! User roles
        enum ItemRole
        {
            ActionRole = Qt::UserRole
        };

        //! Constructor
        CActionModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CActionModel() override;

        //! \copydoc QAbstractItemModel::data
        QVariant data(const QModelIndex &index, int role) const override;

        //! \copydoc QAbstractItemModel::flags
        Qt::ItemFlags flags(const QModelIndex &index) const override;

        //! \copydoc QAbstractItemModel::index
        QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

        //! \copydoc QAbstractItemModel::parent
        QModelIndex parent(const QModelIndex &index) const override;

        //! \copydoc QAbstractItemModel::rowCount
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;

        //! \copydoc QAbstractItemModel::columnCount
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    private:
        //! Init model data
        void setupModelData();

        QScopedPointer<CActionItem> m_rootItem;
    };
} // ns

#endif // guard
