// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_ACTIONHOTKEYLISTMODEL_H
#define SWIFT_GUI_MODELS_ACTIONHOTKEYLISTMODEL_H

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QObject>
#include <QVariant>
#include <Qt>

#include "gui/swiftguiexport.h"
#include "misc/input/actionhotkeylist.h"

class QModelIndex;

namespace swift::gui::models
{
    //! Hotkey list model
    class SWIFT_GUI_EXPORT CActionHotkeyListModel : public QAbstractTableModel
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
        swift::misc::input::CActionHotkeyList m_actionHotkeys;
    };
} // namespace swift::gui::models

#endif // guard
