
#include "blackgui/models/actionitem.h"

#include <QtAlgorithms>

namespace BlackGui
{
    namespace Models
    {

        ActionItem::ActionItem(const QString &action, const QString &name, ActionItem *parent) :
            m_action(action), m_actionName(name), m_parentItem(parent)
        {
        }

        ActionItem::~ActionItem()
        {
            qDeleteAll(m_childItems);
        }

        void ActionItem::appendChild(ActionItem *item)
        {
            m_childItems.append(item);
        }

        ActionItem *ActionItem::findChildByName(const QString &name)
        {
            for (auto child : m_childItems)
            {
                if (child->getActionName() == name) return child;
            }
            return nullptr;
        }

        ActionItem *ActionItem::getChildByRow(int row)
        {
            return m_childItems.value(row);
        }

        int ActionItem::getChildCount() const
        {
            return m_childItems.count();
        }

        int ActionItem::getColumnCount() const
        {
            return 1;
        }

        QString ActionItem::getAction() const
        {
            return m_action;
        }

        QString ActionItem::getActionName() const
        {
            return m_actionName;
        }

        ActionItem *ActionItem::getParentItem()
        {
            return m_parentItem;
        }

        int ActionItem::getRow() const
        {
            if (m_parentItem) { return m_parentItem->m_childItems.indexOf(const_cast<ActionItem *>(this)); }

            return 0;
        }

    }
}
