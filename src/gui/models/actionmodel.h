// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_ACTIONMODEL_H
#define SWIFT_GUI_MODELS_ACTIONMODEL_H

#include <QAbstractItemModel>
#include <QObject>
#include <QScopedPointer>
#include <QVariant>
#include <Qt>

#include "gui/swiftguiexport.h"

class QModelIndex;

namespace swift::gui::models
{
    class CActionItem;

    /*!
     * Action tree model, used with hotkey actions
     * \remark data from CInputManager
     */
    class SWIFT_GUI_EXPORT CActionModel : public QAbstractItemModel
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
        ~CActionModel() override;

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
} // namespace swift::gui::models

#endif // SWIFT_GUI_MODELS_ACTIONMODEL_H
