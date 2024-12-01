// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_ENABLEFORDOCKWIDGETINFOAREA_H
#define SWIFT_GUI_ENABLEFORDOCKWIDGETINFOAREA_H

#include "gui/swiftguiexport.h"
#include "misc/connectionguard.h"

class QWidget;

namespace swift::gui
{
    class CDockWidgetInfoArea;
    class CEnableForFramelessWindow;
    class CInfoArea;

    //! Helper class: If a component is residing in an dockable widget.
    //! This class provides access to its info area and dockable widget.
    class SWIFT_GUI_EXPORT CEnableForDockWidgetInfoArea
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

        CDockWidgetInfoArea *m_parentDockableInfoArea = nullptr; //!< my parent dockable widget
        swift::misc::CConnectionGuard m_connections; //!< connections
    };
} // namespace swift::gui

#endif // SWIFT_GUI_ENABLEFORDOCKWIDGETINFOAREA_H
