/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbcountrycomponent.h"
#include "ui_dbcountrycomponent.h"
#include "blackcore/webdataservices.h"
#include <functional>

using namespace BlackCore;
using namespace BlackMisc::Network;
using namespace BlackGui::Views;

namespace BlackGui
{
    namespace Components
    {
        CDbCountryComponent::CDbCountryComponent(QWidget *parent) :
            QFrame(parent),
            CWebDataServicesAware(nullptr),
            ui(new Ui::CDbCountryComponent)
        {
            ui->setupUi(this);
            this->ui->tvp_Countries->setResizeMode(CViewBaseNonTemplate::ResizingOnce);
            connect(this->ui->tvp_Countries, &CCountryView::requestNewBackendData, this, &CDbCountryComponent::ps_reload);

            // filter and drag and drop
            this->ui->tvp_Countries->setFilterWidget(this->ui->filter_CountryComponent);
            this->ui->tvp_Countries->allowDragDropValueObjects(true, false);
        }

        CDbCountryComponent::~CDbCountryComponent()
        { }

        void CDbCountryComponent::setProvider(IWebDataServicesProvider *webDataReaderProvider)
        {
            CWebDataServicesAware::setProvider(webDataReaderProvider);
            connectDataReadSignal(
                this,
                std::bind(&CDbCountryComponent::ps_countriesRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
            );
            int c = getCountriesCount();
            if (c > 0)
            {
                ps_countriesRead(CEntityFlags::CountryEntity, CEntityFlags::ReadFinished, c);
            }
        }

        void CDbCountryComponent::ps_countriesRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
        {
            Q_UNUSED(count);
            if (entity.testFlag(CEntityFlags::CountryEntity) && readState == CEntityFlags::ReadFinished)
            {
                this->ui->tvp_Countries->updateContainerMaybeAsync(this->getCountries());
            }
        }

        void CDbCountryComponent::ps_reload()
        {
            if (!hasProvider()) { return; }
            triggerRead(CEntityFlags::CountryEntity);
        }
    } // ns
} // ns
