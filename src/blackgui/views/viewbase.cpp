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
#include "../models/namevariantpairlistmodel.h"
#include "../models/atcstationlistmodel.h"
#include "../models/aircraftlistmodel.h"
#include "../models/airportlistmodel.h"
#include "../models/serverlistmodel.h"
#include "../models/userlistmodel.h"
#include "../models/clientlistmodel.h"
#include "../models/keyboardkeylistmodel.h"

#include <QHeaderView>
#include <QModelIndex>
#include <QTime>
#include <QAction>

using namespace BlackMisc;
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
        }

        void CViewBaseNonTemplate::customMenu(QMenu &menu) const
        {
            menu.addAction(BlackMisc::CIcons::refresh16(), "Update", this, SIGNAL(requestUpdate()));
            menu.addAction(BlackMisc::CIcons::delete16(), "Clear", this, SLOT(ps_clear()));
            menu.addSeparator();
            menu.addAction(BlackMisc::CIcons::resize16(), "Full resize", this, SLOT(fullResizeToContents()));

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

        void CViewBaseNonTemplate::standardInit()
        {
            int fh = qRound(1.5 * this->getHorizontalHeaderFontHeight());
            this->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive); // faster mode
            this->horizontalHeader()->setStretchLastSection(true);
            this->verticalHeader()->setDefaultSectionSize(fh);
            this->verticalHeader()->setMinimumSectionSize(fh);
            this->initRowsResizeModeToInteractive();
        }

        void CViewBaseNonTemplate::initRowsResizeModeToInteractive()
        {
            const int h = this->verticalHeader()->minimumSectionSize();
            this->setRowsResizeModeToInteractive(h);
        }

        void CViewBaseNonTemplate::setRowsResizeModeToInteractive(int height)
        {
            QHeaderView *verticalHeader = this->verticalHeader();
            Q_ASSERT(verticalHeader);
            verticalHeader->setSectionResizeMode(QHeaderView::Interactive);
            verticalHeader->setDefaultSectionSize(height);
        }

        bool CViewBaseNonTemplate::resize() const
        {
            if (m_resizeMode == ResizingOnce) { return m_resizeCount < 1; }
            if (m_resizeMode == ResizingAuto)
            {
                if (reachedResizeThreshold()) return false;
                if (m_resizeAutoNthTime < 2) return true;
                return (m_resizeCount % m_resizeAutoNthTime) == 0;
            }
            return m_resizeMode == ResizingOff;
        }

        void CViewBaseNonTemplate::fullResizeToContents()
        {
            // KWB remove
            QTime t;
            t.start();

            m_resizeCount++;
            this->resizeColumnsToContents();
            this->resizeRowsToContents();

            qDebug() << this->objectName() << "resize ms:" << t.elapsed() << QThread::currentThreadId();
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

        template <class ModelClass, class ContainerType> int CViewBase<ModelClass, ContainerType>::updateContainer(const ContainerType &container, bool sort, bool resize)
        {
            Q_ASSERT(this->m_model);
            int c = this->m_model->update(container, sort);
            if (resize) { this->resizeToContents(); }
            return c;
        }

        template <class ModelClass, class ContainerType> BlackMisc::CWorker *CViewBase<ModelClass, ContainerType>::updateContainerAsync(const ContainerType &container, bool sort, bool resize)
        {
            ModelClass *model = this->derivedModel();
            auto sortColumn = model->getSortColumn();
            auto sortOrder = model->getSortOrder();
            BlackMisc::CWorker *worker = BlackMisc::CWorker::fromTask(this, "ViewSort", [this, model, container, sort, resize, sortColumn, sortOrder]()
            {
                ContainerType sortedContainer = model->sortContainerByColumn(container, sortColumn, sortOrder);
                QMetaObject::invokeMethod(this, "updateContainer",
                    Q_ARG(QVariant, sortedContainer.toQVariant()), Q_ARG(bool, false), Q_ARG(bool, resize));
            });
            worker->then(this, &CViewBase::asyncUpdateFinished);
            return worker;
        }

        template <class ModelClass, class ContainerType> void CViewBase<ModelClass, ContainerType>::updateContainerMaybeAsync(const ContainerType &container, bool sort, bool resize)
        {
            if (container.size() > asyncThreshold && sort)
            {
                // larger container with sorting
                updateContainerAsync(container, sort, resize);
            }
            else
            {
                updateContainer(container, sort, resize);
            }
        }

        template <class ModelClass, class ContainerType> int CViewBase<ModelClass, ContainerType>::rowCount() const
        {
            Q_ASSERT(this->m_model);
            return this->m_model->rowCount();
        }

        template <class ModelClass, class ContainerType> int CViewBase<ModelClass, ContainerType>::columnCount() const
        {
            Q_ASSERT(this->m_model);
            return this->m_model->columnCount(QModelIndex());
        }

        template <class ModelClass, class ContainerType> bool CViewBase<ModelClass, ContainerType>::isEmpty() const
        {
            Q_ASSERT(this->m_model);
            return this->m_model->rowCount() < 1;
        }

        template <class ModelClass, class ContainerType> void CViewBase<ModelClass, ContainerType>::setObjectName(const QString &name)
        {
            // then name here is mainly set for debugging purposes so each model can be identified
            Q_ASSERT(m_model);
            QTableView::setObjectName(name);
            QString modelName = QString(name).append(':').append(this->m_model->getTranslationContext());
            this->m_model->setObjectName(modelName);
        }

        template <class ModelClass, class ContainerType> void CViewBase<ModelClass, ContainerType>::setSortIndicator()
        {
            if (this->m_model->hasValidSortColumn())
            {
                Q_ASSERT(this->horizontalHeader());
                this->horizontalHeader()->setSortIndicator(
                    this->m_model->getSortColumn(),
                    this->m_model->getSortOrder());
            }
        }

        template <class ModelClass, class ContainerType> void CViewBase<ModelClass, ContainerType>::standardInit(ModelClass *model)
        {
            Q_ASSERT(model || this->m_model);
            if (model)
            {
                this->m_model = model;
                connect(this->m_model, &ModelClass::rowCountChanged, this, &CViewBase::countChanged);
            }
            this->setModel(this->m_model); // via QTableView
            CViewBaseNonTemplate::standardInit();
            this->setSortIndicator();
        }

        template <class ModelClass, class ContainerType>  void CViewBase<ModelClass, ContainerType>::performResizeToContents()
        {
            // small set or large set?
            if (this->resize())
            {
                this->fullResizeToContents();
            }
            else
            {
                this->m_resizeCount++; // skipped resize
            }
        }

        template <class ModelClass, class ContainerType> int CViewBase<ModelClass, ContainerType>::performUpdateContainer(const QVariant &variant, bool sort, bool resize)
        {
            ContainerType c;
            c.convertFromQVariant(variant);
            return this->updateContainer(c, sort, resize);
        }

        // see here for the reason of thess forward instantiations
        // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
        template class CViewBase<BlackGui::Models::CStatusMessageListModel, BlackMisc::CStatusMessageList>;
        template class CViewBase<BlackGui::Models::CNameVariantPairModel, BlackMisc::CNameVariantPairList>;
        template class CViewBase<BlackGui::Models::CAtcStationListModel, BlackMisc::Aviation::CAtcStationList>;
        template class CViewBase<BlackGui::Models::CAircraftListModel, BlackMisc::Aviation::CAircraftList>;
        template class CViewBase<BlackGui::Models::CAirportListModel, BlackMisc::Aviation::CAirportList>;
        template class CViewBase<BlackGui::Models::CServerListModel, BlackMisc::Network::CServerList>;
        template class CViewBase<BlackGui::Models::CUserListModel, BlackMisc::Network::CUserList>;
        template class CViewBase<BlackGui::Models::CClientListModel, BlackMisc::Network::CClientList>;
        template class CViewBase<BlackGui::Models::CKeyboardKeyListModel, BlackMisc::Settings::CSettingKeyboardHotkeyList>;

    } // namespace
} // namespace
