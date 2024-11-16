// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/dbcountrycomponent.h"

#include <QtGlobal>

#include "ui_dbcountrycomponent.h"

#include "core/application.h"
#include "core/webdataservices.h"
#include "gui/filters/countryfilterbar.h"
#include "gui/guiapplication.h"
#include "gui/views/countryview.h"
#include "gui/views/viewbase.h"

using namespace swift::core;
using namespace swift::misc::network;
using namespace swift::gui::views;

namespace swift::gui::components
{
    CDbCountryComponent::CDbCountryComponent(QWidget *parent) : QFrame(parent), ui(new Ui::CDbCountryComponent)
    {
        ui->setupUi(this);
        this->setViewWithIndicator(ui->tvp_Countries);
        ui->tvp_Countries->setResizeMode(CViewBaseNonTemplate::ResizingOnce);
        connect(ui->tvp_Countries, &CCountryView::requestNewBackendData, this, &CDbCountryComponent::onReload);

        // filter and drag and drop
        ui->tvp_Countries->setFilterWidget(ui->filter_CountryComponent);
        ui->tvp_Countries->allowDragDrop(true, false);

        connect(sApp->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbCountryComponent::onCountriesRead,
                Qt::QueuedConnection);
        this->onCountriesRead(CEntityFlags::CountryEntity, CEntityFlags::ReadFinished,
                              sGui->getWebDataServices()->getCountriesCount(), {});
    }

    CDbCountryComponent::~CDbCountryComponent() {}

    void CDbCountryComponent::onCountriesRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count,
                                              const QUrl &url)
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
} // namespace swift::gui::components
