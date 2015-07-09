/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/logcomponent.h"
#include "blackgui/components/datamappingcomponent.h"
#include "blackgui/components/datamaininfoareacomponent.h"
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
        }

        CDataMainInfoAreaComponent::~CDataMainInfoAreaComponent()
        { }

        CLogComponent *CDataMainInfoAreaComponent::getLogComponent() const
        {
            return this->ui->comp_Log;
        }

        CDataMappingComponent *CDataMainInfoAreaComponent::getMappingComponent() const
        {
            return this->ui->comp_Mapping;
        }

        QSize CDataMainInfoAreaComponent::getPreferredSizeWhenFloating(int areaIndex) const
        {
            InfoArea area = static_cast<InfoArea>(areaIndex);
            switch (area)
            {
            case InfoAreaMapping:
            case InfoAreaSettings:
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
            case InfoAreaMapping:
                return CIcons::appMappings16();
            case InfoAreaSettings:
                return CIcons::appSettings16();
            case InfoAreaLog:
                return CIcons::appLog16();
            default:
                return CIcons::empty();
            }
        }
    } // ns
} // ns
