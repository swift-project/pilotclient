#include "actionmodel.h"
#include "actionitem.h"
#include "blackcore/input_manager.h"

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
        {
        }

        int CActionModel::columnCount(const QModelIndex &parent) const
        {
            if (parent.isValid()) { return static_cast<ActionItem *>(parent.internalPointer())->getColumnCount(); }
            else { return m_rootItem->getColumnCount(); }
        }

        QVariant CActionModel::data(const QModelIndex &index, int role) const
        {
            if (!index.isValid()) { return QVariant(); }

            ActionItem *item = static_cast<ActionItem *>(index.internalPointer());

            if (role == Qt::DisplayRole) { return item->getActionName(); }
            if (role == ActionRole) { return item->getAction(); }

            return {};
        }

        Qt::ItemFlags CActionModel::flags(const QModelIndex &index) const
        {
            if (!index.isValid()) { return 0; }

            return QAbstractItemModel::flags(index);
        }

        QModelIndex CActionModel::index(int row, int column, const QModelIndex &parent) const
        {
            if (!hasIndex(row, column, parent)) { return QModelIndex(); }

            ActionItem *parentItem;
            if (!parent.isValid()) { parentItem = m_rootItem.data(); }
            else { parentItem = static_cast<ActionItem *>(parent.internalPointer()); }

            ActionItem *childItem = parentItem->getChildByRow(row);
            if (childItem) { return createIndex(row, column, childItem); }
            else { return {}; }
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
