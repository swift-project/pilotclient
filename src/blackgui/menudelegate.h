/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_MENUDELEGATE_H
#define BLACKGUI_MENUDELEGATE_H

#include <QMenu>
#include <QObject>

namespace BlackGui
{
    /*!
     * Interface to implement a custom menu
     */
    class IMenuDelegate : public QObject
    {
        Q_OBJECT

    public:
        //! Display custom menu
        virtual void customMenu(QMenu &menu) const = 0;

        //! Set nested delegate
        void setNestedDelegate(IMenuDelegate *nestedDelegate) { m_nestedDelegate = nestedDelegate; }

        //! Nested delegate
        IMenuDelegate *getNestedDelegate() const { return m_nestedDelegate; }

        //! Destructor
        virtual ~IMenuDelegate() {}

    protected:
        //! Constructor
        IMenuDelegate(QWidget *parent = nullptr, bool separator = false) :
            QObject(parent), m_separator(separator) {}

        //! Delegate down one level
        void nestedCustomMenu(QMenu &menu) const
        {
            if (!m_nestedDelegate) { return; }
            m_nestedDelegate->customMenu(menu);
        }

        //! Add separator
        void addSeparator(QMenu &menu) const
        {
            if (!m_separator || menu.isEmpty()) { return; }
            menu.addSeparator();
        }

        IMenuDelegate *m_nestedDelegate = nullptr; //!< nested delegate if any
        bool           m_separator      = false;   //!< at end, terminate with separator
    };
} // ns

#endif // guard
