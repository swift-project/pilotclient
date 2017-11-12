/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_VIEWBASE_H
#define BLACKGUI_VIEWBASE_H

#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackgui/menus/menuaction.h"
#include "blackgui/models/modelfilter.h"
#include "blackgui/models/selectionmodel.h"
#include "blackgui/settings/guisettings.h"
#include "blackgui/settings/viewdirectorysettings.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/namevariantpairlist.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/variant.h"

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
class QWidget;

namespace BlackMisc { class CWorker; }
namespace BlackGui
{
    class CDockWidgetInfoArea;
    class CLoadIndicator;

    namespace Menus
    {
        class IMenuDelegate;
        class CFontMenu;
    }

    namespace Filters
    {
        class CFilterDialog;
        class CFilterWidget;
    }

    namespace Views
    {
        //! Non templated base class, allows Q_OBJECT and signals / slots to be used
        class BLACKGUI_EXPORT CViewBaseNonTemplate :
            public QTableView,
            public BlackGui::Components::CEnableForDockWidgetInfoArea
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
                ResizingAuto,    //!< resizing when below threshold, \sa m_resizeAutoNthTime forcing only every n-th update to be resized
                ResizingOnce,    //!< only one time
                PresizeSubset,   //!< use a subset of the data to resize
                ResizingAlways,  //!< always resize
                ResizingOff      //!< never
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
                MenuNone                 = 0,        //!< no menu
                MenuClear                = 1 << 0,   //!< allow clearing the view via menu
                MenuRemoveSelectedRows   = 1 << 1,   //!< allow to remove selected rows
                MenuRefresh              = 1 << 2,   //!< allow refreshing the view via menu
                MenuBackend              = 1 << 3,   //!< allow to request data from backend
                MenuDisplayAutomatically = 1 << 4,   //!< allow to switch display automatically
                MenuDisplayAutomaticallyAndRefresh = MenuDisplayAutomatically | MenuRefresh,  //!< should come together
                MenuFilter               = 1 << 5,   //!< filter can be opened
                MenuMaterializeFilter    = 1 << 6,   //!< materialize filter (filtered data become model data)
                MenuSave                 = 1 << 7,   //!< save as JSON
                MenuLoad                 = 1 << 8,   //!< load from JSON
                MenuToggleSelectionMode  = 1 << 9,   //!< allow to toggle selection mode
                MenuOrderable            = 1 << 10,  //!< items can be ordered (if container is BlackMisc::IOrderableList
                MenuCopy                 = 1 << 11,  //!< copy (for copy/paste)
                MenuPaste                = 1 << 12,  //!< paste (for copy/paste)
                MenuCut                  = 1 << 13,  //!< cut (for copy/paste)
                MenuFont                 = 1 << 14,  //!< font related menu (size)
                MenuLoadAndSave          = MenuLoad  | MenuSave,
                MenuDefault              = MenuToggleSelectionMode | MenuDisplayAutomaticallyAndRefresh | MenuFont | MenuClear,
                MenuDefaultNoClear       = MenuToggleSelectionMode | MenuDisplayAutomaticallyAndRefresh | MenuFont,
                MenuDefaultDbViews       = MenuToggleSelectionMode | MenuBackend | MenuFont,
                // special menus, should be in derived classes, but enums cannot be inherited
                // maybe shifted in the future to elsewhere
                MenuHighlightStashed     = 1 << 15,  //!< highlight stashed models
                MenuCanStashModels       = 1 << 16,  //!< stash models
                MenuStashing             = MenuHighlightStashed | MenuCanStashModels,
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
            virtual bool isEmpty() const = 0 ;

            //! Elements in container
            virtual int rowCount() const = 0;

            //! Is the corresponding model orderable, BlackMisc::Models::CListModelBaseNonTemplate::isOrderable
            virtual bool isOrderable() const = 0;

            //! \copydoc BlackGui::Models::CListModelBaseNonTemplate::setSorting
            virtual void setSorting(const BlackMisc::CPropertyIndex &propertyIndex, Qt::SortOrder order = Qt::AscendingOrder) = 0;

            //! Sort by index
            virtual void sortByPropertyIndex(const BlackMisc::CPropertyIndex &propertyIndex, Qt::SortOrder order = Qt::AscendingOrder) = 0;

            //! Allow to drag and/or drop value objects
            virtual void allowDragDrop(bool allowDrag, bool allowDrop) = 0;

            //! Drop allowed?
            virtual bool isDropAllowed() const = 0;

            //! Accept drop data?
            virtual bool acceptDrop(const QMimeData *mimeData) const = 0;

            //! \copydoc Components::CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea
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
            const QFont &getHorizontalHeaderFont() const { Q_ASSERT(this->horizontalHeader()); return this->horizontalHeader()->font(); }

            //! Horizontal font height
            int getHorizontalHeaderFontHeight() const;

            //! Selection (selected rows)
            bool hasSelection() const;

            //! Selected rows if any
            QModelIndexList selectedRows() const;

            //! Select given rows
            void selectRows(const QSet<int> &rows);

            //! Number of selected rows
            int selectedRowCount() const;

            //! Single selected row
            bool hasSingleSelectedRow() const;

            //! Multiple selected rows
            bool hasMultipleSelectedRows() const;

            //! Allows to select multiple rows
            bool allowsMultipleSelectedRows() const;

            //! Is the current selection mode allow multiple selection
            bool isCurrentlyAllowingMultipleRowSelections() const;

            //! Filter dialog
            void setFilterDialog(BlackGui::Filters::CFilterDialog *filterDialog);

            //! Filter widget if any
            QWidget *getFilterWidget() const { return m_filterWidget; }

            //! Set filter widget
            void setFilterWidget(BlackGui::Filters::CFilterWidget *filterWidget);

            //! Set custom menu if applicable
            BlackGui::Menus::IMenuDelegate *setCustomMenu(BlackGui::Menus::IMenuDelegate *menu, bool nestPreviousMenu = true);

            //! Enable loading indicator
            void enableLoadIndicator(bool enable);

            //! Showing load indicator
            bool isShowingLoadIndicator() const;

            //! Accept click selection
            void acceptClickSelection(bool accept) { m_acceptClickSelection = accept; }

            //! Accept double click selection
            void acceptDoubleClickSelection(bool accept) { m_acceptDoubleClickSelection = accept; }

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
            BlackMisc::CStatusMessage showFileLoadDialog();

            //! Show file save dialog
            BlackMisc::CStatusMessage showFileSaveDialog();

            //! Save file name (optional)
            void setSaveFileName(const QString &saveName) { m_saveFileName = saveName; }

            //! Force that columns are extended to full viewport width.
            //! Workaround as of https://stackoverflow.com/q/3433664/356726
            void setForceColumnsToMaxSize(bool force) { m_forceColumnsToMaxSize = force; }

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

            //! Object has been double clicked
            void objectSelected(const BlackMisc::CVariant &object);

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

            //! Remove selected rows
            virtual int removeSelectedRows() = 0;

            //! Clear any highlighted objects
            virtual void clearHighlighting() = 0;

            //! Materialize filter
            virtual void materializeFilter() = 0;

        protected:
            //! Constructor
            CViewBaseNonTemplate(QWidget *parent);

            //! Method creating the menu
            //! \remarks override this method to contribute to the menu
            //! \sa BlackGui::Views::CViewBaseNonTemplate::ps_customMenuRequested
            virtual void customMenu(BlackGui::Menus::CMenuActions &menuActions);

            //! \name Functions from QTableView
            //! @{
            virtual void resizeEvent(QResizeEvent *event) override;
            virtual void dragEnterEvent(QDragEnterEvent *event) override;
            virtual void dragMoveEvent(QDragMoveEvent *event) override;
            virtual void dragLeaveEvent(QDragLeaveEvent *event) override;
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
            QString getSettingsFileName(bool load) const;

            //! Init menu actions
            BlackGui::Menus::CMenuActions initMenuActions(MenuFlag menu);

            //! Set the sort indicator to the current sort column
            virtual void updateSortIndicator() = 0;

            //! From delegate indicating we are in mouse over state
            virtual void mouseOverCallback(const QModelIndex &index, bool mouseOver) = 0;

            //! Draw drop indicator
            virtual void drawDropIndicator(bool indicator) = 0;

            //! Settings have been changed
            void settingsChanged();

            QString        m_saveFileName;                                     //!< save file name (JSON)
            ResizeMode     m_resizeMode               = PresizeSubset;         //!< mode
            RowsResizeMode m_rowResizeMode            = Interactive;           //!< row resize mode for row height
            SelectionMode  m_originalSelectionMode    = this->selectionMode(); //!< Selection mode set
            int m_resizeCount                         = 0;                     //!< flag / counter, how many resize activities
            int m_skipResizeThreshold                 = 40;                    //!< when to skip resize (rows count)
            int m_resizeAutoNthTime                   = 1;                     //!< with ResizeAuto, resize every n-th time
            int m_loadIndicatorTimeoutMsDefault       = 30 * 1000;             //!< default time for timeout
            bool m_forceStretchLastColumnWhenResized  = false;                 //!< a small table might (few columns) might to fail stretching, force again
            bool m_showingLoadIndicator               = false;                 //!< showing loading indicator
            bool m_enabledLoadIndicator               = true;                  //!< loading indicator enabled/disabled
            bool m_acceptClickSelection               = false;                 //!< clicked
            bool m_acceptRowSelected                  = false;                 //!< selection changed
            bool m_acceptDoubleClickSelection         = false;                 //!< double clicked
            bool m_displayAutomatically               = true;                  //!< display directly when loaded
            bool m_enableDeleteSelectedRows           = false;                 //!< selected rows can be deleted
            bool m_dropIndicator                      = false;                 //!< draw indicator
            bool m_forceColumnsToMaxSize              = true;                  //!< force that columns are extended to full viewport width
            QWidget *m_filterWidget                   = nullptr;               //!< filter widget or dialog
            Menu m_menus                              = MenuDefault;           //!< Default menu settings
            BlackGui::Menus::IMenuDelegate *m_menu    = nullptr;               //!< custom menu if any
            BlackGui::Menus::CFontMenu *m_fontMenu    = nullptr;               //!< font menu if applicable
            BlackGui::CLoadIndicator *m_loadIndicator = nullptr;               //!< load indicator if needed
            QMap<MenuFlag, BlackGui::Menus::CMenuActions> m_menuFlagActions;   //!< initialized actions
            BlackMisc::CSettingReadOnly<BlackGui::Settings::TGeneralGui> m_guiSettings { this, &CViewBaseNonTemplate::settingsChanged }; //!< general GUI settings
            BlackMisc::CSetting<BlackGui::Settings::TViewDirectorySettings> m_dirSettings { this }; //!< directory for load/save

        protected slots:
            //! Helper method with template free signature serving as callback from threaded worker
            int ps_updateContainer(const BlackMisc::CVariant &variant, bool sort, bool resize);

            //! Display the filter dialog
            void ps_displayFilterDialog();

            //! Remove filter
            virtual void ps_removeFilter() = 0;

            //! Filter dialog finished
            virtual bool ps_filterDialogFinished(int status) = 0;

            //! Filter changed in filter widget
            virtual bool ps_filterWidgetChangedFilter(bool enabled) = 0;

            //! Index clicked
            virtual void ps_clicked(const QModelIndex &index) = 0;

            //! Index double clicked
            virtual void ps_doubleClicked(const QModelIndex &index) = 0;

            //! Row selected
            virtual void ps_rowSelected(const QModelIndex &index) = 0;

            //! Load JSON
            virtual BlackMisc::CStatusMessage ps_loadJson() = 0;

            //! Load JSON for action/menu, no return signatur
            void ps_loadJsonAction();

            //! Save JSON
            virtual BlackMisc::CStatusMessage ps_saveJson() = 0;

            //! Save JSON for action/menu, no return signatur
            void ps_saveJsonAction();

            //! Trigger reload from backend by signal requestUpdate();
            void ps_triggerReload();

            //! Trigger reload from backend by signal requestNewBackendData()
            void ps_triggerReloadFromBackend();

            //! Hide load indicator (no parameters)
            void ps_hideLoadIndicator();

            //! Copy
            virtual void ps_copy() = 0;

            //! Cut
            virtual void ps_cut() = 0;

            //! Paste
            virtual void ps_paste() = 0;

            // ------------ slots of CViewDbObjects ----------------
            // need to be declared here and overridden, as this is the only part with valid Q_OBJECT

            //! Highlight DB data
            virtual void ps_toggleHighlightDbData() {}

        private slots:
            //! Custom menu was requested
            void ps_customMenuRequested(QPoint pos);

            //! Toggle the resize mode
            void ps_toggleResizeMode(bool checked);

            //! Indicator has been updated
            void ps_updatedIndicator();

            //! Toggle auto display flag
            void ps_toggleAutoDisplay();

            //! \name Change selection modes
            //! @{
            void ps_setMultiSelection();
            void ps_setExtendedSelection();
            void ps_setSingleSelection();
            //! @}

            //! Clear the model
            virtual void ps_clear() { this->clear(); }

            //! Remove selected rows
            void ps_removeSelectedRows();

        private:
            //! Set the filter widget internally
            //! \remarks used for dialog and filter widget
            void setFilterWidgetImpl(QWidget *filterWidget);
        };
        Q_DECLARE_OPERATORS_FOR_FLAGS(BlackGui::Views::CViewBaseNonTemplate::Menu)

        //! Base class for views
        template <class ModelClass, class ContainerType, class ObjectType> class CViewBase :
            public CViewBaseNonTemplate,
            public BlackGui::Models::ISelectionModel<ContainerType>
        {
            // I cannot use Q_OBJECT here, because: Template classes are not supported by Q_OBJECT
            // and I cannot declare slots as SLOT because I have no Q_OBJECT macro

        public:
            //! Destructor
            virtual ~CViewBase() { if (m_model) { m_model->markDestroyed(); }}

            //! Model
            ModelClass *derivedModel() { return m_model; }

            //! Model
            const ModelClass *derivedModel() const { return m_model; }

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

            //! Value object at
            const ObjectType &at(const QModelIndex &index) const;

            //! Access to container
            const ContainerType &container() const;

            //! \copydoc BlackGui::Models::CListModelBase::containerOrFilteredContainer
            const ContainerType &containerOrFilteredContainer(bool *filtered = nullptr) const;

            //! \name Selection model interface
            //! @{
            virtual void selectObjects(const ContainerType &selectedObjects) override;
            virtual ContainerType selectedObjects() const override;
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
                ContainerType copy(container());
                int r = copy.removeIf(k0, v0, keysValues...);
                if (r < 1) { return 0; }
                this->updateContainerMaybeAsync(copy);
                return r;
            }

            //! \copydoc BlackMisc::CSequence::replaceOrAdd
            template <class K1, class V1>
            void replaceOrAdd(K1 key1, V1 value1, const ObjectType &replacement)
            {
                ContainerType copy(container());
                copy.replaceOrAdd(key1, value1, replacement);
                this->updateContainerMaybeAsync(copy);
            }

            //! \name Slot overrides from base class
            //! @{
            virtual int removeSelectedRows() override;
            virtual void presizeOrFullResizeToContents() override;
            virtual void clearHighlighting() override;
            virtual void materializeFilter() override;
            //! @}

            //! \name BlackGui::Views::CViewBaseNonTemplate implementations
            //! @{
            virtual void clear() override;
            virtual int rowCount() const override;
            virtual bool isEmpty() const override;
            virtual bool isOrderable() const override;
            virtual void allowDragDrop(bool allowDrag, bool allowDrop) override;
            virtual bool isDropAllowed() const override;
            virtual bool acceptDrop(const QMimeData *mimeData) const override;
            virtual void setSorting(const BlackMisc::CPropertyIndex &propertyIndex, Qt::SortOrder order = Qt::AscendingOrder) override;
            virtual void sortByPropertyIndex(const BlackMisc::CPropertyIndex &propertyIndex, Qt::SortOrder order = Qt::AscendingOrder) override;
            //! @}

            //! Column count
            int columnCount() const;

            //! Convert to JSON
            QJsonObject toJson() const;

            //! Convert to JSON string
            QString toJsonString(QJsonDocument::JsonFormat format = QJsonDocument::Indented) const;

            //! Set own name and the model's name
            virtual void setObjectName(const QString &name);

            //! Set filter and take ownership, any previously set filter will be destroyed
            void takeFilterOwnership(std::unique_ptr<BlackGui::Models::IModelFilter<ContainerType>> &filter);

            //! Removes filter and destroys filter object
            void removeFilter();

            //! Has filter set?
            bool hasFilter() const;

            //! Add the object and container type as accepted drop types CDropBase::addAcceptedMetaTypeId
            void addContainerTypesAsDropTypes(bool objectType = true, bool containerType = true);

            //! Init so items can be ordered, includes enabling drag and drop
            void initAsOrderable();

            //! Drop actions
            void setDropActions(Qt::DropActions dropActions) { Q_ASSERT(m_model); m_model->setDropActions(dropActions); }

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
            virtual bool reachedResizeThreshold(int containrerSize = -1) const override;
            virtual void performModeBasedResizeToContent() override;
            virtual int performUpdateContainer(const BlackMisc::CVariant &variant, bool sort, bool resize) override;
            virtual void updateSortIndicator() override;
            virtual void mouseOverCallback(const QModelIndex &index, bool mouseOver) override;
            virtual void drawDropIndicator(bool indicator) override;
            //! @}

            //! Modify JSON data loaded in BlackGui::Views::CViewBaseNonTemplate::ps_loadJson
            virtual BlackMisc::CStatusMessage modifyLoadedJsonData(ContainerType &data) const;

            //! Verify JSON data loaded in BlackGui::Views::CViewBaseNonTemplate::ps_loadJson
            virtual BlackMisc::CStatusMessage validateLoadedJsonData(const ContainerType &data) const;

            //! In BlackGui::Views::CViewBaseNonTemplate::ps_loadJson the view has been updated because of loaded JSON data
            //! \remark I cannot use a signal with a template parameter, so this functions serves as callback
            virtual void jsonLoadedAndModelUpdated(const ContainerType &data);

            //! \copydoc CViewBaseNonTemplate::customMenu
            virtual void customMenu(BlackGui::Menus::CMenuActions &menuActions) override;

            // --------------------------------------------- SLOTS start here -----------------------------------------

            //! \name Slot overrides from base class
            //! @{
            virtual bool ps_filterDialogFinished(int status) override;
            virtual bool ps_filterWidgetChangedFilter(bool enabled) override;
            virtual void ps_removeFilter() override;
            virtual void ps_clicked(const QModelIndex &index) override;
            virtual void ps_doubleClicked(const QModelIndex &index) override;
            virtual void ps_rowSelected(const QModelIndex &index) override;
            virtual BlackMisc::CStatusMessage ps_loadJson() override;
            virtual BlackMisc::CStatusMessage ps_saveJson() override;
            virtual void ps_copy() override;
            virtual void ps_cut() override;
            virtual void ps_paste() override;
            //! @}
        };
    } // namespace
} // namespace
#endif // guard
