/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
using namespace BlackMisc::Network;
using namespace BlackGui::Views;

namespace BlackGui::Components
{
    CDbCountryComponent::CDbCountryComponent(QWidget *parent) :
        QFrame(parent),
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
    { }

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
