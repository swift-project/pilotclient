/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKGUI_MENUS_MENUDELEGATE_H
#define BLACKGUI_MENUS_MENUDELEGATE_H

#include "blackmisc/logcategories.h"
#include "blackgui/menus/menuaction.h"

#include <QMenu>
#include <QObject>

class QWidget;

namespace BlackGui::Menus
{
    /*!
     * Interface to implement a custom menu
     */
    class IMenuDelegate : public QObject
    {
        Q_OBJECT

    public:
        //! Display custom menu
        virtual void customMenu(CMenuActions &menuActions) = 0;

        //! Set nested delegate
        void setNestedDelegate(IMenuDelegate *nestedDelegate) { m_nestedDelegate = nestedDelegate; }

        //! Nested delegate
        IMenuDelegate *getNestedDelegate() const { return m_nestedDelegate; }

        //! Destructor
        virtual ~IMenuDelegate() {}

        //! Log categories
        const QStringList &getLogCategories()
        {
            static const QStringList cats({ BlackMisc::CLogCategories::guiComponent() });
            return cats;
        }

    protected:
        //! Constructor
        IMenuDelegate(QWidget *parent = nullptr) : QObject(parent) {}

        //! Delegate down one level
        void nestedCustomMenu(CMenuActions &menuActions) const
        {
            if (!m_nestedDelegate) { return; }
            m_nestedDelegate->customMenu(menuActions);
        }

        //! Does the previous (menu) item contain string?
        bool previousMenuItemContains(const QString &str, const QMenu &menu, Qt::CaseSensitivity cs = Qt::CaseSensitive) const
        {
            if (menu.isEmpty() || str.isEmpty()) { return false; }
            const QString t(menu.actions().last()->text());
            return t.contains(str, cs);
        }

        IMenuDelegate *m_nestedDelegate = nullptr; //!< nested delegate if any
    };
} // ns

#endif // guard
