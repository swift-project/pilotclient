/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackgui/dockwidgetinfoarea.h"
#include "blackgui/filters/filterdialog.h"
#include "blackgui/filters/filterwidget.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/loadindicator.h"
#include "blackgui/models/allmodels.h"
#include "blackgui/menus/menuaction.h"
#include "blackgui/menus/menudelegate.h"
#include "blackgui/shortcut.h"
#include "blackgui/views/viewbase.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/airport.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/country.h"
#include "blackmisc/countrylist.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/icons.h"
#include "blackmisc/identifier.h"
#include "blackmisc/identifierlist.h"
#include "blackmisc/namevariantpair.h"
#include "blackmisc/namevariantpairlist.h"
#include "blackmisc/network/client.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/network/user.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/propertyindexlist.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/worker.h"

#include <QAction>
#include <QDialog>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QFileDialog>
#include <QFontMetrics>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QList>
#include <QMenu>
#include <QMetaObject>
#include <QMetaType>
#include <QModelIndex>
#include <QPalette>
#include <QPoint>
#include <QRect>
#include <QShortcut>
#include <QVariant>
#include <QWidget>
#include <limits>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackGui::Menus;
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
            this->horizontalHeader()->setSortIndicatorShown(true);

            // scroll modes
            this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
            this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
            this->setWordWrap(true);

            // shortcuts
            QShortcut *filter = new QShortcut(CShortcut::keyDisplayFilter(), this, SLOT(ps_displayFilterDialog()), nullptr, Qt::WidgetShortcut);
            filter->setObjectName("Filter shortcut for " + this->objectName());
            QShortcut *clearSelection = new QShortcut(CShortcut::keyClearSelection(), this, SLOT(clearSelection()), nullptr, Qt::WidgetShortcut);
            clearSelection->setObjectName("Clear selection shortcut for " + this->objectName());
            QShortcut *saveJson = new QShortcut(CShortcut::keySaveViews(), this, SLOT(ps_saveJsonAction()), nullptr, Qt::WidgetShortcut);
            saveJson->setObjectName("Save JSON for " + this->objectName());
            QShortcut *deleteRow = new QShortcut(CShortcut::keyDelete(), this, SLOT(ps_removeSelectedRows()), nullptr, Qt::WidgetShortcut);
            deleteRow->setObjectName("Delete selected rows for " + this->objectName());
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

        void CViewBaseNonTemplate::setFilterWidgetImpl(QWidget *filterWidget)
        {
            // dialog or filter widget
            if (this->m_filterWidget)
            {
                disconnect(this->m_filterWidget);
                this->menuRemoveItems(MenuFilter);
                if (m_filterWidget->parent() == this) { m_filterWidget->deleteLater(); }
                m_filterWidget = nullptr;
            }

            if (filterWidget)
            {
                this->menuAddItems(MenuFilter);
                this->m_filterWidget = filterWidget;
            }
        }

        void CViewBaseNonTemplate::setFilterDialog(CFilterDialog *filterDialog)
        {
            this->setFilterWidgetImpl(filterDialog);
            if (filterDialog)
            {
                bool s = connect(filterDialog, &CFilterDialog::finished, this, &CViewBaseNonTemplate::ps_filterDialogFinished);
                Q_ASSERT_X(s, Q_FUNC_INFO, "filter dialog connect");
                Q_UNUSED(s);
            }
        }

        void CViewBaseNonTemplate::setFilterWidget(CFilterWidget *filterWidget)
        {
            this->setFilterWidgetImpl(filterWidget);
            if (filterWidget)
            {
                bool s = connect(filterWidget, &CFilterWidget::changeFilter, this, &CViewBaseNonTemplate::ps_filterWidgetChangedFilter);
                Q_ASSERT_X(s, Q_FUNC_INFO, "filter connect");
                s = connect(this, &CViewBaseNonTemplate::modelDataChanged, filterWidget, &CFilterWidget::onRowCountChanged);
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
            return CGuiUtility::mainApplicationWindow();
        }

        CStatusMessage CViewBaseNonTemplate::showFileLoadDialog()
        {
            return this->ps_loadJson();
        }

        CStatusMessage CViewBaseNonTemplate::showFileSaveDialog() const
        {
            return this->ps_saveJson();
        }

        IMenuDelegate *CViewBaseNonTemplate::setCustomMenu(IMenuDelegate *menu, bool nestPreviousMenu)
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
            return menu;
        }

        CMenuActions CViewBaseNonTemplate::initMenuActions(CViewBaseNonTemplate::MenuFlag menu)
        {
            if (this->m_menuFlagActions.contains(menu)) { return this->m_menuFlagActions.value(menu); }

            CMenuActions ma;
            switch (menu)
            {
            case MenuRefresh: { ma.addAction(BlackMisc::CIcons::refresh16(), "Update", CMenuAction::pathViewUpdates(), { this, &CViewBaseNonTemplate::requestUpdate }); break; }
            case MenuBackend: { ma.addAction(BlackMisc::CIcons::refresh16(), "Reload from backend", CMenuAction::pathViewUpdates(), { this, &CViewBaseNonTemplate::requestNewBackendData}); break; }
            case MenuDisplayAutomatically:
                {
                    QAction *a = ma.addAction(CIcons::appMappings16(), "Automatically display (when loaded)", CMenuAction::pathViewUpdates(), { this, &CViewBaseNonTemplate::ps_toggleAutoDisplay });
                    a->setCheckable(true);
                    a->setChecked(this->displayAutomatically());
                    break;
                }
            case MenuRemoveSelectedRows: { ma.addAction(BlackMisc::CIcons::delete16(), "Remove selected rows", CMenuAction::pathViewAddRemove(), { this, &CViewBaseNonTemplate::ps_removeSelectedRows }, CShortcut::keyDelete()); break; }
            case MenuClear: { ma.addAction(BlackMisc::CIcons::delete16(), "Clear", CMenuAction::pathViewAddRemove(), { this, &CViewBaseNonTemplate::ps_clear }); break; }
            case MenuFilter:
                {
                    ma.addAction(CIcons::filter16(), "Filter", CMenuAction::pathViewFilter(), { this, &CViewBaseNonTemplate::ps_displayFilterDialog }, CShortcut::keyDisplayFilter());
                    ma.addAction(CIcons::filter16(), "Remove Filter", CMenuAction::pathViewFilter(), { this, &CViewBaseNonTemplate::ps_removeFilter });
                    break;
                }
            case MenuLoad: { ma.addAction(CIcons::disk16(), "Load from file", CMenuAction::pathViewLoadSave(), { this, &CViewBaseNonTemplate::ps_loadJsonAction }); break; }
            case MenuSave: { ma.addAction(CIcons::disk16(), "Save data in file", CMenuAction::pathViewLoadSave(), { this, &CViewBaseNonTemplate::ps_saveJsonAction }, CShortcut::keySaveViews()); break; }
            default:
                break;
            }
            this->m_menuFlagActions.insert(menu, ma);
            return ma;
        }

        void CViewBaseNonTemplate::customMenu(CMenuActions &menuActions)
        {
            // delegate?
            if (this->m_menu) { this->m_menu->customMenu(menuActions); }

            // standard view menus
            if (this->m_menus.testFlag(MenuRefresh)) { menuActions.addActions(this->initMenuActions(MenuRefresh)); }
            if (this->m_menus.testFlag(MenuBackend)) { menuActions.addActions(this->initMenuActions(MenuBackend)); }
            if (m_showingLoadIndicator)
            {
                // just in case, if this ever will be dangling
                menuActions.addAction(BlackMisc::CIcons::preloader16(), "Hide load indicator", CMenuAction::pathViewUpdates(), nullptr, { this, &CViewBaseNonTemplate::hideLoadIndicator });
            }

            if (this->m_menus.testFlag(MenuClear)) { menuActions.addActions(this->initMenuActions(MenuClear)); }
            if (this->m_menus.testFlag(MenuDisplayAutomatically))
            {
                // here I expect only one action
                QAction *a = menuActions.addActions(this->initMenuActions(MenuDisplayAutomatically)).first();
                a->setChecked(this->displayAutomatically());
            }
            if (this->m_menus.testFlag(MenuRemoveSelectedRows))
            {
                if (this->hasSelection())
                {
                    menuActions.addActions(this->initMenuActions(MenuRemoveSelectedRows));
                }
            }
            if (this->m_menus.testFlag(MenuFilter))
            {
                menuActions.addActions(this->initMenuActions(MenuFilter));
            }

            // selection menus, not in menu action list because it depends on current selection
            const SelectionMode sm = this->selectionMode();
            if (sm == MultiSelection || sm == ExtendedSelection)
            {
                menuActions.addAction("Select all", CMenuAction::pathViewSelection(), nullptr, { this, &CViewBaseNonTemplate::selectAll }, Qt::CTRL + Qt::Key_A);
            }
            if (sm != NoSelection)
            {
                menuActions.addAction("Clear selection", CMenuAction::pathViewSelection(), nullptr, { this, &CViewBaseNonTemplate::clearSelection }, CShortcut::keyClearSelection());
            }
            if ((this->m_originalSelectionMode == MultiSelection || this->m_originalSelectionMode == ExtendedSelection) && this->m_menus.testFlag(MenuToggleSelectionMode))
            {
                if (sm != MultiSelection)
                {
                    QAction *a = menuActions.addAction("Switch to multi selection", CMenuAction::pathViewSelection(), nullptr, { this, &CViewBaseNonTemplate::ps_toggleSelectionMode });
                    a->setData(MultiSelection);
                }

                if (sm != ExtendedSelection)
                {
                    QAction *a = menuActions.addAction("Switch to extended selection", CMenuAction::pathViewSelection(), nullptr, { this, &CViewBaseNonTemplate::ps_toggleSelectionMode });
                    a->setData(ExtendedSelection);
                }

                if (sm != SingleSelection)
                {
                    QAction *a = menuActions.addAction("Switch to single selection", CMenuAction::pathViewSelection(), nullptr, { this, &CViewBaseNonTemplate::ps_toggleSelectionMode });
                    a->setData(SingleSelection);
                }
            }

            // load/save
            if (m_menus.testFlag(MenuLoad)) { menuActions.addActions(this->initMenuActions(MenuLoad)); }
            if (m_menus.testFlag(MenuSave) && !isEmpty()) { menuActions.addActions(this->initMenuActions(MenuSave)); }

            // resizing
            menuActions.addAction(BlackMisc::CIcons::resize16(), "Resize", CMenuAction::pathViewResize(), nullptr, { this, &CViewBaseNonTemplate::presizeOrFullResizeToContents });

            // resize to content might decrease performance,
            // so I only allow changing to "content resizing" if size matches
            const bool enabled = !this->reachedResizeThreshold();
            const bool autoResize = this->m_resizeMode == ResizingAuto;

            // when not auto let set how we want to resize rows
            if (m_rowResizeMode == Interactive)
            {
                QAction *a = menuActions.addAction(BlackMisc::CIcons::resizeVertical16(), " Resize rows to content (auto)", CMenuAction::pathViewResize(), nullptr, { this, &CViewBaseNonTemplate::rowsResizeModeToContent });
                a->setEnabled(enabled && !autoResize);
            }
            else
            {
                QAction *a = menuActions.addAction(BlackMisc::CIcons::resizeVertical16(), "Resize rows interactive", CMenuAction::pathViewResize(), nullptr, { this, &CViewBaseNonTemplate::rowsResizeModeToInteractive });
                a->setEnabled(!autoResize);
            }

            QAction *actionInteractiveResize = menuActions.addAction(CIcons::viewMultiColumn(), "Resize (auto)", CMenuAction::pathViewResize(), nullptr);
            actionInteractiveResize->setObjectName(this->objectName().append("ActionResizing"));
            actionInteractiveResize->setCheckable(true);
            actionInteractiveResize->setChecked(autoResize);
            actionInteractiveResize->setEnabled(enabled);
            connect(actionInteractiveResize, &QAction::toggled, this, &CViewBaseNonTemplate::ps_toggleResizeMode);
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

        int CViewBaseNonTemplate::selectedRowCount() const
        {
            if (!this->hasSelection()) { return 0;}
            return this->selectedRows().count();
        }

        bool CViewBaseNonTemplate::hasSingleSelectedRow() const
        {
            return this->selectedRowCount() == 1;
        }

        bool CViewBaseNonTemplate::hasMultipleSelectedRows() const
        {
            return this->selectedRowCount() > 1;
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

        QString CViewBaseNonTemplate::getDefaultFilename(bool load) const
        {
            // some logic to find a useful default name
            QString dir = CBuildConfig::getDocumentationDirectory();

            if (load)
            {
                return CFileUtils::appendFilePaths(dir, CFileUtils::jsonWildcardAppendix());
            }

            // Save file path
            QString name(this->m_saveFileName);
            if (name.isEmpty())
            {
                // create a name
                if (this->getDockWidgetInfoArea()) { name = this->getDockWidgetInfoArea()->windowTitle(); }
                else if (!windowTitle().isEmpty()) { name = this->windowType(); }
                else { name = this->metaObject()->className(); }
            }
            if (!name.endsWith(CFileUtils::jsonAppendix(), Qt::CaseInsensitive))
            {
                name += CFileUtils::jsonAppendix();
            }
            return CFileUtils::appendFilePaths(dir, name);
        }

        void CViewBaseNonTemplate::menuRemoveItems(Menu menusToRemove)
        {
            this->m_menus &= (~menusToRemove);
        }

        void CViewBaseNonTemplate::menuAddItems(Menu menusToAdd)
        {
            this->m_menus |= menusToAdd;
            if (menusToAdd.testFlag(MenuRemoveSelectedRows))
            {
                this->m_enableDeleteSelectedRows = true;
            }
        }

        int CViewBaseNonTemplate::ps_updateContainer(const CVariant &variant, bool sort, bool resize)
        {
            return this->performUpdateContainer(variant, sort, resize);
        }

        void CViewBaseNonTemplate::ps_displayFilterDialog()
        {
            if (!this->m_menus.testFlag(MenuFilter)) { return; }
            if (!this->m_filterWidget) { return; }
            this->m_filterWidget->show();
        }

        void CViewBaseNonTemplate::ps_loadJsonAction()
        {
            if (this->isEmpty()) { return; }
            if (!this->m_menus.testFlag(MenuLoad)) { return; }
            this->ps_loadJson();
        }

        void CViewBaseNonTemplate::ps_saveJsonAction()
        {
            if (this->isEmpty()) { return; }
            if (!this->m_menus.testFlag(MenuSave)) { return; }
            this->ps_saveJson();
        }

        void CViewBaseNonTemplate::onModelChanged()
        {
            this->updateSortIndicator();
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

        void CViewBaseNonTemplate::showLoadIndicator(int containerSizeDependent, bool processEvents)
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

            if (!this->m_loadIndicator)
            {
                this->m_loadIndicator = new CLoadIndicator(64, 64, this);
            }
            this->centerLoadIndicator();
            this->m_loadIndicator->startAnimation();
            if (processEvents)
            {
                sGui->processEventsToRefreshGui();
            }
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
            if (m_resizeMode == PresizeSubset) { return false; }
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
            // magic trick from:
            // http://stackoverflow.com/q/3433664/356726
            this->setVisible(false);
            const QRect vpOriginal = this->viewport()->geometry();
            QRect vpNew = vpOriginal;
            vpNew.setWidth(std::numeric_limits<int>::max());
            this->viewport()->setGeometry(vpNew);

            this->m_resizeCount++;
            this->resizeColumnsToContents(); // columns
            this->resizeRowsToContents(); // rows
            if (m_forceStretchLastColumnWhenResized)
            {
                // re-stretch
                this->horizontalHeader()->setStretchLastSection(true);
            }
            this->viewport()->setGeometry(vpOriginal);

            // if I store the original visibility and then
            // set it back here, the whole view disappears
            this->setVisible(true);
        }

        void CViewBaseNonTemplate::ps_customMenuRequested(QPoint pos)
        {
            QMenu menu;
            CMenuActions menuActions;
            this->customMenu(menuActions);
            if (menuActions.isEmpty()) { return; }
            menuActions.toQMenu(menu, true);

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

        void CViewBaseNonTemplate::ps_toggleSelectionMode()
        {
            if (this->m_originalSelectionMode == ExtendedSelection || this->m_originalSelectionMode == MultiSelection)
            {
                QAction *action = qobject_cast<QAction *>(sender());
                if (action && action->data().isValid() && action->data().canConvert<int>())
                {
                    SelectionMode sm = static_cast<SelectionMode>(action->data().toInt());
                    this->setSelectionMode(sm);
                }
                else
                {
                    this->setSelectionMode(this->m_originalSelectionMode);
                }
            }
        }

        void CViewBaseNonTemplate::ps_removeSelectedRows()
        {
            if (!m_enableDeleteSelectedRows) { return; }
            this->removeSelectedRows();
        }

        void CViewBaseNonTemplate::ps_updatedIndicator()
        {
            this->update();
        }

        void CViewBaseNonTemplate::dragEnterEvent(QDragEnterEvent *event)
        {
            if (!event || !this->acceptDrop(event->mimeData())) { return; }
            setBackgroundRole(QPalette::Highlight);
            event->acceptProposedAction();
        }

        void CViewBaseNonTemplate::dragMoveEvent(QDragMoveEvent *event)
        {
            if (!event || !this->acceptDrop(event->mimeData())) { return; }
            event->acceptProposedAction();
        }

        void CViewBaseNonTemplate::dragLeaveEvent(QDragLeaveEvent *event)
        {
            if (!event) { return; }
            event->accept();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        CViewBase<ModelClass, ContainerType, ObjectType>::CViewBase(QWidget *parent, ModelClass *model) : CViewBaseNonTemplate(parent), m_model(model)
        {
            this->setSortingEnabled(true);
            if (model)
            {
                this->standardInit(model);
            }
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        int CViewBase<ModelClass, ContainerType, ObjectType>::updateContainer(const ContainerType &container, bool sort, bool resize)
        {
            Q_ASSERT_X(this->m_model, Q_FUNC_INFO, "Missing model");
            if (container.isEmpty())
            {
                // shortcut
                this->clear();
                return 0;
            }

            // we have data
            this->showLoadIndicator(container.size());
            const bool reallyResize = resize && isResizeConditionMet(container.size()); // do we really perform resizing
            bool presize = (m_resizeMode == PresizeSubset) &&
                           this->isEmpty() && // only when no data yet
                           !reallyResize;     // not when we resize later
            presize = presize || (this->isEmpty() && resize && !reallyResize); // we presize if we wanted to resize but actually do not because of condition
            const bool presizeThresholdReached = presize && container.size() > ResizeSubsetThreshold; // only when size making sense

            // when we will not resize, we might presize
            if (presizeThresholdReached)
            {
                const int presizeRandomElements = this->getPresizeRandomElementsSize(container.size());
                if (presizeRandomElements > 0)
                {
                    this->m_model->update(container.sampleElements(presizeRandomElements), false);
                    this->fullResizeToContents();
                }
            }
            const int c = this->m_model->update(container, sort);

            // resize after real update according to mode
            if (presizeThresholdReached)
            {
                // currently no furhter actions
            }
            else if (reallyResize)
            {
                this->resizeToContents();
            }
            else if (presize && !presizeThresholdReached)
            {
                // small amount of data not covered before
                this->fullResizeToContents();
            }
            this->updateSortIndicator(); // make sure sort indicator represents sort order
            this->hideLoadIndicator();
            return c;
        }

        int CViewBaseNonTemplate::getPresizeRandomElementsSize(int containerSize) const
        {
            containerSize = containerSize >= 0 ? containerSize : this->rowCount();
            const int presizeRandomElements = containerSize > 1000 ? containerSize / 100 : containerSize / 40;
            return presizeRandomElements;
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
                this->updateContainer(sortedContainer, false, resize);
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
        void CViewBase<ModelClass, ContainerType, ObjectType>::insert(const ContainerType &container, bool resize)
        {
            Q_ASSERT(this->m_model);
            this->m_model->insert(container);
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
        const ContainerType &CViewBase<ModelClass, ContainerType, ObjectType>::containerOrFilteredContainer() const
        {
            Q_ASSERT(this->m_model);
            return this->m_model->containerOrFilteredContainer();
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
        int CViewBase<ModelClass, ContainerType, ObjectType>::updateSelected(const CPropertyIndexVariantMap &vm)
        {
            if (vm.isEmpty()) { return 0; }
            if (!hasSelection()) { return 0; }
            int c = 0;

            int lastUpdatedRow = -1;
            int firstUpdatedRow = -1;
            const CPropertyIndexList propertyIndexes(vm.indexes());
            const QModelIndexList indexes = this->selectedRows();

            for (const QModelIndex &i : indexes)
            {
                if (i.row() == lastUpdatedRow) { continue; }
                lastUpdatedRow = i.row();
                if (firstUpdatedRow < 0 || lastUpdatedRow < firstUpdatedRow) { firstUpdatedRow = lastUpdatedRow; }
                ObjectType obj(this->at(i));

                // update all properties in map
                for (const CPropertyIndex &pi : propertyIndexes)
                {
                    obj.setPropertyByIndex(pi, vm.value(pi));
                }

                // and update container
                if (this->derivedModel()->setInContainer(i, obj))
                {
                    c++;
                }
            }

            if (c > 0)
            {
                this->derivedModel()->emitDataChanged(firstUpdatedRow, lastUpdatedRow);
            }
            return c;
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        int CViewBase<ModelClass, ContainerType, ObjectType>::updateSelected(const CVariant &variant, const CPropertyIndex &index)
        {
            const CPropertyIndexVariantMap vm(index, variant);
            return this->updateSelected(vm);
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
            if (currentRows == selectedRowCount())
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
        void CViewBase<ModelClass, ContainerType, ObjectType>::presizeOrFullResizeToContents()
        {
            const int rc = this->rowCount();
            if (rc > ResizeSubsetThreshold)
            {
                const int presizeRandomElements = this->getPresizeRandomElementsSize(rc);
                if (presizeRandomElements > 0)
                {
                    const ContainerType containerBackup(this->container());
                    this->m_model->update(containerBackup.sampleElements(presizeRandomElements), false);
                    this->fullResizeToContents();
                    this->m_model->update(containerBackup, false);
                }
            }
            else
            {
                this->fullResizeToContents();
            }
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
        bool CViewBase<ModelClass, ContainerType, ObjectType>::isOrderable() const
        {
            Q_ASSERT(this->m_model);
            return this->m_model->isOrderable();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::allowDragDrop(bool allowDrag, bool allowDrop)
        {
            Q_ASSERT(this->m_model);

            // see model for implementing logic of drag
            this->viewport()->setAcceptDrops(allowDrop);
            this->setDragEnabled(allowDrag);
            this->setDropIndicatorShown(allowDrop);
            this->m_model->allowDrop(allowDrop);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        bool CViewBase<ModelClass, ContainerType, ObjectType>::isDropAllowed() const
        {
            Q_ASSERT(this->m_model);
            return this->m_model->isDropAllowed();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        bool CViewBase<ModelClass, ContainerType, ObjectType>::acceptDrop(const QMimeData *mimeData) const
        {
            Q_ASSERT(this->m_model);
            const bool a = this->m_model->acceptDrop(mimeData);
            return a;
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::setSorting(const CPropertyIndex &propertyIndex, Qt::SortOrder order)
        {
            Q_ASSERT(this->m_model);
            this->m_model->setSorting(propertyIndex, order);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        QJsonObject CViewBase<ModelClass, ContainerType, ObjectType>::toJson() const
        {
            Q_ASSERT(this->m_model);
            return this->m_model->toJson();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        QString CViewBase<ModelClass, ContainerType, ObjectType>::toJsonString(QJsonDocument::JsonFormat format) const
        {
            Q_ASSERT(this->m_model);
            return this->m_model->toJsonString(format);
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
        void CViewBase<ModelClass, ContainerType, ObjectType>::addContainerTypesAsDropTypes(bool objectType, bool containerType)
        {
            if (objectType) { this->m_model->addAcceptedMetaTypeId(qMetaTypeId<ObjectType>()); }
            if (containerType) { this->m_model->addAcceptedMetaTypeId(qMetaTypeId<ContainerType>()); }
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::initAsOrderable()
        {
            Q_ASSERT_X(isOrderable(), Q_FUNC_INFO, "Model not orderable");
            this->allowDragDrop(true, true);
            this->setDragDropMode(InternalMove);
            this->setDropActions(Qt::MoveAction);
            this->addContainerTypesAsDropTypes(true, true);
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
                if (model == this->m_model) { return; }
                if (this->m_model)
                {
                    this->m_model->disconnect();
                }

                this->m_model = model;
                bool c = connect(this->m_model, &ModelClass::modelDataChanged, this, &CViewBase::modelDataChanged);
                Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
                c = connect(this->m_model, &ModelClass::objectChanged, this, &CViewBase::objectChanged);
                Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
                c = connect(this->m_model, &ModelClass::changed, this, &CViewBase::modelChanged);
                Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
                c = connect(this->m_model, &ModelClass::changed, this, &CViewBase::onModelChanged);
                Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
                Q_UNUSED(c);
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
            // small set or large set? This only performs real resizing, no presizing
            // remark, see also presizeOrFullResizeToContents
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
        void CViewBase<ModelClass, ContainerType, ObjectType>::updateSortIndicator()
        {
            if (this->derivedModel()->hasValidSortColumn())
            {
                const int index = this->derivedModel()->getSortColumn();
                Qt::SortOrder order = this->derivedModel()->getSortOrder();
                this->horizontalHeader()->setSortIndicator(index, order);
            }
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        CStatusMessage CViewBase<ModelClass, ContainerType, ObjectType>::modifyLoadedJsonData(ContainerType &data) const
        {
            Q_UNUSED(data);
            static const CStatusMessage e(this, CStatusMessage::SeverityInfo, "no modification", true);
            return e;
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        CStatusMessage CViewBase<ModelClass, ContainerType, ObjectType>::validateLoadedJsonData(const ContainerType &data) const
        {
            Q_UNUSED(data);
            static const CStatusMessage e(this, CStatusMessage::SeverityInfo, "no validation", true);
            return e;
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::jsonLoadedAndModelUpdated(const ContainerType &data)
        {
            Q_UNUSED(data);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        CStatusMessage CViewBase<ModelClass, ContainerType, ObjectType>::ps_loadJson()
        {
            CStatusMessage m;
            do
            {
                const QString fileName = QFileDialog::getOpenFileName(nullptr,
                                         tr("Load data file"), getDefaultFilename(true),
                                         tr("swift (*.json *.txt)"));
                if (fileName.isEmpty())
                {
                    m = CStatusMessage(this, CStatusMessage::SeverityDebug, "Load canceled", true);
                    break;
                }

                QString json(CFileUtils::readFileToString(fileName));
                if (json.isEmpty())
                {
                    m = CStatusMessage(this, CStatusMessage::SeverityWarning, "Reading " + fileName + " yields no data", true);
                    break;
                }
                ContainerType container;
                container.convertFromJson(json);
                m = this->modifyLoadedJsonData(container);
                if (m.isFailure()) { break; } // modification error
                m = this->validateLoadedJsonData(container);
                if (m.isFailure()) { break; } // validaton error
                this->updateContainerMaybeAsync(container);
                m = CStatusMessage(this, CStatusMessage::SeverityInfo, "Reading " + fileName + " completed", true);
                this->jsonLoadedAndModelUpdated(container);
            }
            while (false);

            emit this->jsonLoadCompleted(m);
            return m;
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        CStatusMessage CViewBase<ModelClass, ContainerType, ObjectType>::ps_saveJson() const
        {
            const QString fileName = QFileDialog::getSaveFileName(nullptr,
                                     tr("Save data file"), getDefaultFilename(false),
                                     tr("swift (*.json *.txt)"));
            if (fileName.isEmpty()) { return CStatusMessage(this, CStatusMessage::SeverityDebug, "Save canceled", true); }
            const QString json(this->toJsonString());
            bool ok = CFileUtils::writeStringToFileInBackground(json, fileName);
            if (ok)
            {
                return CStatusMessage(this, CStatusMessage::SeverityInfo, "Writing " + fileName + " in progress", true);
            }
            else
            {
                return CStatusMessage(this, CStatusMessage::SeverityError, "Writing " + fileName + " failed", true);
            }
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
                    // takes the filter and triggers the filtering
                    IModelFilterProvider<ContainerType> *provider = dynamic_cast<IModelFilterProvider<ContainerType>*>(this->m_filterWidget);
                    Q_ASSERT_X(provider, Q_FUNC_INFO, "Filter widget does not provide interface");
                    if (!provider) { return false; }
                    std::unique_ptr<IModelFilter<ContainerType>> f(provider->createModelFilter());
                    if (f->isValid())
                    {
                        this->takeFilterOwnership(f);
                    }
                    else
                    {
                        this->removeFilter();
                    }
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
