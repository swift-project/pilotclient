// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_AIRCRAFTCATEGORYTREEVIEW_H
#define SWIFT_GUI_VIEWS_AIRCRAFTCATEGORYTREEVIEW_H

#include <QMap>
#include <QModelIndex>
#include <QObject>
#include <QPoint>
#include <QTreeView>

#include "gui/swiftguiexport.h"
#include "misc/aviation/aircraftcategorylist.h"
#include "misc/digestsignal.h"

namespace swift::gui
{
    namespace models
    {
        class CAircraftCategoryTreeModel;
        class CColumns;
    } // namespace models

    namespace views
    {
        //! ATC stations view
        class SWIFT_GUI_EXPORT CAircraftCategoryTreeView : public QTreeView
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAircraftCategoryTreeView(QWidget *parent = nullptr);

            //! Update container
            void updateContainer(const swift::misc::aviation::CAircraftCategoryList &categories);

            //! Clear
            void clear();

            //! Resize all columns
            void fullResizeToContents();

            //! Set columns
            void setColumns(const models::CColumns &columns);

            //! Empty data
            bool isEmpty() const;

        private:
            //! Used model
            const models::CAircraftCategoryTreeModel *categoryModel() const;

            //! Used model
            swift::gui::models::CAircraftCategoryTreeModel *categoryModel();

            //! Resize all columns
            void fullResizeToContentsImpl();

            //! The selected object
            swift::misc::aviation::CAircraftCategory selectedObject() const;

            //! Expanded
            void onExpanded(const QModelIndex &index);

            //! Custom menu
            void customMenu(const QPoint &point);

            swift::misc::CDigestSignal m_dsFullResize { this, &CAircraftCategoryTreeView::fullResizeToContentsImpl,
                                                        1000, 25 };
        };
    } // namespace views
} // namespace swift::gui

#endif // guard
