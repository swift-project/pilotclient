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

#include "blackgui/dockwidgetinfoarea.h"
#include "blackgui/infoarea.h"
#include "blackgui/enableforframelesswindow.h"
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
            virtual bool setParentDockWidgetInfoArea(BlackGui::CDockWidgetInfoArea *parentDockableWidget);

            //! The parent info area
            const CInfoArea *getParentInfoArea() const;

            //! The parent info area
            CInfoArea *getParentInfoArea();

            //! Is the parent dockable widget floating?
            bool isParentDockWidgetFloating() const;

            //! \copydoc CDockWidgetInfoArea::isVisibleWidget
            bool isVisibleWidget() const;

            //! Main application window if any
            BlackGui::CEnableForFramelessWindow *mainApplicationWindow() const;

            //! Main application window widget if any
            QWidget *mainApplicationWindowWidget() const;

            //! Display myself
            void displayMyself();

        protected:
            //! Constructor
            //! \remarks Normally the infoa area will be provided later \sa setParentDockWidgetInfoArea
            CEnableForDockWidgetInfoArea(CDockWidgetInfoArea *parentInfoArea = nullptr);

        private:
            BlackGui::CDockWidgetInfoArea *m_parentDockableInfoArea = nullptr; //!< my parent dockable widget

        };
    }
} // namespace

#endif // guard
