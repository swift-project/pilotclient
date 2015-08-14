/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_ACTIONMODEL_H
#define BLACKGUI_ACTIONMODEL_H

#include "blackcore/actionbind.h"
#include <QAbstractItemModel>
#include <QScopedPointer>

namespace BlackGui
{
    namespace Models
    {

        class ActionItem;

        /*!
         * Action tree model
         */
        class CActionModel : public QAbstractItemModel
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
            ~CActionModel();

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
            void setupModelData();

            QScopedPointer<ActionItem> m_rootItem;
        };

    }
}

#endif // guard
