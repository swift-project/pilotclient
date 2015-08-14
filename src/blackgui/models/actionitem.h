/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_ACTIONITEM_H
#define BLACKGUI_ACTIONITEM_H

#include <QString>
#include <QVariant>

namespace BlackGui
{
    namespace Models
    {

        //! One single action item in a tree
        class ActionItem
        {
        public:
            //! Constructor
            ActionItem(const QString &action, const QString &name, ActionItem *parentItem = nullptr);

            //! Destructor
            ~ActionItem();

            //! Append a new child
            void appendChild(ActionItem *child);

            //! Find child by its name
            ActionItem *findChildByName(const QString &name);

            //! Get child by row
            ActionItem *getChildByRow(int row);

            //! Number of childs
            int getChildCount() const;

            //! Number of columns
            int getColumnCount() const;

            //! Returns the stored action
            QString getAction() const;

            //! Get action name
            QString getActionName() const;

            //! Get row of this item
            int getRow() const;

            //! Get parent item
            ActionItem *getParentItem();

        private:
            QList<ActionItem *> m_childItems;
            QString m_action;
            QString m_actionName;
            ActionItem *m_parentItem;
        };

    }
}

#endif // guard
