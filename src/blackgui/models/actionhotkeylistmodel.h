/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_ACTIONHOTKEYLISTMODEL_H
#define BLACKGUI_MODELS_ACTIONHOTKEYLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/input/actionhotkeylist.h"

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QObject>
#include <QVariant>
#include <Qt>

class QModelIndex;

namespace BlackGui::Models
{
    //! Hotkey list model
    class BLACKGUI_EXPORT CActionHotkeyListModel : public QAbstractTableModel
    {
        Q_OBJECT

    public:
        //! Item role
        enum ItemRole
        {
            ActionHotkeyRole = Qt::UserRole
        };

        //! Constructor
        CActionHotkeyListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CActionHotkeyListModel() override {}

        //! \copydoc QAbstractTableModel::rowCount
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;

        //! \copydoc QAbstractTableModel::columnCount
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;

        //! \copydoc QAbstractTableModel::data
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

        //! \copydoc QAbstractTableModel::setData
        bool setData(const QModelIndex &index, const QVariant &var, int role) override;

        //! \copydoc QAbstractTableModel::headerData
        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

        //! \copydoc QAbstractTableModel::insertRows
        bool insertRows(int position, int rows, const QModelIndex &index) override;

        //! \copydoc QAbstractTableModel::removeRows
        bool removeRows(int position, int rows, const QModelIndex &index) override;

        //! Clear model
        void clear();

    private:
        BlackMisc::Input::CActionHotkeyList m_actionHotkeys;
    };
} // ns

#endif // guard
