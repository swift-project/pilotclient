/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/webdataservices.h"
#include "blackgui/components/infobarwebreadersstatuscomponent.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/verify.h"
#include "ui_infobarwebreadersstatuscomponent.h"

#include <QtGlobal>

using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackCore;
using namespace BlackMisc::Network;

namespace BlackGui::Components
{
    CInfoBarWebReadersStatusBase::CInfoBarWebReadersStatusBase(QWidget *parent) : QFrame(parent)
    {}

    void CInfoBarWebReadersStatusBase::init()
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "No sGui");
        this->initLeds();

        // connect timer
        m_timer.setInterval(30 * 1000);
        m_timer.start();
        m_timer.setObjectName("CInfoBarWebReadersStatusBase::CheckSwiftDbTimer");
        bool c = connect(&m_timer, &QTimer::timeout, this, &CInfoBarWebReadersStatusBase::checkServerAndData);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Failed connect");
        c = connect(sGui, &CGuiApplication::changedInternetAccessibility, this, &CInfoBarWebReadersStatusBase::networkAccessibilityChanged);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Failed connect");
        c = connect(sGui, &CGuiApplication::changedSwiftDbAccessibility, this, &CInfoBarWebReadersStatusBase::dbAccessibilityChanged);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Failed connect");

        if (sGui->hasWebDataServices())
        {
            c = connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CInfoBarWebReadersStatusBase::dataRead, Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Failed connect");
        }

        Q_UNUSED(c)
    }

    CInfoBarWebReadersStatusBase::~CInfoBarWebReadersStatusBase()
    {
        m_timer.stop();
    }

    void CInfoBarWebReadersStatusBase::consolidationRunning(bool running)
    {
        if (!m_ledConsolidation) { return; }
        m_ledConsolidation->setOn(running);
    }

    void CInfoBarWebReadersStatusBase::initLeds()
    {
        CLedWidget::LedShape shape = CLedWidget::Rounded;
        m_ledSwiftDb->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "DB online", "DB offline", 14);
        m_ledDataReady->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "all data ready", "data missing", 14);
        if (m_ledConsolidation) { m_ledConsolidation->setValues(CLedWidget::Blue, CLedWidget::Black, shape, "consolidation running", "idle", 14); }

        m_ledIcaoAircraft->setValues(CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Red, shape, "reading", "idle", "failed", 14);
        m_ledIcaoAirline->setValues(CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Red, shape, "reading", "idle", "failed", 14);
        m_ledCountries->setValues(CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Red, shape, "reading", "idle", "failed", 14);
        m_ledModels->setValues(CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Red, shape, "reading", "idle", "failed", 14);
        m_ledLiveries->setValues(CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Red, shape, "reading", "idle", "failed", 14);
        m_ledDistributors->setValues(CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Red, shape, "reading", "idle", "failed", 14);
    }

    void CInfoBarWebReadersStatusBase::dataRead(CEntityFlags::Entity entities, CEntityFlags::ReadState readState, int count)
    {
        Q_UNUSED(count);
        QList<CLedWidget *> leds = this->entitiesToLeds(entities);
        if (!leds.isEmpty()) { this->setLedReadStates(leds, readState); }
    }

    void CInfoBarWebReadersStatusBase::networkAccessibilityChanged(bool accessible)
    {
        Q_UNUSED(accessible);
        this->checkServerAndData();
    }

    void CInfoBarWebReadersStatusBase::dbAccessibilityChanged(bool accessible, const CUrl &testedUrl)
    {
        Q_UNUSED(accessible);
        Q_UNUSED(testedUrl);
        this->checkServerAndData();
    }

    void CInfoBarWebReadersStatusBase::checkServerAndData()
    {
        const bool swift = sGui && sGui->isSwiftDbAccessible();
        m_ledSwiftDb->setOn(swift);

        const bool allData = hasAllData();
        m_ledDataReady->setOn(allData);
    }

    void CInfoBarWebReadersStatusBase::showConsolidationStatus(bool show)
    {
        if (m_ledConsolidation)
        {
            m_ledConsolidation->setVisible(show);
        }
    }

    void CInfoBarWebReadersStatusBase::setLedReadStates(const QList<CLedWidget *> &leds, CEntityFlags::ReadState readState)
    {
        for (CLedWidget *led : leds)
        {
            this->setLedReadState(led, readState);
        }
    }

    void CInfoBarWebReadersStatusBase::setLedReadState(CLedWidget *led, CEntityFlags::ReadState readState)
    {
        Q_ASSERT_X(led, Q_FUNC_INFO, "no LED");
        const int resetTimeMs = qRound(2.5 * 1000);
        switch (readState)
        {
        case CEntityFlags::ReadSkipped:
        case CEntityFlags::ReadFinished:
        case CEntityFlags::ReadFinishedRestricted:
            led->setOn(false);
            led->blink(resetTimeMs); // blink here (temp. "on"), since read from cache results in immediate ReadFinished
            break;
        case CEntityFlags::ReadStarted:
        case CEntityFlags::ReadParsing:
            led->setOn(true);
            break;
        case CEntityFlags::ReadFailed:
            led->setTriState(2 * resetTimeMs);
            break;
        default:
            BLACK_VERIFY_X(false, Q_FUNC_INFO, "read state not handled");
            break;
        }
    }

    QList<CLedWidget *> CInfoBarWebReadersStatusBase::entitiesToLeds(CEntityFlags::Entity entities) const
    {
        QList<CLedWidget *> leds;
        if (entities.testFlag(CEntityFlags::CountryEntity)) { leds << m_ledCountries; }
        if (entities.testFlag(CEntityFlags::DistributorEntity)) { leds << m_ledDistributors; }
        if (entities.testFlag(CEntityFlags::AircraftIcaoEntity)) { leds << m_ledIcaoAircraft; }
        if (entities.testFlag(CEntityFlags::AirlineIcaoEntity)) { leds << m_ledIcaoAirline; }
        if (entities.testFlag(CEntityFlags::LiveryEntity)) { leds << m_ledLiveries; }
        if (entities.testFlag(CEntityFlags::ModelEntity)) { leds << m_ledModels; }
        return leds;
    }

    bool CInfoBarWebReadersStatusBase::hasAllData() const
    {
        if (!sGui) { return false; }
        if (!sGui->hasWebDataServices()) { return false; }
        return sGui->getWebDataServices()->getAirlineIcaoCodesCount() > 0 &&
               sGui->getWebDataServices()->getAircraftIcaoCodesCount() > 0 &&
               sGui->getWebDataServices()->getDistributorsCount() > 0 &&
               sGui->getWebDataServices()->getModelsCount() > 0 &&
               sGui->getWebDataServices()->getLiveriesCount() > 0 &&
               sGui->getWebDataServices()->getCountriesCount() > 0;
    }

    void CInfoBarWebReadersStatusBase::setLeds(
        CLedWidget *ledDb, CLedWidget *ledDataReady, CLedWidget *ledConsolidation, CLedWidget *ledIcaoAircraft,
        CLedWidget *ledIcaoAirline, CLedWidget *ledCountries, CLedWidget *ledDistributors, CLedWidget *ledLiveries, CLedWidget *ledModels)
    {
        m_ledSwiftDb = ledDb;
        m_ledDataReady = ledDataReady;
        m_ledConsolidation = ledConsolidation;
        m_ledIcaoAircraft = ledIcaoAircraft;
        m_ledIcaoAirline = ledIcaoAirline;
        m_ledCountries = ledCountries;
        m_ledDistributors = ledDistributors;
        m_ledLiveries = ledLiveries;
        m_ledModels = ledModels;
    }

    CInfoBarWebReadersStatusComponent::CInfoBarWebReadersStatusComponent(QWidget *parent) : CInfoBarWebReadersStatusBase(parent), ui(new Ui::CInfoBarWebReadersStatusComponent)
    {
        ui->setupUi(this);
        this->setLeds(ui->led_SwiftDb, ui->led_DataReady, ui->led_Consolidation, ui->led_IcaoAircraft, ui->led_IcaoAirline, ui->led_Countries, ui->led_Distributors, ui->led_Liveries, ui->led_Models);
        this->init();
    }

    CInfoBarWebReadersStatusComponent::~CInfoBarWebReadersStatusComponent()
    {}

    void CInfoBarWebReadersStatusComponent::showConsolidationStatus(bool show)
    {
        ui->lbl_Consolidation->setVisible(show);
        CInfoBarWebReadersStatusBase::showConsolidationStatus(show);
    }
} // namespace
