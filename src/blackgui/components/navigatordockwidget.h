/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_NAVIGATORDOCKWIDGET_H
#define BLACKGUI_COMPONENTS_NAVIGATORDOCKWIDGET_H

#include "blackgui/blackguiexport.h"
#include "blackgui/dockwidgetinfoarea.h"
#include <QDockWidget>
#include <QScopedPointer>

namespace Ui { class CNavigatorDockWidget; }

namespace BlackGui
{
    namespace Components
    {

        //! Dock widget for navigators
        class BLACKGUI_EXPORT CNavigatorDockWidget : public BlackGui::CDockWidgetInfoArea
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CNavigatorDockWidget(QWidget *parent = nullptr);

            //! Destructor
            ~CNavigatorDockWidget();

            //! Add action as navigator item
            void addAction(QAction * action);

            //! Add actions as navigator items
            void addActions(QList<QAction *> actions);

            //! Navigator
            void buildNavigator(int columns);

        protected:
            //! \copydoc CDockWidgetInfoArea::addToContextMenu
            virtual void addToContextMenu(QMenu *contextMenu) const override;

        protected:
            //! \copydoc CDockWidget::ps_onStyleSheetsChanged
            virtual void ps_onStyleSheetsChanged() override;

        private slots:
            //! Change the layout
            void ps_changeLayout();

        private:
            QScopedPointer<Ui::CNavigatorDockWidget> ui;
            QList<QWidget *> m_widgets;
            QList<QAction *> m_actions;
            bool m_firstBuild = true;

            //! Insert own actions
            void insertOwnActions();

            //! How many columns for given rows
            int columnsForRows(int rows);

            //! Set widgets to their minimum size
            void setMinimumSizeForWidgets(int rows, int columns);

        };

    } // ns
} // ns

#endif // guard
