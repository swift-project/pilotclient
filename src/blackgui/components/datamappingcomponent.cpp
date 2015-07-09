/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "datamappingcomponent.h"
#include "ui_datamappingcomponent.h"
#include "blackcore/web_datareader.h"

using namespace BlackGui::Views;
using namespace BlackCore;

namespace BlackGui
{
    namespace Components
    {
        CDataMappingComponent::CDataMappingComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDataMappingComponent)
        {
            ui->setupUi(this);
            connect(this->ui->tvp_Distributors, &CDistributorView::requestUpdate, this, &CDataMappingComponent::ps_requestModelDataUpdate);
            connect(this->ui->tvp_Liveries, &CLiveryView::requestUpdate, this, &CDataMappingComponent::ps_requestModelDataUpdate);
            connect(this->ui->tvp_Models, &CLiveryView::requestUpdate, this, &CDataMappingComponent::ps_requestModelDataUpdate);
        }

        CDataMappingComponent::~CDataMappingComponent()
        { }

        void CDataMappingComponent::readersInitialized(CWebDataReader *webReaders)
        {
            Q_ASSERT_X(webReaders, Q_FUNC_INFO, "Missing readers");
            if (this->m_webDataReader) { return; }
            if (!this->m_webReaderSignalConnections.isEmpty()) { return; }
            this->m_webDataReader = webReaders;
            this->m_webReaderSignalConnections = this->m_webDataReader->connectSwiftDatabaseSignals(
                    this, // the object here must be the same as in the bind
                    std::bind(&CDataMappingComponent::ps_aircraftIcaoCodeRead, this, std::placeholders::_1),
                    std::bind(&CDataMappingComponent::ps_airlineIcaoCodeRead, this, std::placeholders::_1),
                    std::bind(&CDataMappingComponent::ps_liveriesRead, this, std::placeholders::_1),
                    std::bind(&CDataMappingComponent::ps_distributorsRead, this, std::placeholders::_1),
                    std::bind(&CDataMappingComponent::ps_modelsRead, this, std::placeholders::_1));
        }

        void CDataMappingComponent::ps_aircraftIcaoCodeRead(int number)
        {
            Q_UNUSED(number);
        }

        void CDataMappingComponent::ps_airlineIcaoCodeRead(int number)
        {
            Q_UNUSED(number);
        }

        void CDataMappingComponent::ps_liveriesRead(int number)
        {
            Q_ASSERT_X(this->m_webDataReader, Q_FUNC_INFO, "Missing reader");
            Q_UNUSED(number);
            this->ui->tvp_Liveries->updateContainerMaybeAsync(this->m_webDataReader->getLiveries());
        }

        void CDataMappingComponent::ps_distributorsRead(int number)
        {
            Q_ASSERT_X(this->m_webDataReader, Q_FUNC_INFO, "Missing reader");
            Q_UNUSED(number);
            this->ui->tvp_Distributors->updateContainerMaybeAsync(this->m_webDataReader->getDistributors());
        }

        void CDataMappingComponent::ps_modelsRead(int number)
        {
            Q_ASSERT_X(this->m_webDataReader, Q_FUNC_INFO, "Missing reader");
            Q_UNUSED(number);
            this->ui->tvp_Models->updateContainerMaybeAsync(this->m_webDataReader->getModels());
        }

        void CDataMappingComponent::ps_requestModelDataUpdate()
        {
            Q_ASSERT_X(this->m_webDataReader, Q_FUNC_INFO, "Missing reader");
            this->m_webDataReader->readModelDataInBackground();
        }

    } // ns
} // ns
