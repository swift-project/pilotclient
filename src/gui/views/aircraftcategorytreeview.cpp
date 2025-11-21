// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/views/aircraftcategorytreeview.h"

#include <QAction>
#include <QMenu>
#include <QModelIndex>

#include "gui/menus/menuaction.h"
#include "gui/models/aircraftcategorytreemodel.h"
#include "misc/icons.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::gui::models;

namespace swift::gui::views
{
    CAircraftCategoryTreeView::CAircraftCategoryTreeView(QWidget *parent) : QTreeView(parent)
    {
        this->setModel(new CAircraftCategoryTreeModel(this));
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, &CAircraftCategoryTreeView::customContextMenuRequested, this,
                &CAircraftCategoryTreeView::customMenu);
        connect(this, &CAircraftCategoryTreeView::expanded, this, &CAircraftCategoryTreeView::onExpanded,
                Qt::QueuedConnection);
    }

    void CAircraftCategoryTreeView::updateContainer(const CAircraftCategoryList &categories)
    {
        if (!this->categoryModel()) { return; }
        this->categoryModel()->updateContainer(categories);

        //! \fixme 2019-02 workaround for HEAP: Free Heap block 000001AB439BFFF0 modified at 000001AB439C00BC after it
        //! was freed
        // using Qt::QueuedConnection seems to fix for expand all
        // also this->expandToDepth(0) seems to work
        this->expandAll();
    }

    void CAircraftCategoryTreeView::clear()
    {
        if (!this->categoryModel()) { return; }
        this->categoryModel()->clear();
    }

    void CAircraftCategoryTreeView::fullResizeToContents() { m_dsFullResize.inputSignal(); }

    void CAircraftCategoryTreeView::setColumns(const CColumns &columns)
    {
        if (this->categoryModel()) { this->categoryModel()->setColumns(columns); }
    }

    void CAircraftCategoryTreeView::fullResizeToContentsImpl()
    {
        if (this->isEmpty()) { return; }
        const int cc = this->categoryModel()->columnCount();
        for (int c = 0; c < cc; c++) { this->resizeColumnToContents(c); }
    }

    bool CAircraftCategoryTreeView::isEmpty() const { return this->categoryModel()->rowCount() < 1; }

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
        const CAircraftCategoryTreeModel *model = this->categoryModel();
        if (!model) { return {}; }
        return model->container().frontOrDefault();
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

        auto *menu = new QMenu(this); // menu
        auto *resize = new QAction(CIcons::resize16(), "Resize", this);
        connect(resize, &QAction::triggered, this, &CAircraftCategoryTreeView::fullResizeToContentsImpl);

        menu->addAction(resize);
        menu->popup(this->viewport()->mapToGlobal(point));
    }
} // namespace swift::gui::views
