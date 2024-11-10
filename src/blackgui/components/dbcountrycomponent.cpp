// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/application.h"
#include "blackcore/webdataservices.h"
#include "blackgui/components/dbcountrycomponent.h"
#include "blackgui/filters/countryfilterbar.h"
#include "blackgui/guiapplication.h"
#include "blackgui/views/countryview.h"
#include "blackgui/views/viewbase.h"
#include "ui_dbcountrycomponent.h"

#include <QtGlobal>

using namespace BlackCore;
using namespace swift::misc::network;
using namespace BlackGui::Views;

namespace BlackGui::Components
{
    CDbCountryComponent::CDbCountryComponent(QWidget *parent) : QFrame(parent),
                                                                ui(new Ui::CDbCountryComponent)
    {
        ui->setupUi(this);
        this->setViewWithIndicator(ui->tvp_Countries);
        ui->tvp_Countries->setResizeMode(CViewBaseNonTemplate::ResizingOnce);
        connect(ui->tvp_Countries, &CCountryView::requestNewBackendData, this, &CDbCountryComponent::onReload);

        // filter and drag and drop
        ui->tvp_Countries->setFilterWidget(ui->filter_CountryComponent);
        ui->tvp_Countries->allowDragDrop(true, false);

        connect(sApp->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbCountryComponent::onCountriesRead, Qt::QueuedConnection);
        this->onCountriesRead(CEntityFlags::CountryEntity, CEntityFlags::ReadFinished, sGui->getWebDataServices()->getCountriesCount(), {});
    }

    CDbCountryComponent::~CDbCountryComponent()
    {}

    void CDbCountryComponent::onCountriesRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count, const QUrl &url)
    {
        Q_UNUSED(count)
        Q_UNUSED(url)

        if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
        if (entity.testFlag(CEntityFlags::CountryEntity) && CEntityFlags::isFinishedReadState(readState))
        {
            ui->tvp_Countries->updateContainerMaybeAsync(sApp->getWebDataServices()->getCountries());
        }
    }

    void CDbCountryComponent::onReload()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
        sApp->getWebDataServices()->triggerLoadingDirectlyFromDb(CEntityFlags::CountryEntity);
    }
} // ns
