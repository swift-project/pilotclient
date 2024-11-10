// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/datainfoareacomponent.h"
#include "blackgui/components/datamaininfoareacomponent.h"
#include "blackgui/components/dbaircrafticaocomponent.h"
#include "blackgui/components/dbliverycomponent.h"
#include "blackgui/components/dbmappingcomponent.h"
#include "blackgui/components/dbmodelcomponent.h"
#include "blackgui/components/logcomponent.h"
#include "misc/icons.h"
#include "misc/verify.h"
#include "ui_datamaininfoareacomponent.h"
#include <QIcon>

using namespace swift::misc;
using namespace BlackGui;
using namespace BlackGui::Components;

namespace BlackGui::Components
{
    CDataMainInfoAreaComponent::CDataMainInfoAreaComponent(QWidget *parent) : CInfoArea(parent),
                                                                              ui(new Ui::CDataMainInfoAreaComponent)
    {
        ui->setupUi(this);
        this->initInfoArea(); // init base class
        this->setWindowIcon(CIcons::swiftDatabase24());

        connect(ui->comp_Mapping, &CDbMappingComponent::filterByLivery, ui->comp_DataInfoArea->getLiveryComponent(), &CDbLiveryComponent::filter);
        connect(ui->comp_Mapping, &CDbMappingComponent::filterByAircraftIcao, ui->comp_DataInfoArea->getAircraftComponent(), &CDbAircraftIcaoComponent::filter);
        connect(ui->comp_Mapping, &CDbMappingComponent::requestUpdatedData, ui->comp_DataInfoArea, &CDataInfoAreaComponent::requestUpdatedData);

        connect(ui->comp_DataInfoArea->getModelComponent(), &CDbModelComponent::requestStash, ui->comp_Mapping, &CDbMappingComponent::stashModels);
        connect(ui->comp_Log, &CLogComponent::requestAttention, this, &CDataMainInfoAreaComponent::selectLog);

        ui->comp_Log->showFilterBar();
    }

    CDataMainInfoAreaComponent::~CDataMainInfoAreaComponent()
    {}

    CLogComponent *CDataMainInfoAreaComponent::getLogComponent() const
    {
        return ui->comp_Log;
    }

    CDbMappingComponent *CDataMainInfoAreaComponent::getMappingComponent() const
    {
        return ui->comp_Mapping;
    }

    CDataInfoAreaComponent *CDataMainInfoAreaComponent::getDataInfoAreaComponent() const
    {
        return ui->comp_DataInfoArea;
    }

    CDataSettingsComponent *CDataMainInfoAreaComponent::getDataSettingsComponent() const
    {
        return ui->comp_Settings;
    }

    void CDataMainInfoAreaComponent::displayLog()
    {
        this->selectArea(InfoAreaLog);
        SWIFT_VERIFY_X(this->getLogComponent(), Q_FUNC_INFO, "No log component");
        if (!this->getLogComponent()) { return; }
        this->getLogComponent()->displayLog();
    }

    void CDataMainInfoAreaComponent::selectLog()
    {
        this->selectArea(InfoAreaLog);
    }

    QSize CDataMainInfoAreaComponent::getPreferredSizeWhenFloating(int areaIndex) const
    {
        const InfoArea area = static_cast<InfoArea>(areaIndex);
        switch (area)
        {
        case InfoAreaData:
        case InfoAreaMapping:
        case InfoAreaSettings:
        case InfoAreaLog:
        default:
            return QSize(800, 600);
        }
    }

    const QPixmap &CDataMainInfoAreaComponent::indexToPixmap(int areaIndex) const
    {
        const InfoArea area = static_cast<InfoArea>(areaIndex);
        switch (area)
        {
        case InfoAreaData: return CIcons::appDatabase16();
        case InfoAreaMapping: return CIcons::appMappings16();
        case InfoAreaSettings: return CIcons::appSettings16();
        case InfoAreaLog: return CIcons::appLog16();
        default: return CIcons::empty();
        }
    }
} // ns
