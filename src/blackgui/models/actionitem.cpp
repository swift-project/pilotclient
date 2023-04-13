/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/actionitem.h"
#include <QtAlgorithms>

using namespace BlackMisc;

namespace BlackGui::Models
{
    CActionItem::CActionItem(const QString &action, const QString &name, CActionItem *parentItem) : m_action(action), m_actionName(name), m_parentItem(parentItem)
    {}

    CActionItem::CActionItem(const QString &action, const QString &name, const QPixmap &icon, CActionItem *parentItem) : m_action(action), m_actionName(name), m_icon(icon), m_parentItem(parentItem)
    {}

    CActionItem::CActionItem(const QString &action, const QString &name, CIcons::IconIndex icon, CActionItem *parentItem) : m_action(action), m_actionName(name), m_icon(CIcons::pixmapByIndex(icon)), m_parentItem(parentItem)
    {}

    CActionItem::~CActionItem()
    {
        qDeleteAll(m_childItems);
    }

    void CActionItem::appendChild(CActionItem *item)
    {
        m_childItems.append(item);
    }

    CActionItem *CActionItem::findChildByName(const QString &name) const
    {
        for (auto child : m_childItems)
        {
            if (child->getActionName() == name) return child;
        }
        return nullptr;
    }

    CActionItem *CActionItem::getChildByRow(int row) const
    {
        return m_childItems.value(row);
    }

    int CActionItem::getChildCount() const
    {
        return m_childItems.count();
    }

    bool CActionItem::hasChildren() const
    {
        return getChildCount() > 0;
    }

    int CActionItem::getColumnCount() const
    {
        return 1;
    }

    QString CActionItem::getAction() const
    {
        return m_action;
    }

    QString CActionItem::getActionName() const
    {
        return m_actionName;
    }

    CActionItem *CActionItem::getParentItem() const
    {
        return m_parentItem;
    }

    int CActionItem::getRow() const
    {
        if (m_parentItem) { return m_parentItem->m_childItems.indexOf(const_cast<CActionItem *>(this)); }
        return 0;
    }
}
