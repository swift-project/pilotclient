/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "viewbase.h"
#include "blackgui/models/allmodels.h"
#include "blackgui/stylesheetutility.h"
#include "blackgui/guiutility.h"
#include "blackgui/shortcut.h"
#include "blackcore/registermetadata.h"

#include <QHeaderView>
#include <QModelIndex>
#include <QTime>
#include <QAction>
#include <QSortFilterProxyModel>
#include <QDialog>
#include <QLabel>
#include <QMovie>
#include <QPainter>
#include <QShortcut>

using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackGui::Models;
using namespace BlackGui::Filters;

namespace BlackGui
{
    namespace Views
    {
        CViewBaseNonTemplate::CViewBaseNonTemplate(QWidget *parent) :
            QTableView(parent)
        {
            this->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(this, &QWidget::customContextMenuRequested, this, &CViewBaseNonTemplate::ps_customMenuRequested);
            connect(this, &QTableView::clicked, this, &CViewBaseNonTemplate::ps_clicked);
            connect(this, &QTableView::doubleClicked, this, &CViewBaseNonTemplate::ps_doubleClicked);

            // scroll modes
            this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
            this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
            this->setWordWrap(true);

            QShortcut *filter = new QShortcut(CShortcut::keyDisplayFilter(), this, SLOT(ps_displayFilterDialog()), nullptr, Qt::WidgetShortcut);
            filter->setObjectName("Filter shortcut for " + this->objectName());

            QShortcut *clearSelection = new QShortcut(CShortcut::keyClearSelection(), this, SLOT(clearSelection()), nullptr, Qt::WidgetShortcut);
            clearSelection->setObjectName("Cleat selection shortcut for " + this->objectName());
        }

        bool CViewBaseNonTemplate::setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget)
        {
            bool c = CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea(parentDockableWidget);
            return c;
        }

        void CViewBaseNonTemplate::resizeToContents()
        {
            this->performModeBasedResizeToContent();
        }

        void CViewBaseNonTemplate::setFilterDialog(CFilterDialog *filterDialog)
        {
            if (filterDialog)
            {
                this->m_withMenuFilter = true;
                this->m_filterWidget = filterDialog;
                connect(filterDialog, &CFilterDialog::finished, this, &CViewBaseNonTemplate::ps_filterDialogFinished);
            }
            else
            {
                if (this->m_filterWidget) { disconnect(this->m_filterWidget); }
                this->m_withMenuFilter = false;
                this->m_filterWidget->deleteLater();
                this->m_filterWidget = nullptr;
            }
        }

        void CViewBaseNonTemplate::setFilterWidget(CFilterWidget *filterWidget)
        {
            if (this->m_filterWidget)
            {
                disconnect(this->m_filterWidget);
                m_filterWidget = nullptr;
            }

            if (filterWidget)
            {
                this->m_withMenuFilter = false;
                this->m_filterWidget = filterWidget;
                bool s = connect(filterWidget, &CFilterWidget::changeFilter, this, &CViewBaseNonTemplate::ps_filterWidgetChangedFilter);
                Q_ASSERT_X(s, Q_FUNC_INFO, "filter connect");
                s = connect(this, &CViewBaseNonTemplate::rowCountChanged, filterWidget, &CFilterWidget::onRowCountChanged);
                Q_ASSERT_X(s, Q_FUNC_INFO, "filter connect");
                Q_UNUSED(s);
            }
        }

        void CViewBaseNonTemplate::enableLoadIndicator(bool enable)
        {
            m_enabledLoadIndicator = enable;
        }

        bool CViewBaseNonTemplate::isShowingLoadIndicator() const
        {
            return m_enabledLoadIndicator && m_showingLoadIndicator;
        }

        void CViewBaseNonTemplate::setSelectionModel(QItemSelectionModel *model)
        {
            if (this->selectionModel()) { disconnect(this->selectionModel()); }
            QTableView::setSelectionModel(model);
            if (this->selectionModel())
            {
                connect(this->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &CViewBaseNonTemplate::ps_rowSelected);
            }
        }

        QWidget *CViewBaseNonTemplate::mainApplicationWindowWidget() const
        {
            return CGuiUtility::mainApplicationWindowWidget();
        }

        void CViewBaseNonTemplate::setCustomMenu(IMenuDelegate *menu, bool nestPreviousMenu)
        {
            if (menu && nestPreviousMenu)
            {
                // new menu with nesting
                menu->setNestedDelegate(this->m_menu);
                m_menu =  menu;
            }
            else if (!menu && nestPreviousMenu)
            {
                // nested new menu
                m_menu = m_menu->getNestedDelegate();
            }
            else
            {
                // no nesting
                m_menu = menu;
            }
        }

        void CViewBaseNonTemplate::customMenu(QMenu &menu) const
        {
            // delegate?
            if (this->m_menu) { this->m_menu->customMenu(menu); }

            // standard menus
            bool withStandardMenu = this->m_withMenuItemRefresh || this->m_withMenuItemBackend || this->m_withMenuItemClear || this->m_withMenuDisplayAutomatically;
            if (this->m_withMenuItemRefresh) { menu.addAction(BlackMisc::CIcons::refresh16(), "Update", this, SIGNAL(requestUpdate())); }
            if (this->m_withMenuItemBackend) { menu.addAction(BlackMisc::CIcons::refresh16(), "Reload from backend", this, SIGNAL(requestNewBackendData())); }
            if (this->m_withMenuItemClear) { menu.addAction(BlackMisc::CIcons::delete16(), "Clear", this, SLOT(ps_clear())); }
            if (this->m_withMenuDisplayAutomatically)
            {
                QAction *a = menu.addAction(CIcons::appMappings16(), "Automatically display (when loaded)", this, SLOT(ps_toggleAutoDisplay()));
                a->setCheckable(true);
                a->setChecked(this->displayAutomatically());
            }
            if (withStandardMenu) { menu.addSeparator(); }

            if (this->m_withMenuFilter)
            {
                menu.addAction(BlackMisc::CIcons::tableSheet16(), "Filter", this, SLOT(ps_displayFilterDialog()), CShortcut::keyDisplayFilter());
                menu.addAction(BlackMisc::CIcons::tableSheet16(), "Remove Filter", this, SLOT(ps_removeFilter()));
            }
            if (!menu.isEmpty()) { menu.addSeparator(); }

            // selection menus
            SelectionMode sm = this->selectionMode();
            if (sm == MultiSelection || sm == ExtendedSelection)
            {
                menu.addAction(BlackMisc::CIcons::empty16(), "Select all", this, SLOT(selectAll()), Qt::CTRL + Qt::Key_A);
            }
            menu.addAction(BlackMisc::CIcons::empty16(), "Clear selection", this, SLOT(clearSelection()), CShortcut::keyClearSelection());
            if (!menu.isEmpty()) { menu.addSeparator(); }

            // resizing
            menu.addAction(BlackMisc::CIcons::resize16(), "Full resize", this, SLOT(fullResizeToContents()));
            if (m_rowResizeMode == Interactive)
            {
                menu.addAction(BlackMisc::CIcons::resizeVertical16(), "Resize rows to content", this, SLOT(rowsResizeModeToContent()));
            }
            else
            {
                menu.addAction(BlackMisc::CIcons::resizeVertical16(), "Resize rows interactive", this, SLOT(rowsResizeModeToInteractive()));
            }

            // resize to content might decrease performance,
            // so I only allow changing to "content resizing" if size matches
            bool enabled = !this->reachedResizeThreshold();
            QAction *actionInteractiveResize = new QAction(&menu);
            actionInteractiveResize->setObjectName(this->objectName().append("ActionResizing"));
            actionInteractiveResize->setIconText("Resize (auto)");
            actionInteractiveResize->setIcon(CIcons::viewMultiColumn());
            actionInteractiveResize->setCheckable(true);
            actionInteractiveResize->setChecked(this->m_resizeMode == ResizingAuto);
            actionInteractiveResize->setEnabled(enabled);
            menu.addAction(actionInteractiveResize);
            connect(actionInteractiveResize, &QAction::toggled, this, &CViewBaseNonTemplate::ps_toggleResizeMode);

            if (m_showingLoadIndicator)
            {
                // just in case, if this ever will be dangling
                menu.addAction(BlackMisc::CIcons::preloader16(), "Hide load indicator", this, SLOT(hideLoadIndicator()));
            }
        }

        void CViewBaseNonTemplate::paintEvent(QPaintEvent *event)
        {
            QTableView::paintEvent(event);
            // CStyleSheetUtility::useStyleSheetInDerivedWidget(this, QStyle::PE_Widget);
        }

        void CViewBaseNonTemplate::showEvent(QShowEvent *event)
        {
            if (this->isShowingLoadIndicator())
            {
                // re-center
                this->centerLoadIndicator();
            }
            QTableView::showEvent(event);
        }

        void CViewBaseNonTemplate::allowDragDropValueObjects(bool allowDrag, bool allowDrop)
        {
            // see model for implementing logic of drag
            this->setAcceptDrops(allowDrop);
            this->setDragEnabled(allowDrag);
            this->setDropIndicatorShown(allowDrop);
            CDropBase::allowDrop(allowDrop);
        }

        void CViewBaseNonTemplate::allowDrop(bool allow)
        {
            this->allowDragDropValueObjects(this->dragEnabled(), allow);
        }

        bool CViewBaseNonTemplate::isDropAllowed() const
        {
            return this->acceptDrops();
        }

        int CViewBaseNonTemplate::getHorizontalHeaderFontHeight() const
        {
            QFontMetrics m(this->getHorizontalHeaderFont());
            int h = m.height();
            return h;
        }

        bool CViewBaseNonTemplate::hasSelection() const
        {
            return this->selectionModel()->hasSelection();
        }

        QModelIndexList CViewBaseNonTemplate::selectedRows() const
        {
            return this->selectionModel()->selectedRows();
        }

        int CViewBaseNonTemplate::selectedRowsCount() const
        {
            if (!this->hasSelection()) { return 0;}
            return this->selectedRows().count();
        }

        bool CViewBaseNonTemplate::hasSingleSelectedRow() const
        {
            return this->selectedRowsCount() == 1;
        }

        bool CViewBaseNonTemplate::hasMultipleSelectedRows() const
        {
            return this->selectedRowsCount() > 1;
        }

        void CViewBaseNonTemplate::init()
        {
            int fh = qRound(1.5 * this->getHorizontalHeaderFontHeight());
            this->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive); // faster mode
            this->horizontalHeader()->setStretchLastSection(true);
            this->verticalHeader()->setDefaultSectionSize(fh); // for height
            this->verticalHeader()->setMinimumSectionSize(fh); // for height

            switch (this->m_rowResizeMode)
            {
            case Interactive: this->rowsResizeModeToInteractive(); break;
            case Content: this->rowsResizeModeToContent(); break;
            default:
                Q_ASSERT_X(false, Q_FUNC_INFO, "wrong resize mode");
                break;
            }
        }

        QString CViewBaseNonTemplate::getDefaultFilename() const
        {
            // some logic to find a useful default name
            QStringList pathes(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation));
            QString name;
            if (getDockWidgetInfoArea()) { name = getDockWidgetInfoArea()->windowTitle(); }
            else if (!windowTitle().isEmpty()) { name = windowType(); }
            else { name = this->metaObject()->className(); }
            name += ".json";

            if (!pathes.isEmpty())
            {
                QString p(CFileUtils::appendFilePaths(pathes.first(), "swift/" + name));
                if (!QDir(p).exists())
                {
                    p = CFileUtils::appendFilePaths(pathes.first(), "swift " + name);
                }
                return p;
            }
            else
            {
                return name;
            }
        }

        void CViewBaseNonTemplate::menuRemoveItems(Menu menusToRemove)
        {
            this->m_menus &= (~menusToRemove);
        }

        void CViewBaseNonTemplate::menuAddItems(Menu menusToAdd)
        {
            this->m_menus |= menusToAdd;
        }

        int CViewBaseNonTemplate::ps_updateContainer(const CVariant &variant, bool sort, bool resize)
        {
            return this->performUpdateContainer(variant, sort, resize);
        }

        void CViewBaseNonTemplate::ps_displayFilterDialog()
        {
            if (!this->m_withMenuFilter) { return; }
            if (!this->m_filterWidget) { return; }
            this->m_filterWidget->show();
        }

        void CViewBaseNonTemplate::rowsResizeModeToInteractive()
        {
            const int height = this->verticalHeader()->minimumSectionSize();
            QHeaderView *verticalHeader = this->verticalHeader();
            Q_ASSERT_X(verticalHeader, Q_FUNC_INFO, "Missing vertical header");
            verticalHeader->setSectionResizeMode(QHeaderView::Interactive);
            verticalHeader->setDefaultSectionSize(height);
            this->m_rowResizeMode = Interactive;
        }

        void CViewBaseNonTemplate::rowsResizeModeToContent()
        {
            QHeaderView *verticalHeader = this->verticalHeader();
            Q_ASSERT(verticalHeader);
            verticalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
            this->m_rowResizeMode = Content;
        }

        void CViewBaseNonTemplate::showLoadIndicator(int containerSizeDependent)
        {
            if (!m_enabledLoadIndicator) { return; }
            if (this->m_showingLoadIndicator) { return; }
            if (this->hasDockWidgetArea())
            {
                if (!this->isVisibleWidget()) { return; }
            }

            if (containerSizeDependent >= 0)
            {
                // really with indicator?
                if (containerSizeDependent < ResizeSubsetThreshold) { return; }
            }
            this->m_showingLoadIndicator = true;
            emit loadIndicatorVisibilityChanged(this->m_showingLoadIndicator);
            // this->setStyleSheet(styleSheet());

            if (!this->m_loadIndicator)
            {
                this->m_loadIndicator = new CLoadIndicator(64, 64, this);
            }
            this->centerLoadIndicator();
            this->m_loadIndicator->startAnimation();
        }

        void CViewBaseNonTemplate::centerLoadIndicator()
        {
            if (!m_loadIndicator) { return; }
            QPoint middle = this->viewport()->geometry().center();
            int w = m_loadIndicator->width();
            int h = m_loadIndicator->height();
            int x = middle.x() - w / 2;
            int y = middle.y() - h / 2;
            this->m_loadIndicator->setGeometry(x, y, w, h);
        }

        void CViewBaseNonTemplate::hideLoadIndicator()
        {
            if (!this->m_showingLoadIndicator) { return; }
            this->m_showingLoadIndicator = false;
            emit loadIndicatorVisibilityChanged(this->m_showingLoadIndicator);
            if (!this->m_loadIndicator) { return; }
            this->m_loadIndicator->stopAnimation();
        }

        bool CViewBaseNonTemplate::isResizeConditionMet(int containerSize) const
        {
            if (m_resizeMode == ResizingOnceSubset) { return false; }
            if (m_resizeMode == ResizingOff) { return false; }
            if (m_resizeMode == ResizingOnce) { return m_resizeCount < 1; }
            if (m_resizeMode == ResizingAuto)
            {
                if (reachedResizeThreshold(containerSize)) { return false; }
                if (m_resizeAutoNthTime < 2)  { return true; }
                return (m_resizeCount % m_resizeAutoNthTime) == 0;
            }
            return false;
        }

        void CViewBaseNonTemplate::fullResizeToContents()
        {
            m_resizeCount++;
            this->resizeColumnsToContents(); // columns
            this->resizeRowsToContents(); // rows
            if (m_forceStretchLastColumnWhenResized)
            {
                // re-stretch
                this->horizontalHeader()->setStretchLastSection(true);
            }
        }

        void CViewBaseNonTemplate::ps_customMenuRequested(QPoint pos)
        {
            QMenu menu;
            this->customMenu(menu);
            if (menu.isEmpty()) { return; }

            QPoint globalPos = this->mapToGlobal(pos);
            menu.exec(globalPos);
        }

        void CViewBaseNonTemplate::ps_toggleResizeMode(bool checked)
        {
            if (checked)
            {
                this->m_resizeMode = ResizingAuto;
            }
            else
            {
                this->m_resizeMode = ResizingOff;
            }
        }

        void CViewBaseNonTemplate::ps_toggleAutoDisplay()
        {
            QAction *a = qobject_cast<QAction *>(QObject::sender());
            if (!a) { return; }
            Q_ASSERT_X(a->isCheckable(), Q_FUNC_INFO, "object not checkable");
            this->m_displayAutomatically = a->isChecked();
        }

        void CViewBaseNonTemplate::ps_updatedIndicator()
        {
            this->update();
        }

        void CViewBaseNonTemplate::dragEnterEvent(QDragEnterEvent *event)
        {
            if (!event || !acceptDrop(event->mimeData())) { return; }
            setBackgroundRole(QPalette::Highlight);
            event->acceptProposedAction();
        }

        void CViewBaseNonTemplate::dragMoveEvent(QDragMoveEvent *event)
        {
            if (!event || !acceptDrop(event->mimeData())) { return; }
            event->acceptProposedAction();
        }

        void CViewBaseNonTemplate::dragLeaveEvent(QDragLeaveEvent *event)
        {
            if (!event) { return; }
            event->accept();
        }

        void CViewBaseNonTemplate::dropEvent(QDropEvent *event)
        {
            Q_UNUSED(event);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        CViewBase<ModelClass, ContainerType, ObjectType>::CViewBase(QWidget *parent, ModelClass *model) : CViewBaseNonTemplate(parent), m_model(model)
        {
            this->setSortingEnabled(true);
            if (model)
            {
                this->setModel(this->m_model);
            }
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        int CViewBase<ModelClass, ContainerType, ObjectType>::updateContainer(const ContainerType &container, bool sort, bool resize)
        {
            Q_ASSERT_X(this->m_model, Q_FUNC_INFO, "Missing model");
            this->showLoadIndicator(container.size());
            bool reallyResize = resize && isResizeConditionMet(container.size()); // do we really perform resizing
            bool presize = (m_resizeMode == ResizingOnceSubset) &&
                           this->isEmpty() && // only when no data yet
                           !reallyResize;     // not when we resize later
            presize = presize || (this->isEmpty() && resize && !reallyResize); // we presize if we wanted to resize but actually do not because of condition
            bool presizeThreshold = presize && container.size() > ResizeSubsetThreshold; // only when size making sense

            // when we will not resize, we might presize
            if (presizeThreshold)
            {
                int presizeRandomElements = container.size() / 100;
                this->m_model->update(container.randomElements(presizeRandomElements), false);
                this->fullResizeToContents();
            }
            int c = this->m_model->update(container, sort);

            // resize after real update according to mode
            if (presizeThreshold)
            {
                // currently no furhter actions
            }
            else if (reallyResize)
            {
                this->resizeToContents();
            }
            else if (presize && !presizeThreshold)
            {
                // small amount of data not covered before
                this->fullResizeToContents();
            }
            this->hideLoadIndicator();
            return c;
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        BlackMisc::CWorker *CViewBase<ModelClass, ContainerType, ObjectType>::updateContainerAsync(const ContainerType &container, bool sort, bool resize)
        {
            Q_UNUSED(sort);
            ModelClass *model = this->derivedModel();
            auto sortColumn = model->getSortColumn();
            auto sortOrder = model->getSortOrder();
            this->showLoadIndicator(container.size());
            BlackMisc::CWorker *worker = BlackMisc::CWorker::fromTask(this, "ViewSort", [model, container, sortColumn, sortOrder]()
            {
                return model->sortContainerByColumn(container, sortColumn, sortOrder);
            });
            worker->thenWithResult<ContainerType>(this, [this, resize](const ContainerType & sortedContainer)
            {
                this->ps_updateContainer(CVariant::from(sortedContainer), false, resize);
            });
            worker->then(this, &CViewBase::asyncUpdateFinished);
            return worker;
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::updateContainerMaybeAsync(const ContainerType &container, bool sort, bool resize)
        {
            if (container.size() > ASyncRowsCountThreshold && sort)
            {
                // larger container with sorting
                updateContainerAsync(container, sort, resize);
            }
            else
            {
                updateContainer(container, sort, resize);
            }
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::insert(const ObjectType &value, bool resize)
        {
            Q_ASSERT(this->m_model);
            this->m_model->insert(value);
            if (resize) { this->performModeBasedResizeToContent(); }
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        const ObjectType &CViewBase<ModelClass, ContainerType, ObjectType>::at(const QModelIndex &index) const
        {
            Q_ASSERT(this->m_model);
            return this->m_model->at(index);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        const ContainerType &CViewBase<ModelClass, ContainerType, ObjectType>::container() const
        {
            Q_ASSERT(this->m_model);
            return this->m_model->container();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        ContainerType CViewBase<ModelClass, ContainerType, ObjectType>::selectedObjects() const
        {
            if (!this->hasSelection()) { return ContainerType(); }
            ContainerType c;
            QModelIndexList indexes = this->selectedRows();
            for (const QModelIndex &i : indexes)
            {
                c.push_back(this->at(i));
            }
            return c;
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        int CViewBase<ModelClass, ContainerType, ObjectType>::updateSelected(const CVariant &variant, const CPropertyIndex &index)
        {
            if (!hasSelection()) { return 0; }
            QModelIndexList indexes = this->selectedRows();
            int c = 0;
            int lastRow = -1;
            int firstRow = -1;

            for (const QModelIndex &i : indexes)
            {
                if (i.row() == lastRow) { continue; }
                lastRow = i.row();
                if (firstRow < 0 || lastRow < firstRow) { firstRow = lastRow; }
                ObjectType obj(this->at(i));
                obj.setPropertyByIndex(variant, index);
                if (this->derivedModel()->setInContainer(i, obj))
                {
                    c++;
                }
            }

            if (c > 0)
            {
                this->derivedModel()->sendDataChanged(firstRow, lastRow);
            }
            return c;
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        ObjectType CViewBase<ModelClass, ContainerType, ObjectType>::selectedObject() const
        {
            ContainerType c = this->selectedObjects();
            return c.frontOrDefault();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        int CViewBase<ModelClass, ContainerType, ObjectType>::removeSelectedRows()
        {
            if (!this->hasSelection()) { return 0; }
            if (this->isEmpty()) { return 0; }

            int currentRows = this->rowCount();
            if (currentRows == selectedRowsCount())
            {
                this->clear();
                return currentRows;
            }

            ContainerType selected(selectedObjects());
            ContainerType newObjects(container());
            for (const ObjectType &obj : selected)
            {
                newObjects.remove(obj);
            }

            int delta = currentRows - newObjects.size();
            this->updateContainerMaybeAsync(newObjects);
            return delta;
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        int CViewBase<ModelClass, ContainerType, ObjectType>::rowCount() const
        {
            Q_ASSERT(this->m_model);
            return this->m_model->rowCount();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        int CViewBase<ModelClass, ContainerType, ObjectType>::columnCount() const
        {
            Q_ASSERT(this->m_model);
            return this->m_model->columnCount(QModelIndex());
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        bool CViewBase<ModelClass, ContainerType, ObjectType>::isEmpty() const
        {
            Q_ASSERT(this->m_model);
            return this->m_model->rowCount() < 1;
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::setObjectName(const QString &name)
        {
            // then name here is mainly set for debugging purposes so each model can be identified
            Q_ASSERT(m_model);
            QTableView::setObjectName(name);
            QString modelName = QString(name).append(':').append(this->m_model->getTranslationContext());
            this->m_model->setObjectName(modelName);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::takeFilterOwnership(std::unique_ptr<BlackGui::Models::IModelFilter<ContainerType> > &filter)
        {
            this->derivedModel()->takeFilterOwnership(filter);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::removeFilter()
        {
            this->derivedModel()->removeFilter();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        bool CViewBase<ModelClass, ContainerType, ObjectType>::hasFilter() const
        {
            return derivedModel()->hasFilter();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::setSortIndicator()
        {
            if (this->m_model->hasValidSortColumn())
            {
                Q_ASSERT(this->horizontalHeader());
                this->horizontalHeader()->setSortIndicator(
                    this->m_model->getSortColumn(),
                    this->m_model->getSortOrder());
            }
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::standardInit(ModelClass *model)
        {
            Q_ASSERT_X(model || this->m_model, Q_FUNC_INFO, "Missing model");
            if (model)
            {
                this->m_model = model;
                connect(this->m_model, &ModelClass::rowCountChanged, this, &CViewBase::rowCountChanged);
                connect(this->m_model, &ModelClass::objectChanged, this, &CViewBase::objectChanged);
                connect(this->m_model, &ModelClass::changed, this, &CViewBase::modelChanged);
            }

            this->setModel(this->m_model); // via QTableView
            CViewBaseNonTemplate::init();
            this->setSortIndicator();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        bool CViewBase<ModelClass, ContainerType, ObjectType>::reachedResizeThreshold(int containerSize) const
        {
            if (containerSize < 0) { return this->rowCount() > m_skipResizeThreshold; }
            return containerSize > m_skipResizeThreshold;
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::performModeBasedResizeToContent()
        {
            // small set or large set?
            if (this->isResizeConditionMet())
            {
                this->fullResizeToContents();
            }
            else
            {
                this->m_resizeCount++; // skipped resize
            }
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        int CViewBase<ModelClass, ContainerType, ObjectType>::performUpdateContainer(const BlackMisc::CVariant &variant, bool sort, bool resize)
        {
            ContainerType c(variant.to<ContainerType>());
            return this->updateContainer(c, sort, resize);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        bool CViewBase<ModelClass, ContainerType, ObjectType>::ps_filterDialogFinished(int status)
        {
            QDialog::DialogCode statusCode = static_cast<QDialog::DialogCode>(status);
            return ps_filterWidgetChangedFilter(statusCode == QDialog::Accepted);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        bool CViewBase<ModelClass, ContainerType, ObjectType>::ps_filterWidgetChangedFilter(bool enabled)
        {
            if (enabled)
            {
                if (!this->m_filterWidget)
                {
                    this->removeFilter();
                }
                else
                {
                    IModelFilterProvider<ContainerType> *provider = dynamic_cast<IModelFilterProvider<ContainerType>*>(this->m_filterWidget);
                    Q_ASSERT_X(provider, Q_FUNC_INFO, "Filter widget does not provide interface");
                    if (!provider) { return false; }
                    std::unique_ptr<IModelFilter<ContainerType>> f(provider->createModelFilter());
                    this->takeFilterOwnership(f);
                }
            }
            else
            {
                // no filter
                this->removeFilter();
            }
            return true; // handled
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::ps_removeFilter()
        {
            this->derivedModel()->removeFilter();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::ps_clicked(const QModelIndex &index)
        {
            if (!m_acceptClickSelection) { return; }
            if (!index.isValid()) { return; }
            emit objectClicked(CVariant::fromValue(at(index)));
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::ps_doubleClicked(const QModelIndex &index)
        {
            if (!m_acceptDoubleClickSelection) { return; }
            if (!index.isValid()) { return; }
            emit objectDoubleClicked(CVariant::fromValue(at(index)));
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::ps_rowSelected(const QModelIndex &index)
        {
            if (!m_acceptRowSelected) { return; }
            if (!index.isValid()) { return; }
            emit objectSelected(CVariant::fromValue(at(index)));
        }

        // see here for the reason of thess forward instantiations
        // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
        template class CViewBase<BlackGui::Models::CAircraftIcaoCodeListModel, BlackMisc::Aviation::CAircraftIcaoCodeList, BlackMisc::Aviation::CAircraftIcaoCode>;
        template class CViewBase<BlackGui::Models::CAircraftModelListModel, BlackMisc::Simulation::CAircraftModelList, BlackMisc::Simulation::CAircraftModel>;
        template class CViewBase<BlackGui::Models::CAirlineIcaoCodeListModel, BlackMisc::Aviation::CAirlineIcaoCodeList, BlackMisc::Aviation::CAirlineIcaoCode>;
        template class CViewBase<BlackGui::Models::CAirportListModel, BlackMisc::Aviation::CAirportList, BlackMisc::Aviation::CAirport>;
        template class CViewBase<BlackGui::Models::CAtcStationListModel, BlackMisc::Aviation::CAtcStationList, BlackMisc::Aviation::CAtcStation>;
        template class CViewBase<BlackGui::Models::CClientListModel, BlackMisc::Network::CClientList, BlackMisc::Network::CClient>;
        template class CViewBase<BlackGui::Models::CCountryListModel, BlackMisc::CCountryList, BlackMisc::CCountry>;
        template class CViewBase<BlackGui::Models::CDistributorListModel, BlackMisc::Simulation::CDistributorList, BlackMisc::Simulation::CDistributor>;
        template class CViewBase<BlackGui::Models::CIdentifierListModel, BlackMisc::CIdentifierList, BlackMisc::CIdentifier>;
        template class CViewBase<BlackGui::Models::CLiveryListModel, BlackMisc::Aviation::CLiveryList, BlackMisc::Aviation::CLivery>;
        template class CViewBase<BlackGui::Models::CNameVariantPairModel, BlackMisc::CNameVariantPairList, BlackMisc::CNameVariantPair>;
        template class CViewBase<BlackGui::Models::CServerListModel, BlackMisc::Network::CServerList, BlackMisc::Network::CServer>;
        template class CViewBase<BlackGui::Models::CSimulatedAircraftListModel, BlackMisc::Simulation::CSimulatedAircraftList, BlackMisc::Simulation::CSimulatedAircraft>;
        template class CViewBase<BlackGui::Models::CStatusMessageListModel, BlackMisc::CStatusMessageList, BlackMisc::CStatusMessage>;
        template class CViewBase<BlackGui::Models::CTextMessageListModel, BlackMisc::Network::CTextMessageList, BlackMisc::Network::CTextMessage>;
        template class CViewBase<BlackGui::Models::CUserListModel, BlackMisc::Network::CUserList, BlackMisc::Network::CUser>;

    } // namespace
} // namespace
