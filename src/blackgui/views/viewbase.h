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
#include "blackgui/filters/filterdialog.h"
#include "blackgui/filters/filterwidget.h"
#include "blackgui/models/modelfilter.h"
#include "blackgui/menudelegate.h"
#include "blackgui/loadindicator.h"
#include "blackgui/dropbase.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/icons.h"
#include "blackmisc/worker.h"
#include "blackmisc/variant.h"
#include "blackmisc/propertyindex.h"
#include <QTableView>
#include <QWizardPage>
#include <QHeaderView>
#include <QMenu>
#include <QPoint>
#include <QFont>
#include <QList>

namespace BlackGui
{
    namespace Views
    {
        //! Non templated base class, allows Q_OBJECT and signals / slots to be used
        class BLACKGUI_EXPORT CViewBaseNonTemplate :
            public QTableView,
            public BlackGui::Components::CEnableForDockWidgetInfoArea,
            public BlackGui::CDropBase
        {
            Q_OBJECT

            //! Load indicator property allows using in stylesheet
            Q_PROPERTY(bool isShowingLoadIndicator READ isShowingLoadIndicator NOTIFY loadIndicatorVisibilityChanged)

        public:
            //! Resize mode, when to resize rows / columns
            //! \remarks Using own resizing (other than QHeaderView::ResizeMode)
            enum ResizeMode
            {
                ResizingAuto,       //!< resizing when below threshold, \sa m_resizeAutoNthTime forcing only every n-th update to be resized
                ResizingOnce,       //!< only one time
                ResizingOnceSubset, //!< use a subset of the data to resize
                ResizingOff         //!< never
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
                MenuNone                 = 0,       ///< no menu
                MenuClear                = 1 << 0,  ///< allow clearing the view via menu
                MenuRefresh              = 1 << 1,  ///< allow refreshing the view via menu
                MenuBackend              = 1 << 2,  ///< allow to request data from backend
                MenuDisplayAutomatically = 1 << 3,  ///< allow to switch display automatically
                MenuFilter               = 1 << 4,  ///< filter can be opened
                MenuSave                 = 1 << 5,  ///< save as JSON
                MenuLoad                 = 1 << 6,  ///< load from JSON
                MenuLoadAndSave          = MenuLoad | MenuSave,
                MenuDefault              = MenuClear | MenuDisplayAutomatically
            };
            Q_DECLARE_FLAGS(Menu, MenuFlag)

            //! When (row count) to use asynchronous updates
            static const int ASyncRowsCountThreshold = 50;

            //! When to use pre-sizing with random elements
            static const int ResizeSubsetThreshold = 50;

            //! Clear data
            virtual void clear() = 0;

            //! Empty?
            virtual bool isEmpty() const = 0 ;

            //! Allow to drag and/or drop value objects
            virtual void allowDragDropValueObjects(bool allowDrag, bool allowDrop);

            //! \copydoc CDockWidget::allowDrop
            virtual void allowDrop(bool allow) override;

            //! \copydoc CDockWidget::isDropAllowed
            virtual bool isDropAllowed() const override;

            //! \copydoc CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea
            virtual bool setParentDockWidgetInfoArea(BlackGui::CDockWidgetInfoArea *parentDockableWidget);

            //! Resize mode
            ResizeMode getResizeMode() const { return m_resizeMode; }

            //! Set resize mode
            void setResizeMode(ResizeMode mode) { m_resizeMode = mode; }

            //! In \sa ResizingAuto mode, how often to update. "1" updates every time, "2" every 2nd time, ..
            void setAutoResizeFrequency(int updateEveryNthTime) { this->m_resizeAutoNthTime = updateEveryNthTime; }

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

            //! Number of selected rows
            int selectedRowsCount() const;

            //! Single selected row
            bool hasSingleSelectedRow() const;

            //! Multiple selected rows
            bool hasMultipleSelectedRows() const;

            //! Filter dialog
            void setFilterDialog(BlackGui::Filters::CFilterDialog *filterDialog);

            //! Set filter widget
            void setFilterWidget(BlackGui::Filters::CFilterWidget *filterDialog);

            //! Set custom menu if applicable
            void setCustomMenu(BlackGui::IMenuDelegate *menu, bool nestPreviousMenu = true);

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
            void menuAddItems(Menu menusToRemove);

            //! \copydoc QTableView::setSelectionModel();
            virtual void setSelectionModel(QItemSelectionModel *model) override;

            //! Main application window widget if any
            QWidget *mainApplicationWindowWidget() const;

        signals:
            //! Ask for new data from currently loaded data
            void requestUpdate();

            //! Load data from backend (where it makes sense)
            void requestNewBackendData();

            //! Load indicator's visibility has been changed
            void loadIndicatorVisibilityChanged(bool visible);

            //! Asynchronous update finished
            void asyncUpdateFinished();

            //! Number of elements changed
            void rowCountChanged(int count, bool withFilter);

            //! Model bas been changed
            void modelChanged();

            //! Single object was changed in model
            void objectChanged(const BlackMisc::CVariant &object, const BlackMisc::CPropertyIndex &changedIndex);

            //! Object has been clicked
            void objectClicked(const BlackMisc::CVariant &object);

            //! Object has been double clicked
            void objectDoubleClicked(const BlackMisc::CVariant &object);

            //! Object has been double clicked
            void objectSelected(const BlackMisc::CVariant &object);

        public slots:
            //! Resize to contents, strategy depends on container size
            virtual void resizeToContents();

            //! Full resizing to content, might be slow
            virtual void fullResizeToContents();

            //! Init as interactive, as this allows manually resizing
            void rowsResizeModeToInteractive();

            //! Resize mode to content
            void rowsResizeModeToContent();

            //! Show loading indicator
            void showLoadIndicator(int containerSizeDependent = -1);

            //! Hide loading indicator
            void hideLoadIndicator();

        protected:
            //! Constructor
            CViewBaseNonTemplate(QWidget *parent);

            //! Method creating the menu
            //! \remarks override this method to contribute to the menu
            virtual void customMenu(QMenu &menu) const;

            //! \copydoc QTableView::paintEvent
            virtual void paintEvent(QPaintEvent *event) override;

            //! \copydoc QTableView::showEvent
            virtual void showEvent(QShowEvent *event) override;

            //! \copydoc QTableView::dragEnterEvent
            virtual void dragEnterEvent(QDragEnterEvent *event) override;

            //! \copydoc QTableView::dragMoveEvent
            virtual void dragMoveEvent(QDragMoveEvent *event) override;

            //! \copydoc QTableView::dragLeaveEvent
            virtual void dragLeaveEvent(QDragLeaveEvent *event) override;

            //! \copydoc QTableView::dropEvent
            virtual void dropEvent(QDropEvent *event) override;

            //! Perform resizing / non slot method for template
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

            //! Center / re-center load indicator
            void centerLoadIndicator();

            //! Init default values
            void init();

            //! Default file for load/save operations
            QString getDefaultFilename() const;

            ResizeMode m_resizeMode = ResizingOnceSubset;            //!< mode
            RowsResizeMode m_rowResizeMode            = Interactive; //!< row resize mode for row height
            int m_resizeCount                         = 0;           //!< flag / counter, how many resize activities
            int m_skipResizeThreshold                 = 40;          //!< when to skip resize (rows count)
            int m_resizeAutoNthTime                   = 1;           //!< with ResizeAuto, resize every n-th time
            bool m_forceStretchLastColumnWhenResized  = false;       //!< a small table might (few columns) might to fail stretching, force again
            bool m_showingLoadIndicator               = false;       //!< showing loading indicator
            bool m_enabledLoadIndicator               = true;        //!< loading indicator enabled/disabled
            bool m_acceptClickSelection               = false;       //!< clicked
            bool m_acceptRowSelected                  = false;       //!< selection changed
            bool m_acceptDoubleClickSelection         = false;       //!< double clicked
            bool m_displayAutomatically               = true;        //!< display directly when loaded
            QWidget *m_filterWidget                   = nullptr;     //!< filter widget if any
            Menu                      m_menus         = MenuDefault; //!< Default menu settings
            BlackGui::IMenuDelegate  *m_menu          = nullptr;     //!< custom menu if any
            BlackGui::CLoadIndicator *m_loadIndicator = nullptr;     //!< load indicator if neeeded

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

            //! Save JSON
            virtual BlackMisc::CStatusMessage ps_saveJson() const = 0;

            //! Save JSON called by shortcut
            virtual void ps_saveJsonShortcut();

        private slots:
            //! Custom menu was requested
            void ps_customMenuRequested(QPoint pos);

            //! Toggle the resize mode
            void ps_toggleResizeMode(bool checked);

            //! Indicator has been updated
            void ps_updatedIndicator();

            //! Toggle auto display flag
            void ps_toggleAutoDisplay();

            //! Clear the model
            virtual void ps_clear() { this->clear(); }

        private:
            //! Set the filter widget internally
            void setFilterWidgetImpl(QWidget *filterWidget);
        };
        Q_DECLARE_OPERATORS_FOR_FLAGS(BlackGui::Views::CViewBaseNonTemplate::Menu)

        //! Base class for views
        template <class ModelClass, class ContainerType, class ObjectType> class CViewBase : public CViewBaseNonTemplate
        {
        public:
            //! Destructor
            virtual ~CViewBase() { if (this->m_model) { this->m_model->markDestroyed(); }}

            //! Model
            ModelClass *derivedModel() { return this->m_model; }

            //! Model
            const ModelClass *derivedModel() const { return this->m_model; }

            //! \copydoc BlackGui::Views::CViewBaseNonTemplate::clear
            virtual void clear() override { Q_ASSERT(this->m_model); this->m_model->clear(); }

            //! Update whole container
            int updateContainer(const ContainerType &container, bool sort = true, bool resize = true);

            //! Update whole container in background
            BlackMisc::CWorker *updateContainerAsync(const ContainerType &container, bool sort = true, bool resize = true);

            //! Based on size call sync / async update
            void updateContainerMaybeAsync(const ContainerType &container, bool sort = true, bool resize = true);

            //! Insert
            void insert(const ObjectType &value, bool resize = true);

            //! Value object at
            const ObjectType &at(const QModelIndex &index) const;

            //! Access to container
            const ContainerType &container() const;

            //! Selected objects
            ContainerType selectedObjects() const;

            //! Update selected objects
            int updateSelected(const BlackMisc::CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! Selected object (or default)
            ObjectType selectedObject() const;

            //! Remove selected rows
            int removeSelectedRows();

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

            //! Row count
            int rowCount() const;

            //! Column count
            int columnCount() const;

            //! \copydoc CViewBaseNonTemplate::isEmpty
            virtual bool isEmpty() const override;

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

        protected:
            ModelClass *m_model = nullptr; //!< corresponding model

            //! Constructor
            CViewBase(QWidget *parent, ModelClass *model = nullptr);

            //! Set the search indicator based on model
            void setSortIndicator();

            //! Standard initialization
            void standardInit(ModelClass *model = nullptr);

            //! \copydoc BlackGui::Views::CViewBaseNonTemplate::reachedResizeThreshold
            virtual bool reachedResizeThreshold(int containrerSize = -1) const override;

            //! \copydoc BlackGui::Views::CViewBaseNonTemplate::performModeBasedResizeToContent
            virtual void performModeBasedResizeToContent() override;

            //! \copydoc BlackGui::Views::CViewBaseNonTemplate::performUpdateContainer
            virtual int performUpdateContainer(const BlackMisc::CVariant &variant, bool sort, bool resize) override;

            //! \copydoc BlackGui::Views::CViewBaseNonTemplate::ps_filterDialogFinished
            //! \remarks Actually a slot, but not defined as such as the template does not support Q_OBJECT
            virtual bool ps_filterDialogFinished(int status) override;

            //! \copydoc BlackGui::Views::CViewBaseNonTemplate::ps_filterWidgetChangedFilter(bool)
            //! \remarks Actually a slot, but not defined as such as the template does not support Q_OBJECT
            virtual bool ps_filterWidgetChangedFilter(bool enabled) override;

            //! \copydoc BlackGui::Views::CViewBaseNonTemplate::ps_removeFilter
            //! \remarks Actually a slot, but not defined as such as the template does not support Q_OBJECT
            virtual void ps_removeFilter() override;

            //! \copydoc BlackGui::Views::CViewBaseNonTemplate::ps_clicked
            //! \remarks Actually a slot, but not defined as such as the template does not support Q_OBJECT
            virtual void ps_clicked(const QModelIndex &index) override;

            //! \copydoc BlackGui::Views::CViewBaseNonTemplate::ps_doubleClicked
            //! \remarks Actually a slot, but not defined as such as the template does not support Q_OBJECT
            virtual void ps_doubleClicked(const QModelIndex &index) override;

            //! \copydoc BlackGui::Views::CViewBaseNonTemplate::ps_rowSelected
            //! \remarks Actually a slot, but not defined as such as the template does not support Q_OBJECT
            virtual void ps_rowSelected(const QModelIndex &index) override;

            //! \copydoc BlackGui::Views::CViewBaseNonTemplate::ps_loadJson
            //! \remarks Actually a slot, but not defined as such as the template does not support Q_OBJECT
            virtual BlackMisc::CStatusMessage ps_loadJson() override;

            //! \copydoc BlackGui::Views::CViewBaseNonTemplate::ps_saveJson
            //! \remarks Actually a slot, but not defined as such as the template does not support Q_OBJECT
            virtual BlackMisc::CStatusMessage ps_saveJson() const override;
        };
    } // namespace
} // namespace
#endif // guard
