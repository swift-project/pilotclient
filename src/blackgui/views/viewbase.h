// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWBASE_H
#define BLACKGUI_VIEWBASE_H

#include "blackgui/models/modelfilter.h"
#include "blackgui/models/selectionmodel.h"
#include "blackgui/settings/guisettings.h"
#include "blackgui/menus/menuaction.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/namevariantpairlist.h"
#include "blackmisc/directories.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/variant.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/propertyindex.h"

#include <QAbstractItemView>
#include <QFlags>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QModelIndexList>
#include <QObject>
#include <QString>
#include <QTableView>
#include <Qt>
#include <QtGlobal>
#include <memory>

class QDragEnterEvent;
class QDragLeaveEvent;
class QDragMoveEvent;
class QFont;
class QItemSelectionModel;
class QMimeData;
class QModelIndex;
class QPaintEvent;
class QPoint;
class QShowEvent;
class QTabWidget;

namespace BlackMisc
{
    class CWorker;
}
namespace BlackGui
{
    class CDockWidgetInfoArea;
    class CLoadIndicator;

    namespace Menus
    {
        class IMenuDelegate;
        class CFontMenu;
    }

    namespace Models
    {
        class CColumns;
    }

    namespace Filters
    {
        class CFilterDialog;
        class CFilterWidget;
    }

    namespace Components
    {
        class CTextEditDialog;
    }

    namespace Views
    {
        //! Non templated base class, allows Q_OBJECT and signals / slots to be used
        class BLACKGUI_EXPORT CViewBaseNonTemplate :
            public COverlayMessagesTableView,
            public CEnableForDockWidgetInfoArea
        {
            Q_OBJECT

            //! Load indicator property allows using in stylesheet
            Q_PROPERTY(bool isShowingLoadIndicator READ isShowingLoadIndicator NOTIFY loadIndicatorVisibilityChanged)

            friend class CViewBaseItemDelegate;
            friend class CViewBaseProxyStyle;

        public:
            //! Resize mode, when to resize rows / columns
            //! \remarks Using own resizing (other than QHeaderView::ResizeMode)
            enum ResizeMode
            {
                ResizingAuto, //!< resizing when below threshold, \sa m_resizeAutoNthTime forcing only every n-th update to be resized
                ResizingOnce, //!< only one time
                PresizeSubset, //!< use a subset of the data to resize
                ResizingAlways, //!< always resize
                ResizingOff //!< never
            };

            //! How rows are resized, makes sense when \sa ResizeMode is \sa ResizingOff
            enum RowsResizeMode
            {
                Interactive,
                Content
            };

            //! Menu flags
            enum MenuFlag
            {
                MenuNone = 0, //!< no menu
                MenuClear = 1 << 0, //!< allow clearing the view via menu
                MenuRemoveSelectedRows = 1 << 1, //!< allow to remove selected rows
                MenuRefresh = 1 << 2, //!< allow refreshing the view via menu
                MenuBackend = 1 << 3, //!< allow to request data from backend
                MenuDisplayAutomatically = 1 << 4, //!< allow to switch display automatically
                MenuDisplayAutomaticallyAndRefresh = MenuDisplayAutomatically | MenuRefresh, //!< should come together
                MenuFilter = 1 << 5, //!< filter can be opened
                MenuMaterializeFilter = 1 << 6, //!< materialize filter (filtered data become model data)
                MenuSave = 1 << 7, //!< save as JSON
                MenuLoad = 1 << 8, //!< load from JSON
                MenuToggleSelectionMode = 1 << 9, //!< allow to toggle selection mode
                MenuOrderable = 1 << 10, //!< items can be ordered (if container is BlackMisc::IOrderableList
                MenuCopy = 1 << 11, //!< copy (for copy/paste)
                MenuPaste = 1 << 12, //!< paste (for copy/paste)
                MenuCut = 1 << 13, //!< cut (for copy/paste)
                MenuFont = 1 << 14, //!< font related menu (size)
                MenuLoadAndSave = MenuLoad | MenuSave,
                MenuDefault = MenuToggleSelectionMode | MenuDisplayAutomaticallyAndRefresh | MenuFont | MenuClear,
                MenuDefaultNoClear = MenuToggleSelectionMode | MenuDisplayAutomaticallyAndRefresh | MenuFont,
                MenuDefaultDbViews = MenuToggleSelectionMode | MenuBackend | MenuFont,
                // special menus, should be in derived classes, but enums cannot be inherited
                // maybe shifted in the future to elsewhere
                MenuHighlightStashed = 1 << 15, //!< highlight stashed models
                MenuCanStashModels = 1 << 16, //!< stash models
                MenuDisableModelsTemp = 1 << 17, //!< temp. disable model
                MenuStashing = MenuHighlightStashed | MenuCanStashModels,
            };
            Q_DECLARE_FLAGS(Menu, MenuFlag)

            //! When (row count) to use asynchronous updates
            static constexpr int ASyncRowsCountThreshold = 50;

            //! When to use pre-sizing with random elements
            static constexpr int ResizeSubsetThreshold = 250;

            //! When to use rows resizing (which is slow)
            //! \see rowsResizeModeToContent
            static constexpr int ResizeRowsToContentThreshold = 20;

            //! Clear data
            virtual void clear() = 0;

            //! Empty?
            virtual bool isEmpty() const = 0;

            //! Elements in container
            virtual int rowCount() const = 0;

            //! Is the corresponding model orderable, BlackMisc::Models::CListModelBaseNonTemplate::isOrderable
            virtual bool isOrderable() const = 0;

            //! \copydoc BlackGui::Models::CListModelBaseNonTemplate::setSorting
            virtual bool setSorting(const BlackMisc::CPropertyIndex &propertyIndex, Qt::SortOrder order = Qt::AscendingOrder) = 0;

            //! \copydoc BlackGui::Models::CListModelBaseNonTemplate::sortByPropertyIndex
            virtual void sortByPropertyIndex(const BlackMisc::CPropertyIndex &propertyIndex, Qt::SortOrder order = Qt::AscendingOrder) = 0;

            //! \copydoc BlackGui::Models::CListModelBaseNonTemplate::setNoSorting
            virtual void setNoSorting() = 0;

            //! \copydoc BlackGui::Models::CListModelBaseNonTemplate::getSortProperty
            virtual BlackMisc::CPropertyIndex getSortProperty() const = 0;

            //! \copydoc BlackGui::Models::CListModelBaseNonTemplate::getSortColumn
            virtual int getSortColumn() const = 0;

            //! \copydoc BlackGui::Models::CListModelBaseNonTemplate::hasValidSortColumn
            virtual bool hasValidSortColumn() const = 0;

            //! \copydoc BlackGui::Models::CListModelBaseNonTemplate::endsWithEmptyColumn
            virtual bool endsWithEmptyColumn() const = 0;

            //! \copydoc BlackGui::Models::CListModelBaseNonTemplate::getSortOrder
            virtual Qt::SortOrder getSortOrder() const = 0;

            //! Allow to drag and/or drop value objects
            virtual void allowDragDrop(bool allowDrag, bool allowDrop, bool allowDropJsonFile = false) = 0;

            //! Drop allowed?
            virtual bool isDropAllowed() const = 0;

            //! Accept drop data?
            virtual bool acceptDrop(const QMimeData *mimeData) const = 0;

            //! \copydoc CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea
            virtual bool setParentDockWidgetInfoArea(BlackGui::CDockWidgetInfoArea *parentDockableWidget) override;

            //! Resize mode
            ResizeMode getResizeMode() const { return m_resizeMode; }

            //! Set resize mode
            void setResizeMode(ResizeMode mode) { m_resizeMode = mode; }

            //! In \sa ResizingAuto mode, how often to update. "1" updates every time, "2" every 2nd time, ..
            void setAutoResizeFrequency(int updateEveryNthTime) { m_resizeAutoNthTime = updateEveryNthTime; }

            //! Display automatically (when models are loaded)
            bool displayAutomatically() const { return m_displayAutomatically; }

            //! Display automatically (when models are loaded)
            void setDisplayAutomatically(bool automatically) { m_displayAutomatically = automatically; }

            //! Header (horizontal) font
            const QFont &getHorizontalHeaderFont() const
            {
                Q_ASSERT(this->horizontalHeader());
                return this->horizontalHeader()->font();
            }

            //! Horizontal font height
            int getHorizontalHeaderFontHeight() const;

            //! Selection (selected rows)
            bool hasSelection() const;

            //! Selected rows if any
            QModelIndexList selectedRows() const;

            //! Unselected (not selected) rows if any
            virtual QModelIndexList unselectedRows() const;

            //! Select given rows
            int selectRows(const QSet<int> &rows);

            //! Number of selected rows
            int selectedRowCount() const;

            //! Unselected row count
            int unselectedRowCount() const;

            //! Single selected row
            bool hasSingleSelectedRow() const;

            //! Multiple selected rows
            bool hasMultipleSelectedRows() const;

            //! Allows to select multiple rows
            bool allowsMultipleSelectedRows() const;

            //! Is the current selection mode allow multiple selection
            bool isCurrentlyAllowingMultipleRowSelections() const;

            //! Filter dialog
            void setFilterDialog(Filters::CFilterDialog *filterDialog);

            //! Filter widget if any
            QWidget *getFilterWidget() const { return m_filterWidget; }

            //! Set filter widget
            void setFilterWidget(Filters::CFilterWidget *filterWidget);

            //! Set custom menu if applicable
            Menus::IMenuDelegate *setCustomMenu(Menus::IMenuDelegate *menu, bool nestPreviousMenu = true);

            //! Enable loading indicator
            void enableLoadIndicator(bool enable);

            //! Showing load indicator
            bool isShowingLoadIndicator() const;

            //! Accept click selection
            void acceptClickSelection(bool accept) { m_acceptClickSelection = accept; }

            //! Accept double click selection
            void acceptDoubleClickSelection(bool accept) { m_acceptDoubleClickSelection = accept; }

            //! Accept row selection
            void acceptRowSelection(bool accept) { m_acceptRowSelection = accept; }

            //! Remove given menu items
            void menuRemoveItems(Menu menusToRemove);

            //! Add given menu items
            void menuAddItems(Menu menusToAdd);

            //! Set menu items
            void setMenu(Menu menuItems) { m_menus = menuItems; }

            //! Menus
            Menu getMenu() const { return m_menus; }

            //! \copydoc QTableView::setSelectionModel
            virtual void setSelectionModel(QItemSelectionModel *model) override;

            //! Main application window widget if any
            QWidget *mainApplicationWindowWidget() const;

            //! Show file load dialog
            BlackMisc::CStatusMessage showFileLoadDialog(const QString &directory = {});

            //! Show file save dialog
            BlackMisc::CStatusMessage showFileSaveDialog(bool selectedOnly, const QString &directory = {});

            //! Save file name (optional)
            void setSaveFileName(const QString &saveName) { m_saveFileName = saveName; }

            //! Allow cache file JSON to be loaded
            bool allowCacheFileFormatJson() const { return m_allowCacheFileJson; }

            //! Enable/disable cache file format to be loaded as JSON
            void setAllowCacheFileFormatJson(bool allow) { m_allowCacheFileJson = allow; }

            //! Force that columns are extended to full viewport width.
            //! Workaround as of https://stackoverflow.com/q/3433664/356726
            //! \deprecated use setWordWrap
            void setForceColumnsToMaxSize(bool force)
            {
                this->setWordWrap(!force);
            }

            //! Resize mode
            void setHorizontalHeaderSectionResizeMode(QHeaderView::ResizeMode mode);

            //! Index of the directory we "remember"
            void setSettingsDirectoryIndex(BlackMisc::CDirectories::ColumnIndex directoryIndex) { m_dirSettingsIndex = directoryIndex; }

        signals:
            //! Ask for new data from currently loaded data
            void requestUpdate();

            //! Load data from backend (where it makes sense)
            void requestNewBackendData();

            //! Load indicator's visibility has been changed
            void loadIndicatorVisibilityChanged(bool visible);

            //! Asynchronous update finished
            void asyncUpdateFinished();

            //! Model data changed
            void modelDataChanged(int count, bool withFilter);

            //! Model data changed,
            void modelDataChangedDigest(int count, bool withFilter);

            //! Model bas been changed (means data in view have been changed)
            void modelChanged();

            //! Single object was changed in model
            void objectChanged(const BlackMisc::CVariant &object, const BlackMisc::CPropertyIndex &changedIndex);

            //! Object has been clicked
            void objectClicked(const BlackMisc::CVariant &object);

            //! Object has been double clicked
            void objectDoubleClicked(const BlackMisc::CVariant &object);

            //! Object has been selected
            void objectSelected(const BlackMisc::CVariant &object);

            //! Objects deleted from model
            void objectsDeleted(const BlackMisc::CVariant &objectContainer);

            //! JSON data load from disk completed, the BlackMisc::CStatusMessage represents the success
            void jsonLoadCompleted(const BlackMisc::CStatusMessage &msg);

        public slots:
            //! Resize to contents, strategy depends on container size
            virtual void resizeToContents();

            //! Full resizing to content, might be slow
            virtual void fullResizeToContents();

            //! Depending on CViewBaseNonTemplate::ResizeSubsetThreshold presize or fully resize
            virtual void presizeOrFullResizeToContents() = 0;

            //! Init as interactive, as this allows manually resizing
            void rowsResizeModeToInteractive();

            //! Resize mode to content
            //! \remark extremely slow for larger views (means views with many rows)
            void rowsResizeModeToContent();

            //! Set content/interactive mode based on ResizeRowsToContentThreshold
            void rowsResizeModeBasedOnThreshold(int elements);

            //! Show loading indicator
            //! \param containerSizeDependent check against resize threshold if indicator makes sense
            //! \param timeoutMs timeout the loading indicator
            //! \param processEvents force event processing to display indicator by updating GUI
            int showLoadIndicator(int containerSizeDependent = -1, int timeoutMs = -1, bool processEvents = true);

            //! Show loading indicator which can time out
            int showLoadIndicatorWithTimeout(int timeoutMs = -1, bool processEvents = true);

            //! Load indicator's default time (ms)
            void setLoadIndicatorTimeoutDefaultTime(int timeoutMs) { m_loadIndicatorTimeoutMsDefault = timeoutMs; }

            //! Underlying model changed
            void onModelChanged();

            //! Hide loading indicator
            void hideLoadIndicator(int loadingId = -1);

            //! Parameterless version of hideLoadIndicator
            void hideLoadIndicatorForced() { this->hideLoadIndicator(); }

            //! Remove selected rows
            virtual int removeSelectedRows() = 0;

            //! Clear any highlighted objects
            virtual void clearHighlighting() = 0;

            //! Materialize filter
            virtual void materializeFilter() = 0;

        protected:
            //! Constructor
            CViewBaseNonTemplate(QWidget *parent);

            //! Destructor
            virtual ~CViewBaseNonTemplate() override;

            //! Method creating the menu
            //! \remarks override this method to contribute to the menu
            //! \sa BlackGui::Views::CViewBaseNonTemplate::customMenuRequested
            virtual void customMenu(Menus::CMenuActions &menuActions);

            //! \name Functions from QTableView
            //! @{
            //! \copydoc QTableView::resizeEvent
            virtual void resizeEvent(QResizeEvent *event) override;

            //! \copydoc QTableView::dragEnterEvent
            virtual void dragEnterEvent(QDragEnterEvent *event) override;

            //! \copydoc QTableView::dragMoveEvent
            virtual void dragMoveEvent(QDragMoveEvent *event) override;

            //! \copydoc QTableView::dragLeaveEvent
            virtual void dragLeaveEvent(QDragLeaveEvent *event) override;

            //! \copydoc QTableView::dropEvent
            virtual void dropEvent(QDropEvent *event) override;
            //! @}

            //! Perform resizing (no presizing) / non slot method for template
            //! \sa CViewBaseNonTemplate::presizeOrFullResizeToContents
            virtual void performModeBasedResizeToContent() = 0;

            //! Helper method with template free signature
            //! \param variant contains the container
            //! \param sort
            //! \param resize
            virtual int performUpdateContainer(const BlackMisc::CVariant &variant, bool sort, bool resize) = 0;

            //! Skip resizing because of size?
            virtual bool reachedResizeThreshold(int containerSize = -1) const = 0;

            //! Resize or skip resize?
            virtual bool isResizeConditionMet(int containerSize = -1) const;

            //! Calculate presize count
            int getPresizeRandomElementsSize(int containerSize = -1) const;

            //! Center / re-center load indicator
            void centerLoadIndicator();

            //! Init default values
            void init();

            //! Default file for load/save operations
            QString getFileDialogFileName(bool load) const;

            //! Init menu actions
            Menus::CMenuActions initMenuActions(MenuFlag menu);

            //! Set the sort indicator to the current sort column
            virtual void updateSortIndicator() = 0;

            //! From delegate indicating we are in mouse over state
            virtual void mouseOverCallback(const QModelIndex &index, bool mouseOver) = 0;

            //! Draw drop indicator
            virtual void drawDropIndicator(bool indicator) = 0;

            //! Save JSON for action/menu, void return signatur
            void saveJsonAction();

            //! Save JSON for action/menu, void return signatur
            void saveSelectedJsonAction();

            //! Display JSON data
            virtual void displayJsonPopup() = 0;

            //! Display JSON data
            virtual void displaySelectedJsonPopup() = 0;

            //! Load JSON for action/menu, void return signatur
            void loadJsonAction();

            //! Load JSON file
            virtual BlackMisc::CStatusMessage loadJsonFile(const QString &filePath) = 0;

            //! Display the filter dialog
            void displayFilterDialog();

            //! Settings have been changed
            void settingsChanged();

            //! @{
            //! Change selection modes
            void setMultiSelection();
            void setExtendedSelection();
            void setSingleSelection();
            //! @}

            //! JSON directory
            //! \remark Default implementation, can be overridden with specifi implementation
            virtual void rememberLastJsonDirectory(const QString &selectedFileOrDir);

            //! JSON directory
            //! \remark Default implementation, can be overridden with specifi implementation
            virtual QString getRememberedLastJsonDirectory() const;

            //! @{
            //! Clipboard cut/copy/paste
            virtual void copy() = 0;
            virtual void cut() = 0;
            virtual void paste() = 0;
            //! @}

            //! Index clicked
            virtual void onClicked(const QModelIndex &index) = 0;

            //! Index double clicked
            virtual void onDoubleClicked(const QModelIndex &index) = 0;

            //! Row selected
            virtual void onRowSelected(const QModelIndex &index) = 0;

            //! Load JSON
            virtual BlackMisc::CStatusMessage loadJson(const QString &directory = {}) = 0;

            //! Save JSON
            virtual BlackMisc::CStatusMessage saveJson(bool selectedOnly = false, const QString &directory = {}) = 0;

            //! Trigger reload from backend by signal requestUpdate();
            void triggerReload();

            //! Trigger reload from backend by signal requestNewBackendData()
            void triggerReloadFromBackend();

            //! Remove filter
            virtual void removeFilter() = 0;

            //! Vertical header
            void showVerticalHeader();

            //! Filter dialog finished
            virtual bool filterDialogFinished(int status) = 0;

            //! Filter changed in filter widget
            virtual bool filterWidgetChangedFilter(bool enabled) = 0;

            //! Init text edit dialog if required and return pointer to it
            Components::CTextEditDialog *textEditDialog();

            ResizeMode m_resizeMode = PresizeSubset; //!< mode
            RowsResizeMode m_rowResizeMode = Interactive; //!< row resize mode for row height
            SelectionMode m_originalSelectionMode = this->selectionMode(); //!< Selection mode set
            int m_resizeCount = 0; //!< flag / counter, how many resize activities
            int m_skipResizeThreshold = 40; //!< when to skip resize (rows count)
            int m_resizeAutoNthTime = 1; //!< with ResizeAuto, resize every n-th time
            int m_loadIndicatorTimeoutMsDefault = 30 * 1000; //!< default time for timeout
            bool m_forceStretchLastColumnWhenResized = true; //!< a small table might (few columns) fail stretching, force again
            bool m_showingLoadIndicator = false; //!< showing loading indicator
            bool m_enabledLoadIndicator = false; //!< loading indicator enabled/disabled
            bool m_acceptClickSelection = false; //!< clicked
            bool m_acceptRowSelection = false; //!< selection changed
            bool m_acceptDoubleClickSelection = false; //!< double clicked
            bool m_displayAutomatically = true; //!< display directly when loaded
            bool m_enableDeleteSelectedRows = false; //!< selected rows can be deleted
            bool m_dropIndicator = false; //!< drop indicator
            bool m_forceColumnsToMaxSize = true; //!< force that columns are extended to full viewport width
            bool m_allowCacheFileJson = true; //!< allow Cache format JSON to be loaded
            QWidget *m_filterWidget = nullptr; //!< filter widget or dialog
            Menu m_menus = MenuDefault; //!< Default menu settings
            Menus::IMenuDelegate *m_menu = nullptr; //!< custom menu if any
            Menus::CFontMenu *m_fontMenu = nullptr; //!< font menu if applicable
            CLoadIndicator *m_loadIndicator = nullptr; //!< load indicator if needed
            Components::CTextEditDialog *m_textEditDialog = nullptr; //!< text edit dialog
            QMap<MenuFlag, Menus::CMenuActions> m_menuFlagActions; //!< initialized actions for menu flag (enum)
            QString m_saveFileName; //!< save file name (JSON)
            BlackMisc::CDirectories::ColumnIndex m_dirSettingsIndex = BlackMisc::CDirectories::IndexDirLastViewJsonOrDefault; //!< allows to set more specialized directories                             //!< remember last JSON directory, having this member allows to have specific dir
            BlackMisc::CSetting<BlackMisc::Settings::TDirectorySettings> m_dirSettings { this }; //!< directory for load/save
            BlackMisc::CSettingReadOnly<Settings::TGeneralGui> m_guiSettings { this, &CViewBaseNonTemplate::settingsChanged }; //!< general GUI settings

        private:
            //! Remove selected rows if enabled
            void removeSelectedRowsChecked();

            //! Toggle auto display flag
            void toggleAutoDisplay();

            //! Custom menu was requested
            void customMenuRequested(const QPoint &pos);

            //! Indicator timed out
            void onLoadIndicatorTimedOut();

            //! Toggle the resize mode
            void toggleResizeMode(bool checked);

            //! Toggle the resize mode
            void toggleWordWrap(bool checked);

            //! Set the filter widget internally
            //! \remarks used for dialog and filter widget
            void setFilterWidgetImpl(QWidget *filterWidget);
        };
        Q_DECLARE_OPERATORS_FOR_FLAGS(BlackGui::Views::CViewBaseNonTemplate::Menu)

        //! Base class for views
        template <class T>
        class CViewBase :
            public CViewBaseNonTemplate,
            public Models::ISelectionModel<typename T::ContainerType>
        {
            // I cannot use Q_OBJECT here, because: Template classes are not supported by Q_OBJECT
            // and I cannot declare slots as SLOT because I have no Q_OBJECT macro

        public:
            //! Model type
            using ModelClass = T;

            //! Model container type
            using ContainerType = typename T::ContainerType;

            //! Model container element type
            using ObjectType = typename T::ObjectType;

            //! Destructor
            virtual ~CViewBase()
            {
                if (m_model) { m_model->markDestroyed(); }
            }

            //! Model
            ModelClass *derivedModel() { return m_model; }

            //! Model
            const ModelClass *derivedModel() const { return m_model; }

            //! The columns
            const Models::CColumns &getColumns() const { return this->derivedModel()->getColumns(); }

            //! Update whole container
            //! \return int size after update
            int updateContainer(const ContainerType &container, bool sort = true, bool resize = true);

            //! Update whole container in background
            BlackMisc::CWorker *updateContainerAsync(const ContainerType &container, bool sort = true, bool resize = true);

            //! Based on size call sync / async update
            void updateContainerMaybeAsync(const ContainerType &container, bool sort = true, bool resize = true);

            //! Insert
            void insert(const ObjectType &value, bool resize = true);

            //! Insert
            void insert(const ContainerType &container, bool resize = true);

            //! Push back
            void push_back(const ObjectType &value, bool resize = true);

            //! Push back
            void push_back(const ContainerType &container, bool resize = true);

            //! Value object at
            const ObjectType &at(const QModelIndex &index) const;

            //! Access to container
            const ContainerType &container() const;

            //! The rows of the given objects
            QList<int> rowsOf(const ContainerType &container) const;

            //! The row of the given object
            int rowOf(const ObjectType &obj) const;

            //! \copydoc BlackGui::Models::CListModelBase::containerOrFilteredContainer
            const ContainerType &containerOrFilteredContainer(bool *filtered = nullptr) const;

            //! \name Selection model interface ISelectionModel
            //! @{

            //! \copydoc BlackGui::Models::ISelectionModel::selectObjects
            virtual void selectObjects(const ContainerType &selectedObjects) override;

            //! \copydoc BlackGui::Models::ISelectionModel::selectedObjects
            virtual ContainerType selectedObjects() const override;

            //! \copydoc BlackGui::Models::ISelectionModel::unselectedObjects
            virtual ContainerType unselectedObjects() const override;
            //! @}

            //! First selected, the only one, or default
            ObjectType firstSelectedOrDefaultObject() const;

            //! Update selected objects
            int updateSelected(const BlackMisc::CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! Update selected objects
            int updateSelected(const BlackMisc::CPropertyIndexVariantMap &vm);

            //! Selected object (or default)
            ObjectType selectedObject() const;

            //! \copydoc BlackMisc::CContainerBase::removeIf
            template <class K0, class V0, class... KeysValues>
            int removeIf(K0 k0, V0 v0, KeysValues... keysValues)
            {
                if (this->rowCount() < 1) { return 0; }
                ContainerType cp(container());
                int r = cp.removeIf(k0, v0, keysValues...);
                if (r < 1) { return 0; }
                this->updateContainerMaybeAsync(cp);
                return r;
            }

            //! \copydoc BlackMisc::CSequence::replaceOrAdd
            template <class K1, class V1>
            void replaceOrAdd(K1 key1, V1 value1, const ObjectType &replacement)
            {
                ContainerType cp(container());
                cp.replaceOrAdd(key1, value1, replacement);
                this->updateContainerMaybeAsync(cp);
            }

            //! \name Slot overrides from base class
            //! @{

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::removeSelectedRows
            virtual int removeSelectedRows() override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::presizeOrFullResizeToContents
            virtual void presizeOrFullResizeToContents() override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::clearHighlighting
            virtual void clearHighlighting() override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::materializeFilter
            virtual void materializeFilter() override;
            //! @}

            //! \name BlackGui::Views::CViewBaseNonTemplate implementations
            //! @{

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::clear
            virtual void clear() override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::rowCount
            virtual int rowCount() const override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::isEmpty
            virtual bool isEmpty() const override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::isOrderable
            virtual bool isOrderable() const override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::allowDragDrop
            virtual void allowDragDrop(bool allowDrag, bool allowDrop, bool allowDropJsonFile = false) override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::isDropAllowed
            virtual bool isDropAllowed() const override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::dropEvent
            virtual void dropEvent(QDropEvent *event) override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::acceptDrop
            virtual bool acceptDrop(const QMimeData *mimeData) const override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::setSorting
            virtual bool setSorting(const BlackMisc::CPropertyIndex &propertyIndex, Qt::SortOrder order = Qt::AscendingOrder) override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::sortByPropertyIndex
            virtual void sortByPropertyIndex(const BlackMisc::CPropertyIndex &propertyIndex, Qt::SortOrder order = Qt::AscendingOrder) override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::setNoSorting
            virtual void setNoSorting() override { m_model->setNoSorting(); }

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::getSortProperty
            virtual BlackMisc::CPropertyIndex getSortProperty() const override { return m_model->getSortProperty(); }

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::getSortColumn
            virtual int getSortColumn() const override { return m_model->getSortColumn(); }

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::hasValidSortColumn
            virtual bool hasValidSortColumn() const override { return m_model->hasValidSortColumn(); }

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::endsWithEmptyColumn
            virtual bool endsWithEmptyColumn() const override { return m_model->endsWithEmptyColumn(); }

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::getSortOrder
            virtual Qt::SortOrder getSortOrder() const override { return m_model->getSortOrder(); }
            //! @}

            //! Sort if columns or order changed
            void sort();

            //! Resort ("forced sorting")
            void resort();

            //! Column count
            int columnCount() const;

            //! Convert to JSON
            QJsonObject toJson(bool selectedOnly = false) const;

            //! Convert to JSON string
            QString toJsonString(QJsonDocument::JsonFormat format = QJsonDocument::Indented, bool selectedOnly = false) const;

            //! Set own name and the model's name
            virtual void setObjectName(const QString &name);

            //! Set filter and take ownership, any previously set filter will be destroyed
            void takeFilterOwnership(std::unique_ptr<Models::IModelFilter<ContainerType>> &filter);

            //! Has filter set?
            bool hasFilter() const;

            //! Add the object and container type as accepted drop types CDropBase::addAcceptedMetaTypeId
            void addContainerTypesAsDropTypes(bool objectType = true, bool containerType = true);

            //! Init so items can be ordered, includes enabling drag and drop
            void initAsOrderable();

            //! Drop actions
            void setDropActions(Qt::DropActions dropActions)
            {
                Q_ASSERT(m_model);
                m_model->setDropActions(dropActions);
            }

            //! Set a tab widget text based on row count, filter etc.
            void setTabWidgetViewText(QTabWidget *tw, int index);

            //! Set the widths based on the column percentages
            //! \sa CColumn::get
            void setPercentageColumnWidths();

        protected:
            ModelClass *m_model = nullptr; //!< corresponding model

            //! Constructor
            CViewBase(QWidget *parent, ModelClass *model = nullptr);

            //! Set the search indicator based on model
            void setSortIndicator();

            //! Standard initialization
            void standardInit(ModelClass *model = nullptr);

            //! \name base class implementations
            //! @{

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::reachedResizeThreshold
            virtual bool reachedResizeThreshold(int containrerSize = -1) const override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::performModeBasedResizeToContent
            virtual void performModeBasedResizeToContent() override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::performUpdateContainer
            virtual int performUpdateContainer(const BlackMisc::CVariant &variant, bool sort, bool resize) override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::updateSortIndicator
            virtual void updateSortIndicator() override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::mouseOverCallback
            virtual void mouseOverCallback(const QModelIndex &index, bool mouseOver) override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::drawDropIndicator
            virtual void drawDropIndicator(bool indicator) override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::copy
            virtual void copy() override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::cut
            virtual void cut() override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::paste
            virtual void paste() override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::displayJsonPopup
            virtual void displayJsonPopup() override { this->displayContainerAsJsonPopup(false); }

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::displaySelectedJsonPopup
            virtual void displaySelectedJsonPopup() override { this->displayContainerAsJsonPopup(true); }

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::filterDialogFinished
            virtual bool filterDialogFinished(int status) override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::filterWidgetChangedFilter
            virtual bool filterWidgetChangedFilter(bool enabled) override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::removeFilter
            virtual void removeFilter() override;
            //! @}

            //! Modify JSON data loaded in BlackGui::Views::CViewBaseNonTemplate::loadJson
            virtual BlackMisc::CStatusMessage modifyLoadedJsonData(ContainerType &data) const;

            //! Verify JSON data loaded in BlackGui::Views::CViewBaseNonTemplate::loadJson
            virtual BlackMisc::CStatusMessage validateLoadedJsonData(const ContainerType &data) const;

            //! In BlackGui::Views::CViewBaseNonTemplate::loadJson the view has been updated because of loaded JSON data
            //! \remark I cannot use a signal with a template parameter, so this functions serves as callback
            virtual void jsonLoadedAndModelUpdated(const ContainerType &data);

            //! \copydoc BlackGui::Views::CViewBaseNonTemplate::customMenu
            virtual void customMenu(Menus::CMenuActions &menuActions) override;

            //! \copydoc BlackGui::Views::CViewBaseNonTemplate::customMenu
            virtual BlackMisc::CStatusMessage loadJsonFile(const QString &fileName) override;

            //! Display the container as JSON popup
            virtual void displayContainerAsJsonPopup(bool selectedOnly);

            //! \name Overrides from base class
            //! @{

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::removeFilter
            virtual void onClicked(const QModelIndex &index) override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::onDoubleClicked
            virtual void onDoubleClicked(const QModelIndex &index) override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::onRowSelected
            virtual void onRowSelected(const QModelIndex &index) override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::loadJson
            virtual BlackMisc::CStatusMessage loadJson(const QString &directory = {}) override;

            //!\copydoc BlackGui::Views::CViewBaseNonTemplate::saveJson
            virtual BlackMisc::CStatusMessage saveJson(bool selectedOnly = false, const QString &directory = {}) override;
            //! @}
        };
    } // namespace
} // namespace
#endif // guard
