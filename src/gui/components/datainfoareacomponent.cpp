// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/datainfoareacomponent.h"

#include <QDateTime>
#include <QIcon>
#include <QTabWidget>
#include <QtGlobal>

#include "ui_datainfoareacomponent.h"

#include "core/webdataservices.h"
#include "gui/components/dbaircraftcategorycomponent.h"
#include "gui/components/dbaircrafticaocomponent.h"
#include "gui/components/dbairlineicaocomponent.h"
#include "gui/components/dbcountrycomponent.h"
#include "gui/components/dbdistributorcomponent.h"
#include "gui/components/dbliverycomponent.h"
#include "gui/components/dbmodelcomponent.h"
#include "gui/guiapplication.h"
#include "misc/directoryutils.h"
#include "misc/icons.h"
#include "misc/logmessage.h"
#include "misc/network/entityflags.h"
#include "misc/statusmessage.h"
#include "misc/swiftdirectories.h"
#include "misc/verify.h"

using namespace swift::misc;
using namespace swift::misc::network;
using namespace swift::gui;
using namespace swift::gui::components;

namespace swift::gui::components
{
    CDataInfoAreaComponent::CDataInfoAreaComponent(QWidget *parent)
        : CInfoArea(parent), ui(new Ui::CDataInfoAreaComponent)
    {
        ui->setupUi(this);
        this->initInfoArea(); // init base class
        this->setWindowIcon(CIcons::swiftDatabase24());
        this->setTabBarPosition(QTabWidget::North);
    }

    CDataInfoAreaComponent::~CDataInfoAreaComponent() = default;

    CDbModelComponent *CDataInfoAreaComponent::getModelComponent() const { return ui->comp_DbModels; }

    CDbLiveryComponent *CDataInfoAreaComponent::getLiveryComponent() const { return ui->comp_DbLiveries; }

    CDbDistributorComponent *CDataInfoAreaComponent::getDistributorComponent() const { return ui->comp_DbDistributors; }

    CDbAircraftIcaoComponent *CDataInfoAreaComponent::getAircraftComponent() const { return ui->comp_DbAircraftIcao; }

    CDbAirlineIcaoComponent *CDataInfoAreaComponent::getAirlineComponent() const { return ui->comp_DbAirlineIcao; }

    CDbCountryComponent *CDataInfoAreaComponent::getCountryComponent() const { return ui->comp_DbCountries; }

    CDbAircraftCategoryComponent *CDataInfoAreaComponent::getAircraftCategoryComponent() const
    {
        return ui->comp_DbAircraftCategories;
    }

    bool CDataInfoAreaComponent::writeDbDataToResourceDir() const
    {
        if (!sGui || !sGui->getWebDataServices()->hasSuccesfullyConnectedSwiftDb())
        {
            CLogMessage(this).warning(u"No connection to DB yet, no new data loaded which can be written");
            return false;
        }

        // write to disk
        const bool s = sGui->getWebDataServices()->writeDbDataToDisk(CSwiftDirectories::staticDbFilesDirectory());
        if (s) { CLogMessage(this).info(u"Written DB data"); }
        else { CLogMessage(this).error(u"Cannot write DB data"); }
        return s;
    }

    bool CDataInfoAreaComponent::readDbDataFromResourceDir()
    {
        if (!sGui) { return false; }
        const CStatusMessageList msgs = sGui->getWebDataServices()->initDbCachesFromLocalResourceFiles(true);

        // info
        bool ok = false;
        if (msgs.isSuccess())
        {
            CLogMessage(this).info(u"Read DB data from directory: %1") << CSwiftDirectories::staticDbFilesDirectory();
            ui->comp_DbAircraftIcao->showLoadIndicator();
            ui->comp_DbAirlineIcao->showLoadIndicator();
            ui->comp_DbCountries->showLoadIndicator();
            ui->comp_DbDistributors->showLoadIndicator();
            ui->comp_DbLiveries->showLoadIndicator();
            ui->comp_DbModels->showLoadIndicator();
            ok = true;
        }
        else { CLogMessage::preformatted(msgs); }
        return ok;
    }

    QSize CDataInfoAreaComponent::getPreferredSizeWhenFloating(int areaIndex) const
    {
        auto area = static_cast<InfoArea>(areaIndex);
        switch (area)
        {
        case InfoAreaAircraftIcao:
        case InfoAreaAirlineIcao:
        case InfoAreaLiveries:
        case InfoAreaModels:
        case InfoAreaCountries:
        case InfoAreaAircraftCategories:
        default: return { 800, 600 };
        }
    }

    const QPixmap &CDataInfoAreaComponent::indexToPixmap(int areaIndex) const
    {
        auto area = static_cast<InfoArea>(areaIndex);
        switch (area)
        {
        case InfoAreaAircraftIcao: return CIcons::appAircraftIcao16();
        case InfoAreaAirlineIcao: return CIcons::appAirlineIcao16();
        case InfoAreaLiveries: return CIcons::appLiveries16();
        case InfoAreaDistributors: return CIcons::appDistributors16();
        case InfoAreaModels: return CIcons::appModels16();
        case InfoAreaCountries: return CIcons::appCountries16();
        case InfoAreaAircraftCategories: return CIcons::appAircraftCategories16();
        default: return CIcons::empty();
        }
    }

    void CDataInfoAreaComponent::requestUpdateOfAllDbData()
    {
        sGui->getWebDataServices()->triggerLoadingDirectlyFromDb(
            CEntityFlags::AllDbEntitiesNoInfoObjectsNoAirportsAndCategories, QDateTime());
    }

    void CDataInfoAreaComponent::requestUpdatedData(CEntityFlags::Entity entity)
    {
        bool requested = false;
        if (entity.testFlag(CEntityFlags::ModelEntity))
        {
            ui->comp_DbModels->requestUpdatedData();
            requested = true;
        }

        // sanity
        SWIFT_VERIFY_X(requested, Q_FUNC_INFO, "Entity not supported");
        Q_UNUSED(requested);
    }
} // namespace swift::gui::components
