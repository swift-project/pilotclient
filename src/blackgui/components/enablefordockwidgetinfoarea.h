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

#include "blackgui/components/textmessagecomponenttab.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/connectionguard.h"

class QWidget;

namespace BlackGui
{
    class CDockWidgetInfoArea;
    class CEnableForFramelessWindow;
    class CInfoArea;

    namespace Components
    {
        //! Helper class: If a component is residing in an dockable widget.
        //! This class provides access to its info area and dockable widget.
        class BLACKGUI_EXPORT CEnableForDockWidgetInfoArea
        {
        public:
            //! Corresponding dockable widget in info area
            CDockWidgetInfoArea *getDockWidgetInfoArea() const { return m_parentDockableInfoArea; }

            //! Has dock area?
            bool hasDockWidgetArea() const { return m_parentDockableInfoArea; }

            //! Corresponding dockable widget in info area
            //! \remarks Usually set from CDockWidgetInfoArea when it is fully initialized
            virtual bool setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget);

            //! The parent info area
            CInfoArea *getParentInfoArea() const;

            //! Is the parent dockable widget floating?
            bool isParentDockWidgetFloating() const;

            //! \copydoc CDockWidgetInfoArea::isVisibleWidget
            bool isVisibleWidget() const;

            //! Main application window if any
            CEnableForFramelessWindow *mainApplicationWindow() const;

            //! Main application window widget if any
            QWidget *mainApplicationWindowWidget() const;

            //! Display myself
            void displayMyself();

        protected:
            //! Constructor
            //! \remarks Normally the info area will be provided later \sa setParentDockWidgetInfoArea
            CEnableForDockWidgetInfoArea(CDockWidgetInfoArea *parentInfoArea = nullptr);

            //! Destructor
            virtual ~CEnableForDockWidgetInfoArea() {}

            //! Deferred activation, as dockwidget is not directly initalized
            void deferredActivate(QObject *relatedObject, int delayMs = 2500);

            //! \copydoc BlackGui::COverlayMessages::initOverlayMessages
            void initOverlayMessages(QSize inner = {});

            //! \copydoc BlackGui::COverlayMessages::showKillButton
            void activateTextMessages(bool activate);

            //! \copydoc BlackGui::COverlayMessages::showOverlayImage
            void showOverlayInlineTextMessage(Components::TextMessageTab tab);

            //! \copydoc BlackGui::COverlayMessages::showOverlayImage
            void showOverlayInlineTextMessage(const BlackMisc::Aviation::CCallsign &callsign);

        private:
            CDockWidgetInfoArea *m_parentDockableInfoArea = nullptr; //!< my parent dockable widget
            BlackMisc::CConnectionGuard m_connections;
        };
    }
} // namespace

#endif // guard
