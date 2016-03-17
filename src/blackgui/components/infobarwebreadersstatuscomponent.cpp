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
#include "blackgui/guiapplication.h"
#include "blackcore/webreaderflags.h"
#include "blackmisc/icons.h"
#include "blackmisc/verify.h"

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
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "No sGui");
            Q_ASSERT_X(sGui->hasWebDataServices(), Q_FUNC_INFO, "No web data services");
            ui->setupUi(this);
            this->initLeds();
            bool c = connect(&m_timer, &QTimer::timeout, this,  &CInfoBarWebReadersStatusComponent::ps_checkServerAndData);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Failed connect");
            m_timer.setInterval(30 * 1000);
            m_timer.start();
            m_timer.setObjectName("CInfoBarWebReadersStatusComponent::CheckSwiftDbTimer");
            c = connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CInfoBarWebReadersStatusComponent::ps_dataRead);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Failed connect");
            Q_UNUSED(c);
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

        void CInfoBarWebReadersStatusComponent::ps_dataRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
        {
            Q_UNUSED(count);
            QList<CLedWidget *> leds = this->entityToLeds(entity);
            if (!leds.isEmpty()) { this->setLedReadStates(leds, readState); }
        }

        void CInfoBarWebReadersStatusComponent::ps_checkServerAndData()
        {
            bool swift =
                sGui &&
                sGui->getWebDataServices()->canConnectSwiftDb();
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
            case CEntityFlags::ReadFinishedRestricted:
                led->setOn(true, blinkTime);
                break;
            case CEntityFlags::StartRead:
                led->setOn(true);
                break;
            case CEntityFlags::ReadFailed:
                led->setTriState(2 * blinkTime);
                break;
            default:
                BLACK_VERIFY_X(false, Q_FUNC_INFO, "read state not handled");
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
            if (!sGui) { return false; }
            return sGui->getWebDataServices()->getAirlineIcaoCodesCount() > 0 &&
                   sGui->getWebDataServices()->getAircraftIcaoCodesCount() > 0 &&
                   sGui->getWebDataServices()->getDistributorsCount() > 0 &&
                   sGui->getWebDataServices()->getModelsCount() > 0 &&
                   sGui->getWebDataServices()->getLiveriesCount() > 0;
        }

    } // namespace
} // namespace
