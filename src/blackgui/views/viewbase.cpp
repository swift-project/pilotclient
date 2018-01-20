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
#include "blackgui/menus/fontmenus.h"
#include "blackgui/shortcut.h"
#include "blackgui/views/viewbase.h"
#include "blackgui/views/viewbaseproxystyle.h"
#include "blackgui/views/viewbaseitemdelegate.h"
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
#include "blackmisc/logmessage.h"
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
#include "blackmisc/simulation/matchingstatisticsentry.h"
#include "blackmisc/simulation/matchingstatistics.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/weather/cloudlayerlist.h"
#include "blackmisc/weather/temperaturelayerlist.h"
#include "blackmisc/weather/windlayerlist.h"
#include "blackmisc/worker.h"

#include <QApplication>
#include <QAction>
#include <QClipboard>
#include <QDialog>
#include <QDesktopWidget>
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
#include <QMetaMethod>
#include <limits>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackGui::Menus;
using namespace BlackGui::Models;
using namespace BlackGui::Filters;
using namespace BlackGui::Settings;

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

            // setting resize mode rowsResizeModeToContent() causes extremly slow views
            // default, see: m_rowResizeMode

            // scroll modes
            this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
            this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
            this->setWordWrap(true);
            this->setTextElideMode(Qt::ElideMiddle);

            // shortcuts
            QShortcut *filter = new QShortcut(CShortcut::keyDisplayFilter(), this, SLOT(ps_displayFilterDialog()), nullptr, Qt::WidgetShortcut);
            filter->setObjectName("Filter shortcut for " + this->objectName());
            QShortcut *clearSelection = new QShortcut(CShortcut::keyClearSelection(), this, SLOT(clearSelection()), nullptr, Qt::WidgetShortcut);
            clearSelection->setObjectName("Clear selection shortcut for " + this->objectName());
            QShortcut *saveJson = new QShortcut(CShortcut::keySaveViews(), this, SLOT(ps_saveJsonAction()), nullptr, Qt::WidgetShortcut);
            saveJson->setObjectName("Save JSON for " + this->objectName());
            QShortcut *deleteRow = new QShortcut(CShortcut::keyDelete(), this, SLOT(ps_removeSelectedRows()), nullptr, Qt::WidgetShortcut);
            deleteRow->setObjectName("Delete selected rows for " + this->objectName());
            QShortcut *copy = new QShortcut(CShortcut::keyCopy(), this, SLOT(ps_copy()), nullptr, Qt::WidgetShortcut);
            copy->setObjectName("Copy rows for " + this->objectName());
        }

        CViewBaseNonTemplate::~CViewBaseNonTemplate()
        {
            // dtor
        }

        bool CViewBaseNonTemplate::setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget)
        {
            const bool c = CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea(parentDockableWidget);
            return c;
        }

        void CViewBaseNonTemplate::resizeToContents()
        {
            this->performModeBasedResizeToContent();
        }

        void CViewBaseNonTemplate::setFilterWidgetImpl(QWidget *filterWidget)
        {
            if (filterWidget == m_filterWidget) { return; }

            // dialog or filter widget
            if (m_filterWidget)
            {
                disconnect(m_filterWidget);
                this->menuRemoveItems(MenuFilter);
                if (m_filterWidget->parent() == this) { m_filterWidget->deleteLater(); }
                m_filterWidget = nullptr;
            }

            if (filterWidget)
            {
                this->menuAddItems(MenuFilter);
                m_filterWidget = filterWidget;
            }
        }

        void CViewBaseNonTemplate::setFilterDialog(CFilterDialog *filterDialog)
        {
            if (filterDialog == m_filterWidget) { return; }
            this->setFilterWidgetImpl(filterDialog);
            if (filterDialog)
            {
                const bool s = connect(filterDialog, &CFilterDialog::finished, this, &CViewBaseNonTemplate::ps_filterDialogFinished);
                Q_ASSERT_X(s, Q_FUNC_INFO, "filter dialog connect");
                Q_UNUSED(s);
            }
        }

        void CViewBaseNonTemplate::setFilterWidget(CFilterWidget *filterWidget)
        {
            if (filterWidget == m_filterWidget) { return; }
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
            return m_loadIndicator && m_enabledLoadIndicator && m_showingLoadIndicator;
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
            return CGuiUtility::mainApplicationWidget();
        }

        CStatusMessage CViewBaseNonTemplate::showFileLoadDialog()
        {
            return this->ps_loadJson();
        }

        CStatusMessage CViewBaseNonTemplate::showFileSaveDialog()
        {
            return this->ps_saveJson();
        }

        IMenuDelegate *CViewBaseNonTemplate::setCustomMenu(IMenuDelegate *menu, bool nestPreviousMenu)
        {
            if (menu && nestPreviousMenu)
            {
                // new menu with nesting
                menu->setNestedDelegate(m_menu);
                m_menu = menu;
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
            if (m_menuFlagActions.contains(menu)) { return m_menuFlagActions.value(menu); }

            CMenuActions ma;
            switch (menu)
            {
            case MenuRefresh:
                {
                    static const QMetaMethod requestSignal = QMetaMethod::fromSignal(&CViewBaseNonTemplate::requestUpdate);
                    if (!this->isSignalConnected(requestSignal)) break;
                    ma.addAction(BlackMisc::CIcons::refresh16(), "Update", CMenuAction::pathViewUpdates(), { this, &CViewBaseNonTemplate::ps_triggerReload }); break;
                }
            case MenuBackend:
                {
                    static const QMetaMethod requestSignal = QMetaMethod::fromSignal(&CViewBaseNonTemplate::requestNewBackendData);
                    if (!this->isSignalConnected(requestSignal)) break;
                    ma.addAction(BlackMisc::CIcons::refresh16(), "Reload from backend", CMenuAction::pathViewUpdates(), { this, &CViewBaseNonTemplate::ps_triggerReloadFromBackend }); break;
                }
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
                    if (m_filterWidget)
                    {
                        const bool dialog = qobject_cast<QDialog *>(m_filterWidget);
                        if (dialog) ma.addAction(CIcons::filter16(), "Show filter", CMenuAction::pathViewFilter(), { this, &CViewBaseNonTemplate::ps_displayFilterDialog }, CShortcut::keyDisplayFilter());
                        ma.addAction(CIcons::filter16(), "Remove Filter", CMenuAction::pathViewFilter(), { this, &CViewBaseNonTemplate::ps_removeFilter });
                    }
                    break;
                }
            case MenuMaterializeFilter: { ma.addAction(CIcons::tableRelationship16(), "Materialize filtered data", CMenuAction::pathViewFilter(), { this, &CViewBaseNonTemplate::materializeFilter }); break; }
            case MenuLoad: { ma.addAction(CIcons::disk16(), "Load from file", CMenuAction::pathViewLoadSave(), { this, &CViewBaseNonTemplate::ps_loadJsonAction }); break; }
            case MenuSave: { ma.addAction(CIcons::disk16(), "Save data in file", CMenuAction::pathViewLoadSave(), { this, &CViewBaseNonTemplate::ps_saveJsonAction }, CShortcut::keySaveViews()); break; }
            case MenuCut:
                {
                    if (!QApplication::clipboard()) break;
                    ma.addAction(CIcons::cut16(), "Cut", CMenuAction::pathViewCutPaste(), { this, &CViewBaseNonTemplate::ps_cut }, QKeySequence(QKeySequence::Paste));
                    break;
                }
            case MenuPaste:
                {
                    if (!QApplication::clipboard()) break;
                    ma.addAction(CIcons::paste16(), "Paste", CMenuAction::pathViewCutPaste(), { this, &CViewBaseNonTemplate::ps_paste }, QKeySequence(QKeySequence::Paste));
                    break;
                }
            case MenuCopy:
                {
                    if (!QApplication::clipboard()) break;
                    ma.addAction(CIcons::copy16(), "Copy", CMenuAction::pathViewCutPaste(), { this, &CViewBaseNonTemplate::ps_copy }, QKeySequence(QKeySequence::Copy));
                    break;
                }
            default:
                break;
            }
            m_menuFlagActions.insert(menu, ma);
            return ma;
        }

        void CViewBaseNonTemplate::settingsChanged()
        {
            if (!this->allowsMultipleSelectedRows()) { return; }
            const CGeneralGuiSettings settings = m_guiSettings.getThreadLocal();
            m_originalSelectionMode = settings.getPreferredSelection();
            if (this->isCurrentlyAllowingMultipleRowSelections())
            {
                this->setSelectionMode(settings.getPreferredSelection());
            }
        }

        void CViewBaseNonTemplate::customMenu(CMenuActions &menuActions)
        {
            // delegate?
            if (m_menu) { m_menu->customMenu(menuActions); }

            // standard view menus
            if (m_menus.testFlag(MenuRefresh)) { menuActions.addActions(this->initMenuActions(MenuRefresh)); }
            if (m_menus.testFlag(MenuBackend)) { menuActions.addActions(this->initMenuActions(MenuBackend)); }
            if (m_showingLoadIndicator)
            {
                // just in case, if this ever will be dangling
                menuActions.addAction(BlackMisc::CIcons::preloader16(), "Hide load indicator", CMenuAction::pathViewUpdates(), nullptr, { this, &CViewBaseNonTemplate::ps_hideLoadIndicator });
            }

            if (m_menus.testFlag(MenuClear)) { menuActions.addActions(this->initMenuActions(MenuClear)); }
            if (m_menus.testFlag(MenuDisplayAutomatically))
            {
                // here I expect only one action
                QAction *a = menuActions.addActions(this->initMenuActions(MenuDisplayAutomatically)).first();
                a->setChecked(this->displayAutomatically());
            }
            if (m_menus.testFlag(MenuRemoveSelectedRows))
            {
                if (this->hasSelection())
                {
                    menuActions.addActions(this->initMenuActions(MenuRemoveSelectedRows));
                }
            }

            if (m_menus.testFlag(MenuCopy)) { menuActions.addActions(this->initMenuActions(MenuCopy)); }
            if (m_menus.testFlag(MenuCut)) { menuActions.addActions(this->initMenuActions(MenuCut)); }
            if (m_menus.testFlag(MenuPaste)) { menuActions.addActions(this->initMenuActions(MenuPaste)); }
            if (m_menus.testFlag(MenuFont) && m_fontMenu)
            {
                menuActions.addActions(m_fontMenu->getActions(), CMenuAction::pathFont());
            }

            if (m_menus.testFlag(MenuFilter) && m_filterWidget)
            {
                menuActions.addActions(this->initMenuActions(MenuFilter));
                if (m_menus.testFlag(MenuMaterializeFilter))
                {
                    menuActions.addActions(this->initMenuActions(MenuMaterializeFilter));
                }
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
            if ((m_originalSelectionMode == MultiSelection || m_originalSelectionMode == ExtendedSelection) && m_menus.testFlag(MenuToggleSelectionMode))
            {
                if (sm != MultiSelection)
                {
                    menuActions.addAction("Switch to multi selection", CMenuAction::pathViewSelection(), nullptr, { this, &CViewBaseNonTemplate::ps_setMultiSelection });
                }

                if (sm != ExtendedSelection)
                {
                    menuActions.addAction("Switch to extended selection", CMenuAction::pathViewSelection(), nullptr, { this, &CViewBaseNonTemplate::ps_setExtendedSelection });
                }

                if (sm != SingleSelection)
                {
                    menuActions.addAction("Switch to single selection", CMenuAction::pathViewSelection(), nullptr, { this, &CViewBaseNonTemplate::ps_setSingleSelection });
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
            const bool autoResize = (m_resizeMode == ResizingAuto);

            // when not set to auto, then lets set how we want to resize rows
            if (m_rowResizeMode == Interactive)
            {
                QAction *a = menuActions.addAction(CIcons::resizeVertical16(), " Resize rows to content (auto), can be slow", CMenuAction::pathViewResize(), nullptr, { this, &CViewBaseNonTemplate::rowsResizeModeToContent });
                a->setEnabled(enabled && !autoResize);
            }
            else
            {
                QAction *a = menuActions.addAction(CIcons::resizeVertical16(), "Resize rows interactively", CMenuAction::pathViewResize(), nullptr, { this, &CViewBaseNonTemplate::rowsResizeModeToInteractive });
                a->setEnabled(!autoResize);
            }

            QAction *actionInteractiveResize = menuActions.addAction(CIcons::viewMultiColumn(), "Resize (auto)", CMenuAction::pathViewResize(), nullptr);
            actionInteractiveResize->setObjectName(this->objectName().append("ActionResizing"));
            actionInteractiveResize->setCheckable(true);
            actionInteractiveResize->setChecked(autoResize);
            actionInteractiveResize->setEnabled(enabled);
            connect(actionInteractiveResize, &QAction::toggled, this, &CViewBaseNonTemplate::ps_toggleResizeMode);
        }

        void CViewBaseNonTemplate::resizeEvent(QResizeEvent *event)
        {
            if (this->isShowingLoadIndicator())
            {
                // re-center
                this->centerLoadIndicator();
            }
            QTableView::resizeEvent(event);
        }

        int CViewBaseNonTemplate::getHorizontalHeaderFontHeight() const
        {
            const QFontMetrics m(this->getHorizontalHeaderFont());
            const int h = m.height();
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

        void CViewBaseNonTemplate::selectRows(const QSet<int> &rows)
        {
            if (!this->selectionModel()) { return; }

            // multiple times faster than multiple than this->selectRow()
            this->clearSelection();
            QItemSelection selectedItems;
            const int columns = this->model()->columnCount() - 1;
            for (int r : rows)
            {
                selectedItems.select(this->model()->index(r, 0), this->model()->index(r, columns));
            }
            this->selectionModel()->select(selectedItems, QItemSelectionModel::Select);
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

        bool CViewBaseNonTemplate::allowsMultipleSelectedRows() const
        {
            return m_originalSelectionMode == ExtendedSelection || m_originalSelectionMode == MultiSelection;
        }

        bool CViewBaseNonTemplate::isCurrentlyAllowingMultipleRowSelections() const
        {
            QAbstractItemView::SelectionMode m = this->selectionMode();
            return m == QAbstractItemView::MultiSelection || m == QAbstractItemView::ExtendedSelection;
        }

        void CViewBaseNonTemplate::init()
        {
            this->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive); // faster mode
            this->horizontalHeader()->setStretchLastSection(true);
            const int fh = qRound(1.5 * this->getHorizontalHeaderFontHeight());
            this->verticalHeader()->setDefaultSectionSize(fh); // for height
            this->verticalHeader()->setMinimumSectionSize(fh); // for height

            switch (m_rowResizeMode)
            {
            case Interactive: this->rowsResizeModeToInteractive(); break;
            case Content: this->rowsResizeModeToContent(); break;
            default:
                Q_ASSERT_X(false, Q_FUNC_INFO, "wrong resize mode");
                break;
            }

            // call this deferred, otherwise the values are overridden with any values
            // from the UI builder
            QTimer::singleShot(500, this, &CViewBaseNonTemplate::settingsChanged);
        }

        QString CViewBaseNonTemplate::getSettingsFileName(bool load) const
        {
            // some logic to find a useful default name
            const QString dir = m_dirSettings.get();
            if (load)
            {
                return CFileUtils::appendFilePaths(dir, CFileUtils::jsonWildcardAppendix());
            }

            // Save file path
            QString name(m_saveFileName);
            if (name.isEmpty())
            {
                // create a name
                if (this->getDockWidgetInfoArea())
                {
                    name = this->getDockWidgetInfoArea()->windowTitle();
                }
                else
                {
                    name = this->metaObject()->className();
                }
            }
            if (!name.endsWith(CFileUtils::jsonAppendix(), Qt::CaseInsensitive))
            {
                name += CFileUtils::jsonAppendix();
            }
            return CFileUtils::appendFilePaths(dir, name);
        }

        void CViewBaseNonTemplate::menuRemoveItems(Menu menusToRemove)
        {
            m_menus &= (~menusToRemove);
        }

        void CViewBaseNonTemplate::menuAddItems(Menu menusToAdd)
        {
            m_menus |= menusToAdd;
            if (menusToAdd.testFlag(MenuRemoveSelectedRows))
            {
                m_enableDeleteSelectedRows = true;
            }
        }

        int CViewBaseNonTemplate::ps_updateContainer(const CVariant &variant, bool sort, bool resize)
        {
            return this->performUpdateContainer(variant, sort, resize);
        }

        void CViewBaseNonTemplate::ps_displayFilterDialog()
        {
            if (!m_menus.testFlag(MenuFilter)) { return; }
            if (!m_filterWidget) { return; }
            m_filterWidget->show();
        }

        void CViewBaseNonTemplate::ps_loadJsonAction()
        {
            if (!m_menus.testFlag(MenuLoad)) { return; }
            const CStatusMessage m = this->ps_loadJson();
            if (!m.isEmpty())
            {
                CLogMessage::preformatted(m);
            }
        }

        void CViewBaseNonTemplate::ps_saveJsonAction()
        {
            if (this->isEmpty()) { return; }
            if (!m_menus.testFlag(MenuSave)) { return; }
            const CStatusMessage m = this->ps_saveJson();
            if (!m.isEmpty())
            {
                CLogMessage::preformatted(m);
            }
        }

        void CViewBaseNonTemplate::ps_triggerReload()
        {
            this->showLoadIndicatorWithTimeout(m_loadIndicatorTimeoutMsDefault);
            emit this->requestUpdate();
        }

        void CViewBaseNonTemplate::ps_triggerReloadFromBackend()
        {
            this->showLoadIndicatorWithTimeout(m_loadIndicatorTimeoutMsDefault);
            emit this->requestNewBackendData();
        }

        void CViewBaseNonTemplate::ps_hideLoadIndicator()
        {
            this->hideLoadIndicator();
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
            m_rowResizeMode = Interactive;
        }

        void CViewBaseNonTemplate::rowsResizeModeToContent()
        {
            QHeaderView *verticalHeader = this->verticalHeader();
            Q_ASSERT(verticalHeader);
            verticalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
            m_rowResizeMode = Content;
        }

        void CViewBaseNonTemplate::rowsResizeModeBasedOnThreshold(int elements)
        {
            if (elements > ResizeRowsToContentThreshold)
            {
                this->rowsResizeModeToInteractive();
            }
            else
            {
                this->rowsResizeModeToContent();
            }
        }

        int CViewBaseNonTemplate::showLoadIndicator(int containerSizeDependent, int timeoutMs, bool processEvents)
        {
            if (!m_enabledLoadIndicator) { return -1; }
            if (m_showingLoadIndicator) { return -1; }
            if (this->hasDockWidgetArea())
            {
                if (!this->isVisibleWidget()) { return -1; }
            }

            if (containerSizeDependent >= 0)
            {
                // really with indicator?
                if (containerSizeDependent < ResizeSubsetThreshold) { return -1; }
            }
            m_showingLoadIndicator = true;
            emit loadIndicatorVisibilityChanged(m_showingLoadIndicator);

            if (!m_loadIndicator)
            {
                m_loadIndicator = new CLoadIndicator(64, 64, this);
            }
            this->centerLoadIndicator();
            return m_loadIndicator->startAnimation(timeoutMs > 0 ? timeoutMs : m_loadIndicatorTimeoutMsDefault, processEvents);
        }

        int CViewBaseNonTemplate::showLoadIndicatorWithTimeout(int timeoutMs, bool processEvents)
        {
            return this->showLoadIndicator(-1, timeoutMs, processEvents);
        }

        void CViewBaseNonTemplate::centerLoadIndicator()
        {
            if (!m_loadIndicator) { return; }
            const QPoint middle = this->viewport()->geometry().center();
            m_loadIndicator->centerLoadIndicator(middle);
        }

        void CViewBaseNonTemplate::hideLoadIndicator(int loadingId)
        {
            if (!m_showingLoadIndicator) { return; }
            m_showingLoadIndicator = false;
            emit loadIndicatorVisibilityChanged(m_showingLoadIndicator);
            if (!m_loadIndicator) { return; }
            m_loadIndicator->stopAnimation(loadingId);
        }

        bool CViewBaseNonTemplate::isResizeConditionMet(int containerSize) const
        {
            if (m_resizeMode == ResizingAlways) { return true; }
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
            // resize to maximum magic trick from:
            // http://stackoverflow.com/q/3433664/356726
            this->setVisible(false);
            const QRect vpOriginal = this->viewport()->geometry();
            if (m_forceColumnsToMaxSize)
            {
                // vpNew.setWidth(std::numeric_limits<qint32>::max()); // largest finite value
                const QRect screenGeometry = QApplication::desktop()->screenGeometry();
                QRect vpNew = vpOriginal;
                vpNew.setWidth(screenGeometry.width());
                this->viewport()->setGeometry(vpNew);
            }

            this->resizeColumnsToContents(); // columns

            // useless if mode is Interactive
            if (m_rowResizeMode == Content)
            {
                this->resizeRowsToContents(); // rows
            }
            m_resizeCount++;

            // re-stretch
            if (m_forceStretchLastColumnWhenResized) { this->horizontalHeader()->setStretchLastSection(true); }
            if (m_forceColumnsToMaxSize) { this->viewport()->setGeometry(vpOriginal); }

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

            // Nested dock widget menu
            const CDockWidgetInfoArea *dockWidget = this->getDockWidgetInfoArea();
            if (dockWidget)
            {
                if (!menu.isEmpty()) { menu.addSeparator(); }
                const QString mm = QString("Dock widget '%1'").arg(dockWidget->windowTitleOrBackup());
                QMenu *dockWidgetSubMenu = menu.addMenu(CIcons::text16(), mm);
                dockWidget->addToContextMenu(dockWidgetSubMenu);
            }

            const QPoint globalPos = this->mapToGlobal(pos);
            menu.exec(globalPos);
        }

        void CViewBaseNonTemplate::ps_toggleResizeMode(bool checked)
        {
            if (checked)
            {
                m_resizeMode = ResizingAuto;
            }
            else
            {
                m_resizeMode = ResizingOff;
            }
        }

        void CViewBaseNonTemplate::ps_toggleAutoDisplay()
        {
            QAction *a = qobject_cast<QAction *>(QObject::sender());
            if (!a) { return; }
            Q_ASSERT_X(a->isCheckable(), Q_FUNC_INFO, "object not checkable");
            m_displayAutomatically = a->isChecked();
        }

        void CViewBaseNonTemplate::ps_setSingleSelection()
        {
            this->setSelectionMode(SingleSelection);
        }

        void CViewBaseNonTemplate::ps_setExtendedSelection()
        {
            if (this->allowsMultipleSelectedRows())
            {
                this->setSelectionMode(ExtendedSelection);
            }
        }

        void CViewBaseNonTemplate::ps_setMultiSelection()
        {
            if (this->allowsMultipleSelectedRows())
            {
                this->setSelectionMode(MultiSelection);
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

        void CViewBaseNonTemplate::dropEvent(QDropEvent *event)
        {
            if (!event) { return; }
            QTableView::dropEvent(event);
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
            Q_ASSERT_X(m_model, Q_FUNC_INFO, "Missing model");
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
                    m_model->update(container.sampleElements(presizeRandomElements), false);
                    this->fullResizeToContents();
                }
            }

            const int c = m_model->update(container, sort);

            // resize after real update according to mode
            if (presizeThresholdReached)
            {
                // currently no furhter actions
            }
            else if (reallyResize)
            {
                this->resizeToContents(); // mode based resize
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
        CWorker *CViewBase<ModelClass, ContainerType, ObjectType>::updateContainerAsync(const ContainerType &container, bool sort, bool resize)
        {
            // avoid unnecessary effort when empty
            if (container.isEmpty())
            {
                this->clear();
                return nullptr;
            }

            Q_UNUSED(sort);
            ModelClass *model = this->derivedModel();
            auto sortColumn = model->getSortColumn();
            auto sortOrder = model->getSortOrder();
            this->showLoadIndicator(container.size());
            CWorker *worker = CWorker::fromTask(this, "ViewSort", [model, container, sortColumn, sortOrder]()
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
            if (container.isEmpty())
            {
                this->clear();
            }
            else if (container.size() > ASyncRowsCountThreshold && sort)
            {
                // larger container with sorting
                this->updateContainerAsync(container, sort, resize);
            }
            else
            {
                this->updateContainer(container, sort, resize);
            }
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::insert(const ObjectType &value, bool resize)
        {
            Q_ASSERT(m_model);
            if (this->rowCount() < 1)
            {
                // this allows presizing
                this->updateContainerMaybeAsync(ContainerType({value}), true, resize);
            }
            else
            {
                m_model->insert(value);
                if (resize) { this->performModeBasedResizeToContent(); }
            }
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::insert(const ContainerType &container, bool resize)
        {
            Q_ASSERT(m_model);
            if (this->rowCount() < 1)
            {
                // this allows presizing
                this->updateContainerMaybeAsync(container, true, resize);
            }
            else
            {
                m_model->insert(container);
                if (resize) { this->performModeBasedResizeToContent(); }
            }
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        const ObjectType &CViewBase<ModelClass, ContainerType, ObjectType>::at(const QModelIndex &index) const
        {
            Q_ASSERT(m_model);
            return m_model->at(index);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        const ContainerType &CViewBase<ModelClass, ContainerType, ObjectType>::container() const
        {
            Q_ASSERT(m_model);
            return m_model->container();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        const ContainerType &CViewBase<ModelClass, ContainerType, ObjectType>::containerOrFilteredContainer(bool *filtered) const
        {
            Q_ASSERT(m_model);
            return m_model->containerOrFilteredContainer(filtered);
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
        ObjectType CViewBase<ModelClass, ContainerType, ObjectType>::firstSelectedOrDefaultObject() const
        {
            if (this->hasSelection())
            {
                return this->selectedObjects().front();
            }
            if (this->rowCount() < 2)
            {
                return this->containerOrFilteredContainer().frontOrDefault();
            }

            // too many, not selected
            return ObjectType();
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
            const ContainerType c = this->selectedObjects();
            return c.frontOrDefault();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        int CViewBase<ModelClass, ContainerType, ObjectType>::removeSelectedRows()
        {
            if (!this->hasSelection()) { return 0; }
            if (this->isEmpty()) { return 0; }

            const int currentRows = this->rowCount();
            if (currentRows == selectedRowCount())
            {
                this->clear();
                return currentRows;
            }

            const ContainerType selected(selectedObjects());
            ContainerType newObjects(container());
            for (const ObjectType &obj : selected)
            {
                newObjects.remove(obj);
            }

            const int delta = currentRows - newObjects.size();
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
                    m_model->update(containerBackup.sampleElements(presizeRandomElements), false);
                    this->fullResizeToContents();
                    m_model->update(containerBackup, false);
                }
            }
            else
            {
                this->fullResizeToContents();
            }
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::clearHighlighting()
        {
            Q_ASSERT(m_model);
            return m_model->clearHighlighting();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::materializeFilter()
        {
            Q_ASSERT(m_model);
            if (!m_model->hasFilter()) { return; }
            if (this->isEmpty()) { return; }
            ContainerType filtered(m_model->containerFiltered());
            this->removeFilter();
            this->updateContainerMaybeAsync(filtered);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::clear()
        {
            Q_ASSERT(m_model);
            m_model->clear();
            this->hideLoadIndicator();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        int CViewBase<ModelClass, ContainerType, ObjectType>::rowCount() const
        {
            Q_ASSERT(m_model);
            return m_model->rowCount();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        int CViewBase<ModelClass, ContainerType, ObjectType>::columnCount() const
        {
            Q_ASSERT(m_model);
            return m_model->columnCount(QModelIndex());
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        bool CViewBase<ModelClass, ContainerType, ObjectType>::isEmpty() const
        {
            Q_ASSERT(m_model);
            return m_model->rowCount() < 1;
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        bool CViewBase<ModelClass, ContainerType, ObjectType>::isOrderable() const
        {
            Q_ASSERT(m_model);
            return m_model->isOrderable();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::allowDragDrop(bool allowDrag, bool allowDrop)
        {
            Q_ASSERT(m_model);

            // see model for implementing logic of drag
            this->viewport()->setAcceptDrops(allowDrop);
            this->setDragEnabled(allowDrag);
            this->setDropIndicatorShown(allowDrag || allowDrop);
            m_model->allowDrop(allowDrop);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        bool CViewBase<ModelClass, ContainerType, ObjectType>::isDropAllowed() const
        {
            Q_ASSERT(m_model);
            return m_model->isDropAllowed();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        bool CViewBase<ModelClass, ContainerType, ObjectType>::acceptDrop(const QMimeData *mimeData) const
        {
            Q_ASSERT(m_model);
            const bool a = m_model->acceptDrop(mimeData);
            return a;
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::setSorting(const CPropertyIndex &propertyIndex, Qt::SortOrder order)
        {
            Q_ASSERT(m_model);
            m_model->setSorting(propertyIndex, order);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::sortByPropertyIndex(const CPropertyIndex &propertyIndex, Qt::SortOrder order)
        {
            m_model->sortByPropertyIndex(propertyIndex, order);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        QJsonObject CViewBase<ModelClass, ContainerType, ObjectType>::toJson() const
        {
            Q_ASSERT(m_model);
            return m_model->toJson();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        QString CViewBase<ModelClass, ContainerType, ObjectType>::toJsonString(QJsonDocument::JsonFormat format) const
        {
            Q_ASSERT(m_model);
            return m_model->toJsonString(format);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::setObjectName(const QString &name)
        {
            // then name here is mainly set for debugging purposes so each model can be identified
            Q_ASSERT(m_model);
            QTableView::setObjectName(name);
            QString modelName = QString(name).append(':').append(m_model->getTranslationContext());
            m_model->setObjectName(modelName);
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
            if (objectType) { m_model->addAcceptedMetaTypeId(qMetaTypeId<ObjectType>()); }
            if (containerType) { m_model->addAcceptedMetaTypeId(qMetaTypeId<ContainerType>()); }
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
            if (m_model->hasValidSortColumn())
            {
                Q_ASSERT(this->horizontalHeader());
                this->horizontalHeader()->setSortIndicator(
                    m_model->getSortColumn(),
                    m_model->getSortOrder());
            }
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::standardInit(ModelClass *model)
        {
            Q_ASSERT_X(model || m_model, Q_FUNC_INFO, "Missing model");
            if (model)
            {
                if (model == m_model) { return; }
                if (m_model)
                {
                    m_model->disconnect();
                }

                m_model = model;
                m_model->setSelectionModel(this);
                bool c = connect(m_model, &ModelClass::modelDataChanged, this, &CViewBase::modelDataChanged);
                Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
                c = connect(m_model, &ModelClass::modelDataChangedDigest, this, &CViewBase::modelDataChangedDigest);
                Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
                c = connect(m_model, &ModelClass::objectChanged, this, &CViewBase::objectChanged);
                Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
                c = connect(m_model, &ModelClass::changed, this, &CViewBase::modelChanged);
                Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
                c = connect(m_model, &ModelClass::changed, this, &CViewBase::onModelChanged);
                Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
                Q_UNUSED(c);
            }

            this->setModel(m_model); // via QTableView
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
                m_resizeCount++; // skipped resize
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
        void CViewBase<ModelClass, ContainerType, ObjectType>::mouseOverCallback(const QModelIndex &index, bool mouseOver)
        {
            // void
            Q_UNUSED(index);
            Q_UNUSED(mouseOver);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::drawDropIndicator(bool indicator)
        {
            m_dropIndicator = indicator;
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::selectObjects(const ContainerType &selectedObjects)
        {
            Q_UNUSED(selectedObjects);
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
            static const CStatusMessage e(this, CStatusMessage::SeverityInfo, "validation passed", true);
            return e;
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::jsonLoadedAndModelUpdated(const ContainerType &data)
        {
            Q_UNUSED(data);
        }

        template<class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::customMenu(CMenuActions &menuActions)
        {
            CViewBaseNonTemplate::customMenu(menuActions);

            // Clear highlighting
            if (this->derivedModel()->hasHighlightedRows())
            {
                menuActions.addAction(CIcons::refresh16(), "Clear highlighting", CMenuAction::pathViewClearHighlighting(), nullptr , { this, &CViewBaseNonTemplate::clearHighlighting });
            }
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        CStatusMessage CViewBase<ModelClass, ContainerType, ObjectType>::ps_loadJson()
        {
            CStatusMessage m;
            do
            {
                const QString fileName = QFileDialog::getOpenFileName(nullptr,
                                         tr("Load data file"), getSettingsFileName(true),
                                         tr("swift (*.json *.txt)"));
                if (fileName.isEmpty())
                {
                    m = CStatusMessage(this).error("Load canceled, no file name");
                    break;
                }

                const QString json(CFileUtils::readFileToString(fileName));
                if (json.isEmpty())
                {
                    m = CStatusMessage(this).warning("Reading '%1' yields no data") << fileName;
                    break;
                }
                if (!Json::looksLikeSwiftJson(json))
                {
                    m = CStatusMessage(this).warning("No swift JSON '%1'") << fileName;
                    break;
                }
                try
                {
                    CVariant containerVariant;
                    containerVariant.convertFromJson(Json::jsonObjectFromString(json));
                    if (!containerVariant.canConvert<ContainerType>())
                    {
                        m = CStatusMessage(this).warning("No valid swift JSON '%1'") << fileName;
                        break;
                    }

                    ContainerType container = containerVariant.value<ContainerType>();
                    const int countBefore = container.size();
                    m = this->modifyLoadedJsonData(container);
                    if (m.isFailure()) { break; } // modification error
                    if (countBefore > 0 && container.isEmpty()) { break; }
                    m = this->validateLoadedJsonData(container);
                    if (m.isFailure()) { break; } // validaton error
                    this->updateContainerMaybeAsync(container);
                    m = CStatusMessage(this, CStatusMessage::SeverityInfo, "Reading " + fileName + " completed", true);
                    this->jsonLoadedAndModelUpdated(container);
                }
                catch (const CJsonException &ex)
                {
                    m = ex.toStatusMessage(this, QString("Reading JSON from '%1'").arg(fileName));
                    break;
                }
            }
            while (false);

            emit this->jsonLoadCompleted(m);
            return m;
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        CStatusMessage CViewBase<ModelClass, ContainerType, ObjectType>::ps_saveJson()
        {
            const QString fileName = QFileDialog::getSaveFileName(nullptr,
                                     tr("Save data file"), getSettingsFileName(false),
                                     tr("swift (*.json *.txt)"));
            if (fileName.isEmpty()) { return CStatusMessage(this, CStatusMessage::SeverityDebug, "Save canceled", true); }
            const QString json(this->toJsonString()); // save as CVariant JSON

            // keep directory for settings
            const QFileInfo file(fileName);
            const QDir fileDir(file.absoluteDir());
            if (fileDir.exists())
            {
                m_dirSettings.setAndSave(fileDir.absolutePath());
            }

            // save file
            const bool ok = CFileUtils::writeStringToFileInBackground(json, fileName);
            return ok ?
                   CStatusMessage(this, CStatusMessage::SeverityInfo, "Writing " + fileName + " in progress", true) :
                   CStatusMessage(this, CStatusMessage::SeverityError, "Writing " + fileName + " failed", true);
        }

        template<class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::ps_copy()
        {
            QClipboard *clipboard = QApplication::clipboard();
            if (!clipboard) { return; }
            if (!this->hasSelection()) { return; }
            const ContainerType selection = this->selectedObjects();
            if (selection.isEmpty()) { return; }
            const CVariant copyJson = CVariant::from(selection);
            const QString json = copyJson.toJsonString();
            clipboard->setText(json);
        }

        template<class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::ps_cut()
        {
            if (!QApplication::clipboard()) { return; }
            this->ps_copy();
            this->removeSelectedRows();
        }

        template<class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::ps_paste()
        {
            const QClipboard *clipboard = QApplication::clipboard();
            if (!clipboard) { return; }
            const QString json = clipboard->text();
            if (json.isEmpty()) { return; }
            if (!Json::looksLikeSwiftJson(json)) { return; } // no JSON
            try
            {
                ContainerType objects;
                objects.convertFromJson(json);
                if (!objects.isEmpty())
                {
                    this->insert(objects);
                }
            }
            catch (const CJsonException &ex)
            {
                Q_UNUSED(ex);
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
                if (!m_filterWidget)
                {
                    this->removeFilter();
                }
                else
                {
                    // takes the filter and triggers the filtering
                    IModelFilterProvider<ContainerType> *provider = dynamic_cast<IModelFilterProvider<ContainerType>*>(m_filterWidget);
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
        // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
        template class CViewBase<BlackGui::Models::CAircraftIcaoCodeListModel, BlackMisc::Aviation::CAircraftIcaoCodeList, BlackMisc::Aviation::CAircraftIcaoCode>;
        template class CViewBase<BlackGui::Models::CAircraftModelListModel, BlackMisc::Simulation::CAircraftModelList, BlackMisc::Simulation::CAircraftModel>;
        template class CViewBase<BlackGui::Models::CAirlineIcaoCodeListModel, BlackMisc::Aviation::CAirlineIcaoCodeList, BlackMisc::Aviation::CAirlineIcaoCode>;
        template class CViewBase<BlackGui::Models::CAirportListModel, BlackMisc::Aviation::CAirportList, BlackMisc::Aviation::CAirport>;
        template class CViewBase<BlackGui::Models::CAtcStationListModel, BlackMisc::Aviation::CAtcStationList, BlackMisc::Aviation::CAtcStation>;
        template class CViewBase<BlackGui::Models::CClientListModel, BlackMisc::Network::CClientList, BlackMisc::Network::CClient>;
        template class CViewBase<BlackGui::Models::CCloudLayerListModel, BlackMisc::Weather::CCloudLayerList, BlackMisc::Weather::CCloudLayer>;
        template class CViewBase<BlackGui::Models::CCountryListModel, BlackMisc::CCountryList, BlackMisc::CCountry>;
        template class CViewBase<BlackGui::Models::CDistributorListModel, BlackMisc::Simulation::CDistributorList, BlackMisc::Simulation::CDistributor>;
        template class CViewBase<BlackGui::Models::CIdentifierListModel, BlackMisc::CIdentifierList, BlackMisc::CIdentifier>;
        template class CViewBase<BlackGui::Models::CLiveryListModel, BlackMisc::Aviation::CLiveryList, BlackMisc::Aviation::CLivery>;
        template class CViewBase<BlackGui::Models::CMatchingStatisticsModel, BlackMisc::Simulation::CMatchingStatistics, BlackMisc::Simulation::CMatchingStatisticsEntry>;
        template class CViewBase<BlackGui::Models::CNameVariantPairModel, BlackMisc::CNameVariantPairList, BlackMisc::CNameVariantPair>;
        template class CViewBase<BlackGui::Models::CServerListModel, BlackMisc::Network::CServerList, BlackMisc::Network::CServer>;
        template class CViewBase<BlackGui::Models::CSimulatedAircraftListModel, BlackMisc::Simulation::CSimulatedAircraftList, BlackMisc::Simulation::CSimulatedAircraft>;
        template class CViewBase<BlackGui::Models::CStatusMessageListModel, BlackMisc::CStatusMessageList, BlackMisc::CStatusMessage>;
        template class CViewBase<BlackGui::Models::CTemperatureLayerListModel, BlackMisc::Weather::CTemperatureLayerList, BlackMisc::Weather::CTemperatureLayer>;
        template class CViewBase<BlackGui::Models::CTextMessageListModel, BlackMisc::Network::CTextMessageList, BlackMisc::Network::CTextMessage>;
        template class CViewBase<BlackGui::Models::CUserListModel, BlackMisc::Network::CUserList, BlackMisc::Network::CUser>;
        template class CViewBase<BlackGui::Models::CWindLayerListModel, BlackMisc::Weather::CWindLayerList, BlackMisc::Weather::CWindLayer>;
    } // namespace
} // namespace
