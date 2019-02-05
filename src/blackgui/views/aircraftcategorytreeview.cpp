/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/views/aircraftcategorytreeview.h"
#include "blackgui/models/aircraftcategorytreemodel.h"
#include "blackgui/menus/menuaction.h"
#include "blackmisc/icons.h"
#include "blackconfig/buildconfig.h"

#include <QFlags>
#include <QMenu>
#include <QtGlobal>
#include <QAction>
#include <QModelIndex>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Views
    {
        CAircraftCategoryTreeView::CAircraftCategoryTreeView(QWidget *parent) : QTreeView(parent)
        {
            this->setModel(new CAircraftCategoryTreeModel(this));
            this->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(this, &CAircraftCategoryTreeView::customContextMenuRequested, this, &CAircraftCategoryTreeView::customMenu);
            connect(this, &CAircraftCategoryTreeView::expanded, this, &CAircraftCategoryTreeView::onExpanded);
        }

        void CAircraftCategoryTreeView::updateContainer(const CAircraftCategoryList &categories)
        {
            if (!this->categoryModel()) { return; }
            this->categoryModel()->updateContainer(categories);
            if (!this->isEmpty()) { this->expandAll(); }
        }

        void CAircraftCategoryTreeView::clear()
        {
            if (!this->categoryModel()) { return; }
            this->categoryModel()->clear();
        }

        void CAircraftCategoryTreeView::fullResizeToContents()
        {
            m_dsFullResize.inputSignal();
        }

        void CAircraftCategoryTreeView::setColumns(const CColumns &columns)
        {
            if (this->categoryModel()) { this->categoryModel()->setColumns(columns); }
        }

        void CAircraftCategoryTreeView::fullResizeToContentsImpl()
        {
            if (this->isEmpty()) { return; }
            const int cc = this->categoryModel()->columnCount();
            for (int c = 0; c < cc; c++)
            {
                this->resizeColumnToContents(c);
            }
        }

        bool CAircraftCategoryTreeView::isEmpty() const
        {
            return this->categoryModel()->rowCount() < 1;
        }

        const CAircraftCategoryTreeModel *CAircraftCategoryTreeView::categoryModel() const
        {
            return qobject_cast<const CAircraftCategoryTreeModel *>(this->model());
        }

        CAircraftCategoryTreeModel *CAircraftCategoryTreeView::categoryModel()
        {
            return qobject_cast<CAircraftCategoryTreeModel *>(this->model());
        }

        CAircraftCategory CAircraftCategoryTreeView::selectedObject() const
        {
            const QModelIndex index = this->currentIndex();
            const QVariant data = this->model()->data(index.siblingAtColumn(0)); // supposed to be the callsign
            const CAircraftCategoryTreeModel *model = this->categoryModel();
            if (!model) { return CAircraftCategory(); }
            return model->container().frontOrDefault();
        }

        QString CAircraftCategoryTreeView::suffixForIndex(const QModelIndex &index)
        {
            const QVariant data = this->model()->data(index); // supposed to be the suffix
            return data.toString();
        }

        void CAircraftCategoryTreeView::onExpanded(const QModelIndex &index)
        {
            Q_UNUSED(index);
            this->fullResizeToContents();
        }

        void CAircraftCategoryTreeView::customMenu(const QPoint &point)
        {
            if (!this->categoryModel()) { return; }
            if (this->categoryModel()->container().isEmpty()) { return; }

            QMenu *menu = new QMenu(this);  // menu
            QAction *resize = new QAction(CIcons::resize16(), "Resize", this);
            connect(resize, &QAction::triggered, this, &CAircraftCategoryTreeView::fullResizeToContentsImpl);

            menu->addAction(resize);
            menu->popup(this->viewport()->mapToGlobal(point));
        }
    } // namespace
} // namespace
