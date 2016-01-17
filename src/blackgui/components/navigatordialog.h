/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_NAVIGATORDIALOG_H
#define BLACKGUI_COMPONENTS_NAVIGATORDIALOG_H

#include "blackgui/blackguiexport.h"
#include "blackgui/enableforframelesswindow.h"
#include <QDialog>
#include <QScopedPointer>
#include <QMenu>

namespace Ui { class CNavigatorDialog; }

namespace BlackGui
{
    namespace Components
    {

        /*!
         * Navigators dialog
         */
        class BLACKGUI_EXPORT CNavigatorDialog :
                public QDialog,
                public CEnableForFramelessWindow
        {
            Q_OBJECT

        public:
            //! Constructor
            CNavigatorDialog(QWidget *parent = nullptr);

            //! Destructor
            ~CNavigatorDialog();

            //! Navigator
            void buildNavigator(int columns);

            //! Toggle frameless mode
            void toggleFrameless();

        protected:
            //! Style sheet has changed
            void ps_onStyleSheetsChanged();

            //! \copydoc QMainWindow::mouseMoveEvent
            virtual void mouseMoveEvent(QMouseEvent *event) override;

            //! \copydoc QMainWindow::mousePressEvent
            virtual void mousePressEvent(QMouseEvent *event) override;

        private slots:
            //! Context menu
            void ps_showContextMenu(const QPoint &pos);

            //! Change the layout
            void ps_changeLayout();

        private:
            //! Insert own actions
            void insertOwnActions();

            //! Contribute to menu
            void addToContextMenu(QMenu *contextMenu) const;

            //! How many columns for given rows
            int columnsForRows(int rows);

            QScopedPointer<Ui::CNavigatorDialog> ui;
            bool m_firstBuild = true;
        };

    } // ns
} // ns

#endif // guard
