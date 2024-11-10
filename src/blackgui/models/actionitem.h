// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_ACTIONITEM_H
#define BLACKGUI_MODELS_ACTIONITEM_H

#include "misc/icons.h"
#include <QList>
#include <QString>
#include <QPixmap>

namespace BlackGui::Models
{
    //! One single action item in a tree
    class CActionItem
    {
    public:
        //! Constructor
        CActionItem(const QString &action, const QString &name, CActionItem *parentItem = nullptr);

        //! Constructor
        CActionItem(const QString &action, const QString &name, const QPixmap &icon, CActionItem *parentItem = nullptr);

        //! Constructor
        CActionItem(const QString &action, const QString &name, swift::misc::CIcons::IconIndex icon, CActionItem *parentItem = nullptr);

        //! Destructor
        ~CActionItem();

        //! @{
        //! Not copyable
        CActionItem(const CActionItem &) = delete;
        CActionItem &operator=(const CActionItem &) = delete;
        //! @}

        //! Append a new child
        void appendChild(CActionItem *child);

        //! Find child by its name
        CActionItem *findChildByName(const QString &name) const;

        //! Get child by row
        CActionItem *getChildByRow(int row) const;

        //! Number of children
        int getChildCount() const;

        //! Icon
        const QPixmap &getIcon() const { return m_icon; }

        //! Set icon
        void setIcon(const QPixmap &icon) { m_icon = icon; }

        //! Has children?
        bool hasChildren() const;

        //! Number of columns
        int getColumnCount() const;

        //! Returns the stored action
        QString getAction() const;

        //! Get action name
        QString getActionName() const;

        //! Get row of this item
        int getRow() const;

        //! Get parent item
        CActionItem *getParentItem() const;

    private:
        QString m_action;
        QString m_actionName;
        QPixmap m_icon;
        QList<CActionItem *> m_childItems;
        CActionItem *m_parentItem = nullptr;
    };
} // ns

#endif // guard
