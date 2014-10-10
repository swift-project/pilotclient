/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_ENABLEFORDOCKWIDGETINFOAREA_H
#define BLACKGUI_ENABLEFORDOCKWIDGETINFOAREA_H

#include "../dockwidgetinfoarea.h"
#include "../infoarea.h"
#include <QWidget>

namespace BlackGui
{
    namespace Components
    {

        //! Helper class: If a component is residing in an dockable widget.
        //! This class provides access to its info area and dockable widget.
        class CEnableForDockWidgetInfoArea
        {
        public:
            //! Corresponding dockable widget in info area
            BlackGui::CDockWidgetInfoArea *getDockWidgetInfoArea() { return m_parentDockableInfoArea; }

            //! Corresponding dockable widget in info area
            const BlackGui::CDockWidgetInfoArea *getDockWidgetInfoArea() const { return m_parentDockableInfoArea; }

            //! Corresponding dockable widget in info area
            //! \remarks Usually set from CDockWidgetInfoArea when it is fully initialized
            bool setParentDockWidgetInfoArea(BlackGui::CDockWidgetInfoArea *parentDockableWidget);

            //! The parent info area
            const CInfoArea *getParentInfoArea() const;

            //! The parent info area
            CInfoArea *getParentInfoArea();

            //! Is the parent dockable widget floating?
            bool isParentDockWidgetFloating() const;

            //! \copydoc CDockWidgetInfoArea::isVisibleWidget
            bool isVisibleWidget() const;

        protected:
            //! Constructor
            CEnableForDockWidgetInfoArea(QWidget *parent);

        private:
            BlackGui::CDockWidgetInfoArea *m_parentDockableInfoArea = nullptr; //!< my parent dockable widget

        };
    }
} // namespace

#endif // guard
