/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/inputmanager.h"
#include "blackgui/models/actionitem.h"
#include "blackgui/models/actionmodel.h"

#include <QString>
#include <QStringList>
#include <QtGlobal>

namespace BlackGui
{
    namespace Models
    {
        CActionModel::CActionModel(QObject *parent) :
            QAbstractItemModel(parent),
            m_rootItem(new ActionItem(QString(), QString()))
        {
            setupModelData();
        }

        CActionModel::~CActionModel()
        { }

        int CActionModel::columnCount(const QModelIndex &parent) const
        {
            return parent.isValid() ?
                   static_cast<ActionItem *>(parent.internalPointer())->getColumnCount() :
                   m_rootItem->getColumnCount();
        }

        QVariant CActionModel::data(const QModelIndex &index, int role) const
        {
            if (!index.isValid()) { return QVariant(); }

            const ActionItem *item = static_cast<ActionItem *>(index.internalPointer());

            if (role == Qt::DisplayRole) { return item->getActionName(); }
            if (role == ActionRole) { return item->getAction(); }

            return {};
        }

        Qt::ItemFlags CActionModel::flags(const QModelIndex &index) const
        {
            if (!index.isValid()) { return 0; }
            const ActionItem *item = static_cast<ActionItem *>(index.internalPointer());
            const Qt::ItemFlags flags = QAbstractItemModel::flags(index);
            const bool selectable = item && !item->hasChildren(); // only leafs are selectable
            return selectable ? flags | Qt::ItemIsSelectable : flags & ~Qt::ItemIsSelectable;
        }

        QModelIndex CActionModel::index(int row, int column, const QModelIndex &parent) const
        {
            if (!hasIndex(row, column, parent)) { return QModelIndex(); }

            const ActionItem *parentItem = parent.isValid() ?
                                           static_cast<ActionItem *>(parent.internalPointer()) :
                                           m_rootItem.data();

            ActionItem *childItem = parentItem->getChildByRow(row);
            return childItem ?
                   createIndex(row, column, childItem) :
                   QModelIndex();
        }

        QModelIndex CActionModel::parent(const QModelIndex &index) const
        {
            if (!index.isValid()) { return {}; }

            ActionItem *childItem = static_cast<ActionItem *>(index.internalPointer());
            ActionItem *parentItem = childItem->getParentItem();

            if (parentItem == m_rootItem.data()) { return {}; }

            return createIndex(parentItem->getRow(), 0, parentItem);
        }

        int CActionModel::rowCount(const QModelIndex &parent) const
        {
            ActionItem *parentItem;
            if (parent.column() > 0) { return 0; }

            if (!parent.isValid()) { parentItem = m_rootItem.data(); }
            else { parentItem = static_cast<ActionItem *>(parent.internalPointer()); }

            return parentItem->getChildCount();
        }

        void CActionModel::setupModelData()
        {
            m_rootItem.reset(new ActionItem(QString(), QString()));

            for (const auto &actionPath : BlackCore::CInputManager::instance()->allAvailableActions())
            {
                const auto tokens = actionPath.split("/", QString::SkipEmptyParts);
                ActionItem *parentItem = m_rootItem.data();
                for (const auto &token : tokens)
                {
                    ActionItem *child = parentItem->findChildByName(token);
                    if (child == nullptr)
                    {
                        child = new ActionItem(actionPath, token, parentItem);
                        parentItem->appendChild(child);
                    }
                    Q_ASSERT(child);
                    parentItem = child;
                }
            }
        }
    }
}
