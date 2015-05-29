/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "viewbase.h"
#include "../models/statusmessagelistmodel.h"
#include "../models/originatorlistmodel.h"
#include "../models/namevariantpairlistmodel.h"
#include "../models/atcstationlistmodel.h"
#include "../models/aircraftmodellistmodel.h"
#include "../models/textmessagelistmodel.h"
#include "../models/airportlistmodel.h"
#include "../models/airportlistmodel.h"
#include "../models/serverlistmodel.h"
#include "../models/userlistmodel.h"
#include "../models/clientlistmodel.h"
#include "../models/simulatedaircraftlistmodel.h"
#include "../models/keyboardkeylistmodel.h"
#include "../guiutility.h"

#include <QHeaderView>
#include <QModelIndex>
#include <QTime>
#include <QAction>
#include <QSortFilterProxyModel>
#include <QDialog>

using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Views
    {

        void CViewBaseNonTemplate::resizeToContents()
        {
            this->performResizeToContents();
        }

        CViewBaseNonTemplate::CViewBaseNonTemplate(QWidget *parent) : QTableView(parent)
        {
            this->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(this, &QWidget::customContextMenuRequested, this, &CViewBaseNonTemplate::ps_customMenuRequested);

            // scroll modes
            this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
            this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
            this->setWordWrap(true);
        }

        void CViewBaseNonTemplate::setFilterDialog(QDialog *filterDialog)
        {
            if (filterDialog)
            {
                this->m_withMenuFilter = true;
                this->m_filterDialog.reset(filterDialog);
                connect(filterDialog, &QDialog::finished, this, &CViewBaseNonTemplate::ps_filterDialogFinished);
            }
            else
            {
                if (!this->m_filterDialog.isNull()) { disconnect(this->m_filterDialog.data()); }
                this->m_withMenuFilter = false;
                this->m_filterDialog.reset(nullptr);
            }
        }

        QWidget *CViewBaseNonTemplate::mainApplicationWindowWidget() const
        {
            return CGuiUtility::mainApplicationWindowWidget();
        }

        void CViewBaseNonTemplate::customMenu(QMenu &menu) const
        {
            if (this->m_withMenuItemRefresh) { menu.addAction(BlackMisc::CIcons::refresh16(), "Update", this, SIGNAL(requestUpdate())); }
            if (this->m_withMenuItemClear) { menu.addAction(BlackMisc::CIcons::delete16(), "Clear", this, SLOT(ps_clear())); }
            if (this->m_withMenuFilter)
            {
                menu.addAction(BlackMisc::CIcons::tableSheet16(), "Filter", this, SLOT(ps_displayFilterDialog()));
                menu.addAction(BlackMisc::CIcons::tableSheet16(), "Remove Filter", this, SLOT(ps_removeFilter()));
            }
            if (!menu.isEmpty()) { menu.addSeparator(); }
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
                Q_ASSERT(false);
                break;
            }
        }

        int CViewBaseNonTemplate::ps_updateContainer(const CVariant &variant, bool sort, bool resize)
        {
            return this->performUpdateContainer(variant, sort, resize);
        }

        void CViewBaseNonTemplate::ps_displayFilterDialog()
        {
            if (!this->m_withMenuFilter) { return; }
            if (!this->m_filterDialog) { return; }
            this->m_filterDialog->show();
        }

        void CViewBaseNonTemplate::rowsResizeModeToInteractive()
        {
            const int height = this->verticalHeader()->minimumSectionSize();
            QHeaderView *verticalHeader = this->verticalHeader();
            Q_ASSERT(verticalHeader);
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

        bool CViewBaseNonTemplate::performResizing() const
        {
            if (m_resizeMode == ResizingOff) { return false; }
            if (m_resizeMode == ResizingOnce) { return m_resizeCount < 1; }
            if (m_resizeMode == ResizingAuto)
            {
                if (reachedResizeThreshold()) { return false; }
                if (m_resizeAutoNthTime < 2)  { return true; }
                return (m_resizeCount % m_resizeAutoNthTime) == 0;
            }
            return false;
        }

        void CViewBaseNonTemplate::fullResizeToContents()
        {
            m_resizeCount++;
            this->resizeColumnsToContents();
            this->resizeRowsToContents();
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

        template <class ModelClass, class ContainerType, class ObjectType> int CViewBase<ModelClass, ContainerType, ObjectType>::updateContainer(const ContainerType &container, bool sort, bool resize)
        {
            Q_ASSERT(this->m_model);
            int c = this->m_model->update(container, sort);
            if (resize) { this->resizeToContents(); }
            return c;
        }

        template <class ModelClass, class ContainerType, class ObjectType> BlackMisc::CWorker *CViewBase<ModelClass, ContainerType, ObjectType>::updateContainerAsync(const ContainerType &container, bool sort, bool resize)
        {
            ModelClass *model = this->derivedModel();
            auto sortColumn = model->getSortColumn();
            auto sortOrder = model->getSortOrder();
            BlackMisc::CWorker *worker = BlackMisc::CWorker::fromTask(this, "ViewSort", [this, model, container, sort, resize, sortColumn, sortOrder]()
            {
                ContainerType sortedContainer = model->sortContainerByColumn(container, sortColumn, sortOrder);
                QMetaObject::invokeMethod(this, "ps_updateContainer",
                                          Q_ARG(BlackMisc::CVariant, CVariant::from(sortedContainer)), Q_ARG(bool, false), Q_ARG(bool, resize));
            });
            worker->then(this, &CViewBase::asyncUpdateFinished);
            return worker;
        }

        template <class ModelClass, class ContainerType, class ObjectType> void CViewBase<ModelClass, ContainerType, ObjectType>::updateContainerMaybeAsync(const ContainerType &container, bool sort, bool resize)
        {
            if (container.size() > asyncRowsCountThreshold && sort)
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
            if (resize) { this->performResizeToContents(); }
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        const ObjectType &CViewBase<ModelClass, ContainerType, ObjectType>::at(const QModelIndex &index) const
        {
            Q_ASSERT(this->m_model);
            return this->m_model->at(index);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        const ContainerType &CViewBase<ModelClass, ContainerType, ObjectType>::getContainer() const
        {
            Q_ASSERT(this->m_model);
            return this->m_model->getContainer();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        ContainerType CViewBase<ModelClass, ContainerType, ObjectType>::selectedObjects() const
        {
            if (!this->hasSelection()) { return ContainerType(); }
            ContainerType c;
            QModelIndexList indexes = this->selectedRows();
            for (QModelIndex &i : indexes)
            {
                c.push_back(this->at(i));
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
            Q_ASSERT(model || this->m_model);
            if (model)
            {
                this->m_model = model;
                connect(this->m_model, &ModelClass::rowCountChanged, this, &CViewBase::rowCountChanged);
                connect(this->m_model, &ModelClass::objectChanged, this, &CViewBase::objectChanged);
            }

            this->setModel(this->m_model); // via QTableView
            CViewBaseNonTemplate::init();
            this->setSortIndicator();
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::performResizeToContents()
        {
            // small set or large set?
            if (this->performResizing())
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
            ContainerType c;
            c.convertFromCVariant(variant);
            return this->updateContainer(c, sort, resize);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        bool CViewBase<ModelClass, ContainerType, ObjectType>::ps_filterDialogFinished(int status)
        {
            QDialog::DialogCode statusCode = static_cast<QDialog::DialogCode>(status);
            if (statusCode == QDialog::Rejected)
            {
                this->derivedModel()->removeFilter();
                return true; // handled
            }
            return false;
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewBase<ModelClass, ContainerType, ObjectType>::ps_removeFilter()
        {
            this->derivedModel()->removeFilter();
        }

        // see here for the reason of thess forward instantiations
        // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
        template class CViewBase<BlackGui::Models::CStatusMessageListModel, BlackMisc::CStatusMessageList, BlackMisc::CStatusMessage>;
        template class CViewBase<BlackGui::Models::CNameVariantPairModel, BlackMisc::CNameVariantPairList, BlackMisc::CNameVariantPair>;
        template class CViewBase<BlackGui::Models::COriginatorListModel, BlackMisc::COriginatorList, BlackMisc::COriginator>;

        template class CViewBase<BlackGui::Models::CAtcStationListModel, BlackMisc::Aviation::CAtcStationList, BlackMisc::Aviation::CAtcStation>;
        template class CViewBase<BlackGui::Models::CAirportListModel, BlackMisc::Aviation::CAirportList, BlackMisc::Aviation::CAirport>;
        template class CViewBase<BlackGui::Models::CServerListModel, BlackMisc::Network::CServerList, BlackMisc::Network::CServer>;
        template class CViewBase<BlackGui::Models::CUserListModel, BlackMisc::Network::CUserList, BlackMisc::Network::CUser>;
        template class CViewBase<BlackGui::Models::CClientListModel, BlackMisc::Network::CClientList, BlackMisc::Network::CClient>;
        template class CViewBase<BlackGui::Models::CTextMessageListModel, BlackMisc::Network::CTextMessageList, BlackMisc::Network::CTextMessage>;
        template class CViewBase<BlackGui::Models::CSimulatedAircraftListModel, BlackMisc::Simulation::CSimulatedAircraftList, BlackMisc::Simulation::CSimulatedAircraft>;
        template class CViewBase<BlackGui::Models::CAircraftModelListModel, BlackMisc::Simulation::CAircraftModelList, BlackMisc::Simulation::CAircraftModel>;
        template class CViewBase<BlackGui::Models::CKeyboardKeyListModel, BlackMisc::Settings::CSettingKeyboardHotkeyList, BlackMisc::Settings::CSettingKeyboardHotkey>;

    } // namespace
} // namespace
