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

#include <QList>
#include <QString>

namespace BlackGui
{
    namespace Models
    {
        //! One single action item in a tree
        class CActionItem
        {
        public:
            //! Constructor
            CActionItem(const QString &action, const QString &name, CActionItem *parentItem = nullptr);

            //! Destructor
            ~CActionItem();

            //! Append a new child
            void appendChild(CActionItem *child);

            //! Find child by its name
            CActionItem *findChildByName(const QString &name) const;

            //! Get child by row
            CActionItem *getChildByRow(int row) const;

            //! Number of children
            int getChildCount() const;

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
            QList<CActionItem *> m_childItems;
            QString m_action;
            QString m_actionName;
            CActionItem *m_parentItem = nullptr;
        };
    }
} // ns

#endif // guard
