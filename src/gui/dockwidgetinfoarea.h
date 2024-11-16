// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_DOCKWIDGETINFOAREA_H
#define SWIFT_GUI_DOCKWIDGETINFOAREA_H

#include <QList>
#include <QObject>

#include "gui/dockwidget.h"
#include "gui/swiftguiexport.h"

class QMenu;

namespace swift::gui
{
    //! Forward declarations
    class CInfoArea;
    class CEnableForDockWidgetInfoArea;

    //! Specialized class for dock widgets serving as info area.
    //! \sa CDockWidgetInfoBar
    //! \sa CInfoArea
    class SWIFT_GUI_EXPORT CDockWidgetInfoArea : public CDockWidget
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDockWidgetInfoArea(QWidget *parent = nullptr);

        //! The parent info area
        const CInfoArea *getParentInfoArea() const;

        //! The parent info area
        CInfoArea *getParentInfoArea();

        //! Is this the selected widget, means it is not floating, and it is the one selected
        bool isSelectedDockWidget() const;

        //! Visible widget
        bool isVisibleWidget() const;

        //! Contribute to menu
        //! \remarks made public so other classes can nest this submenu
        virtual void addToContextMenu(QMenu *contextMenu) const override;

        //! \copydoc CDockWidget::initialFloating
        virtual void initialFloating() override;

    private:
        //! Find all embedded dock widget components. These are components marked as CDockWidgetInfoAreaComponent
        //! \remarks Only directly embedded info area components, not those of nested info areas
        QList<CEnableForDockWidgetInfoArea *> findEmbeddedDockWidgetInfoAreaComponents();

        //! Nested info areas
        QList<CDockWidgetInfoArea *> findNestedInfoAreas();

        //! The parent info areas
        QList<const CInfoArea *> findParentInfoAreas() const;
    };
} // namespace swift::gui

#endif // guard
