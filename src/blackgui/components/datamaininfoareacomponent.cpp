/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/logcomponent.h"
#include "blackgui/components/datamaininfoareacomponent.h"
#include "blackgui/components/dbliverycomponent.h"
#include "blackgui/components/dbaircrafticaocomponent.h"
#include "blackgui/components/dbstashcomponent.h"
#include "ui_datamaininfoareacomponent.h"
#include "blackmisc/icons.h"

using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackGui::Components;

namespace BlackGui
{
    namespace Components
    {
        CDataMainInfoAreaComponent::CDataMainInfoAreaComponent(QWidget *parent) :
            CInfoArea(parent),
            ui(new Ui::CDataMainInfoAreaComponent)
        {
            ui->setupUi(this);
            initInfoArea(); // init base class
            this->setWindowIcon(CIcons::swiftDatabase24());

            connect(ui->comp_Mapping, &CDbMappingComponent::filterByLivery, ui->comp_DataInfoArea->getLiveryComponent(), &CDbLiveryComponent::filter);
            connect(ui->comp_Mapping, &CDbMappingComponent::filterByAircraftIcao, ui->comp_DataInfoArea->getAircraftComponent(), &CDbAircraftIcaoComponent::filter);
        }

        CDataMainInfoAreaComponent::~CDataMainInfoAreaComponent()
        { }

        CLogComponent *CDataMainInfoAreaComponent::getLogComponent() const
        {
            return this->ui->comp_Log;
        }

        CDbMappingComponent *CDataMainInfoAreaComponent::getMappingComponent() const
        {
            return this->ui->comp_Mapping;
        }

        CDataInfoAreaComponent *CDataMainInfoAreaComponent::getDataInfoAreaComponent() const
        {
            return this->ui->comp_DataInfoArea;
        }

        CDbStashComponent *CDataMainInfoAreaComponent::getStashComponent() const
        {
            return this->ui->comp_Stash;
        }

        void CDataMainInfoAreaComponent::setProvider(BlackMisc::Network::IWebDataServicesProvider *provider)
        {
            Q_ASSERT_X(provider, Q_FUNC_INFO, "Missing provider");
            this->ui->comp_DataInfoArea->setProvider(provider);
            this->ui->comp_Mapping->setProvider(provider);
            this->ui->comp_Stash->setProvider(provider);
        }

        QSize CDataMainInfoAreaComponent::getPreferredSizeWhenFloating(int areaIndex) const
        {
            InfoArea area = static_cast<InfoArea>(areaIndex);
            switch (area)
            {
            case InfoAreaData:
            case InfoAreaMapping:
            case InfoAreaSettings:
            case InfoAreaStash:
            case InfoAreaLog:
            default:
                return QSize(800, 600);
            }
        }

        const QPixmap &CDataMainInfoAreaComponent::indexToPixmap(int areaIndex) const
        {
            InfoArea area = static_cast<InfoArea>(areaIndex);
            switch (area)
            {
            case InfoAreaData:
                return CIcons::appDatabase16();
            case InfoAreaMapping:
                return CIcons::appMappings16();
            case InfoAreaSettings:
                return CIcons::appSettings16();
            case InfoAreaLog:
                return CIcons::appLog16();
            case InfoAreaStash:
                return CIcons::appDbStash16();
            default:
                return CIcons::empty();
            }
        }
    } // ns
} // ns
