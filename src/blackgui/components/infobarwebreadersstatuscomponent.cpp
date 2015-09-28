/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "infobarwebreadersstatuscomponent.h"
#include "ui_infobarwebreadersstatuscomponent.h"
#include "blackcore/webreaderflags.h"
#include "blackmisc/icons.h"

using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackCore;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Components
    {
        CInfoBarWebReadersStatusComponent::CInfoBarWebReadersStatusComponent(QWidget *parent) :
            QFrame(parent), ui(new Ui::CInfoBarWebReadersStatusComponent)
        {
            ui->setupUi(this);
            this->initLeds();
            connect(&m_timer, &QTimer::timeout, this,  &CInfoBarWebReadersStatusComponent::ps_checkServerAndData);
            m_timer.setInterval(30 * 1000);
            m_timer.start();
            m_timer.setObjectName("CInfoBarWebReadersStatusComponent::CheckSwiftDbTimer");
        }

        CInfoBarWebReadersStatusComponent::~CInfoBarWebReadersStatusComponent()
        {
            m_timer.stop();
        }

        void CInfoBarWebReadersStatusComponent::initLeds()
        {
            CLedWidget::LedShape shape = CLedWidget::Circle;
            this->ui->led_DataReady->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "all data ready", "data missing", 14);

            this->ui->led_IcaoAircraft->setValues(CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Red, shape, "reading", "idle", "failed", 14);
            this->ui->led_IcaoAirline->setValues(CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Red, shape, "reading", "idle", "failed", 14);
            this->ui->led_Countries->setValues(CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Red, shape, "reading", "idle", "failed", 14);

            this->ui->led_Models->setValues(CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Red, shape, "reading", "idle", "failed", 14);
            this->ui->led_Liveries->setValues(CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Red, shape, "reading", "idle", "failed", 14);
            this->ui->led_Distributors->setValues(CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Red, shape, "reading", "idle", "failed", 14);
        }

        void CInfoBarWebReadersStatusComponent::setProvider(IWebDataServicesProvider *webDataReaderProvider)
        {
            CWebDataServicesAware::setProvider(webDataReaderProvider);
            connectSwiftDatabaseSignals(
                this,
                std::bind(&CInfoBarWebReadersStatusComponent::ps_dataRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
            );
        }

        void CInfoBarWebReadersStatusComponent::ps_dataRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
        {
            if (readState == CEntityFlags::ReadFinished)
            {
                bool swift = CWebReaderFlags::isFromSwiftDb(entity);
                if (swift && count > 0)
                {
                    // avoids unnecessary checks
                    this->ui->led_SwiftDb->setOn(true);
                    this->m_timer.start(); // restart
                }
            }

            QList<CLedWidget *> leds = this->entityToLeds(entity);
            if (!leds.isEmpty()) { this->setLedReadStates(leds, readState); }
        }

        void CInfoBarWebReadersStatusComponent::ps_checkServerAndData()
        {
            bool swift =
                this->hasProvider() &&
                this->canConnectSwiftDb();
            this->ui->led_SwiftDb->setOn(swift);

            bool allData = hasAllData();
            this->ui->led_DataReady->setOn(allData);
        }

        void CInfoBarWebReadersStatusComponent::setLedReadStates(const QList<CLedWidget *> &leds, CEntityFlags::ReadState readState)
        {
            for (CLedWidget *led : leds)
            {
                setLedReadState(led, readState);
            }
        }

        void CInfoBarWebReadersStatusComponent::setLedReadState(CLedWidget *led, CEntityFlags::ReadState readState)
        {
            Q_ASSERT_X(led, Q_FUNC_INFO, "no LED");
            int blinkTime = 2.5 * 1000;
            switch (readState)
            {
            case CEntityFlags::ReadFinished:
                led->setOn(true, blinkTime);
                break;
            case CEntityFlags::StartRead:
                led->setOn(true);
                break;
            case CEntityFlags::ReadFailed:
                led->setTriState(2 * blinkTime);
                break;
            }
        }

        QList<CLedWidget *> CInfoBarWebReadersStatusComponent::entityToLeds(CEntityFlags::Entity entity) const
        {
            QList<CLedWidget *> leds;
            if (entity.testFlag(CEntityFlags::CountryEntity)) { leds << ui->led_Countries; }
            if (entity.testFlag(CEntityFlags::DistributorEntity)) { leds << ui->led_Distributors; }
            if (entity.testFlag(CEntityFlags::AircraftIcaoEntity)) { leds << ui->led_IcaoAircraft; }
            if (entity.testFlag(CEntityFlags::AirlineIcaoEntity)) { leds << ui->led_IcaoAirline; }
            if (entity.testFlag(CEntityFlags::LiveryEntity)) { leds << ui->led_Liveries; }
            if (entity.testFlag(CEntityFlags::ModelEntity)) { leds << ui->led_Models; }
            return leds;
        }

        bool CInfoBarWebReadersStatusComponent::hasAllData() const
        {
            if (!hasProvider()) { return false; }
            return getAirlineIcaoCodesCount() > 0 &&
                   getAircraftIcaoCodesCount() > 0 &&
                   getDistributorsCount() > 0 &&
                   getModelsCount() > 0 &&
                   getLiveriesCount() > 0;
        }

    } // namespace
} // namespace
