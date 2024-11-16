// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/actionhotkeylistmodel.h"

#include <QString>
#include <QtGlobal>

#include "misc/identifier.h"
#include "misc/input/actionhotkey.h"
#include "misc/input/hotkeycombination.h"
#include "misc/sequence.h"

using namespace swift::misc;
using namespace swift::misc::input;

namespace swift::gui::models
{
    CActionHotkeyListModel::CActionHotkeyListModel(QObject *parent) : QAbstractTableModel(parent) {}

    int CActionHotkeyListModel::rowCount(const QModelIndex & /** parent **/) const { return m_actionHotkeys.size(); }

    int CActionHotkeyListModel::columnCount(const QModelIndex & /** parent **/) const { return 3; }

    QVariant CActionHotkeyListModel::data(const QModelIndex &index, int role) const
    {
        if (!index.isValid()) { return QVariant(); }
        if (index.row() >= m_actionHotkeys.size() || index.row() < 0) { return QVariant(); }

        if (role == Qt::DisplayRole)
        {
            const int col = index.column();
            if (col == 0)
            {
                const CIdentifier identifier = m_actionHotkeys[index.row()].getApplicableMachine();
                return identifier.getMachineName();
            }
            if (col == 1)
            {
                const CHotkeyCombination combination = m_actionHotkeys[index.row()].getCombination();
                return combination.asStringWithDeviceNames();
                // return combination.toQString();
            }
            if (col == 2) { return m_actionHotkeys[index.row()].getAction(); }
        }
        else if (role == ActionHotkeyRole)
        {
            const auto hotkey = m_actionHotkeys[index.row()];
            return QVariant::fromValue(hotkey);
        }
        return {};
    }

    QVariant CActionHotkeyListModel::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if (role == Qt::DisplayRole)
        {
            if (orientation == Qt::Horizontal)
            {
                switch (section)
                {
                case 0: return QStringLiteral("Machine");
                case 1: return QStringLiteral("Combination");
                case 2: return QStringLiteral("Action");
                }
            }
        }
        return {};
    }

    bool CActionHotkeyListModel::insertRows(int position, int rows, const QModelIndex &index)
    {
        Q_UNUSED(index)
        beginInsertRows(QModelIndex(), position, position + rows - 1);

        for (int row = 0; row < rows; ++row) { m_actionHotkeys.push_back(swift::misc::input::CActionHotkey()); }

        endInsertRows();
        return true;
    }

    bool CActionHotkeyListModel::removeRows(int position, int rows, const QModelIndex &index)
    {
        Q_UNUSED(index)
        beginRemoveRows(QModelIndex(), position, position + rows - 1);

        Q_ASSERT(position + rows - 1 < m_actionHotkeys.size());

        for (int row = 0; row < rows; ++row)
        {
            auto toRemove = m_actionHotkeys[position + row];
            m_actionHotkeys.remove(toRemove);
        }

        endRemoveRows();
        return true;
    }

    bool CActionHotkeyListModel::setData(const QModelIndex &index, const QVariant &var, int role)
    {
        if (index.isValid() && role == ActionHotkeyRole)
        {
            m_actionHotkeys[index.row()] = var.value<swift::misc::input::CActionHotkey>();
            emit dataChanged(index, index);
            return true;
        }
        return false;
    }

    void CActionHotkeyListModel::clear()
    {
        beginResetModel();
        m_actionHotkeys.clear();
        endResetModel();
    }
} // namespace swift::gui::models
