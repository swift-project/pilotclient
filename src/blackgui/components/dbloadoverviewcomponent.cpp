/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbloadoverviewcomponent.h"
#include "ui_dbloadoverviewcomponent.h"

#include "blackcore/webdataservices.h"
#include "blackgui/guiapplication.h"

using namespace BlackGui;
using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Components
    {
        CDbLoadOverviewComponent::CDbLoadOverviewComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDbLoadOverviewComponent)
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "missing sGui");
            Q_ASSERT_X(sGui->getWebDataServices(), Q_FUNC_INFO, "no data services");

            ui->setupUi(this);

            ui->lbl_DatabaseUrl->setTextFormat(Qt::RichText);
            ui->lbl_DatabaseUrl->setTextInteractionFlags(Qt::TextBrowserInteraction);
            ui->lbl_DatabaseUrl->setOpenExternalLinks(true);

            connect(ui->pb_ReloadAircraft, &QPushButton::pressed, this, &CDbLoadOverviewComponent::ps_reloadPressed);
            connect(ui->pb_ReloadAirlines, &QPushButton::pressed, this, &CDbLoadOverviewComponent::ps_reloadPressed);
            connect(ui->pb_ReloadCountries, &QPushButton::pressed, this, &CDbLoadOverviewComponent::ps_reloadPressed);
            connect(ui->pb_ReloadLiveries, &QPushButton::pressed, this, &CDbLoadOverviewComponent::ps_reloadPressed);
            connect(ui->pb_ReloadModels, &QPushButton::pressed, this, &CDbLoadOverviewComponent::ps_reloadPressed);
            connect(ui->pb_ReloadDistributors, &QPushButton::pressed, this, &CDbLoadOverviewComponent::ps_reloadPressed);
            connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbLoadOverviewComponent::ps_dataLoaded);

            QTimer::singleShot(2000, this, &CDbLoadOverviewComponent::ps_setValues);
        }

        CDbLoadOverviewComponent::~CDbLoadOverviewComponent()
        { }

        void CDbLoadOverviewComponent::ps_setValues()
        {
            if (!sGui) { return; }
            if (!sGui->getWebDataServices()) { return; }
            CDbLoadOverviewComponent::syncronizeCaches();

            ui->le_AircraftIcaoCacheTs->setText(cacheTimestampForEntity(CEntityFlags::AircraftIcaoEntity));
            ui->le_AirlineIcaoCacheTs->setText(cacheTimestampForEntity(CEntityFlags::AirlineIcaoEntity));
            ui->le_LiveriesCacheTs->setText(cacheTimestampForEntity(CEntityFlags::LiveryEntity));
            ui->le_ModelsCacheTs->setText(cacheTimestampForEntity(CEntityFlags::ModelEntity));
            ui->le_CountriesCacheTs->setText(cacheTimestampForEntity(CEntityFlags::CountryEntity));
            ui->le_DistributorsCacheTs->setText(cacheTimestampForEntity(CEntityFlags::DistributorEntity));

            ui->le_AircraftIcaoDbTs->setText(dbTimestampForEntity(CEntityFlags::AircraftIcaoEntity));
            ui->le_AirlineIcaoDbTs->setText(dbTimestampForEntity(CEntityFlags::AirlineIcaoEntity));
            ui->le_LiveriesDbTs->setText(dbTimestampForEntity(CEntityFlags::LiveryEntity));
            ui->le_ModelsDbTs->setText(dbTimestampForEntity(CEntityFlags::ModelEntity));
            ui->le_CountriesDbTs->setText(dbTimestampForEntity(CEntityFlags::CountryEntity));
            ui->le_DistributorsDbTs->setText(dbTimestampForEntity(CEntityFlags::DistributorEntity));

            ui->le_AircraftIcaoCacheCount->setText(cacheCountForEntity(CEntityFlags::AircraftIcaoEntity));
            ui->le_AirlineIcaoCacheCount->setText(cacheCountForEntity(CEntityFlags::AirlineIcaoEntity));
            ui->le_LiveriesCacheCount->setText(cacheCountForEntity(CEntityFlags::LiveryEntity));
            ui->le_ModelsCacheCount->setText(cacheCountForEntity(CEntityFlags::ModelEntity));
            ui->le_CountriesCacheCount->setText(cacheCountForEntity(CEntityFlags::CountryEntity));
            ui->le_DistributorsCacheCount->setText(cacheCountForEntity(CEntityFlags::DistributorEntity));

            ui->le_AircraftIcaoDbCount->setText(dbCountForEntity(CEntityFlags::AircraftIcaoEntity));
            ui->le_AirlineIcaoDbCount->setText(dbCountForEntity(CEntityFlags::AirlineIcaoEntity));
            ui->le_LiveriesDbCount->setText(dbCountForEntity(CEntityFlags::LiveryEntity));
            ui->le_ModelsDbCount->setText(dbCountForEntity(CEntityFlags::ModelEntity));
            ui->le_CountriesDbCount->setText(dbCountForEntity(CEntityFlags::CountryEntity));
            ui->le_DistributorsDbCount->setText(cacheCountForEntity(CEntityFlags::DistributorEntity));

            const QString urlHtml("<a href=\"%1\">Open</a>");
            const QString url = sGui->getGlobalSetup().getDbHomePageUrl().getFullUrl();
            ui->lbl_DatabaseUrl->setText(urlHtml.arg(url));
            ui->lbl_DatabaseUrl->setToolTip(url);
        }

        void CDbLoadOverviewComponent::showLoading()
        {
            if (!this->m_loadIndicator)
            {
                this->m_loadIndicator = new CLoadIndicator(64, 64, this);
            }
            const QPoint middle = this->rect().center();
            this->m_loadIndicator->centerLoadIndicator(middle);
            this->m_loadIndicator->startAnimation(true);
        }

        QString CDbLoadOverviewComponent::formattedTimestamp(const QDateTime &dateTime)
        {
            if (!dateTime.isValid()) { return "-"; }
            return dateTime.toUTC().toString("MM-dd hh:mm:ss");
        }

        QString CDbLoadOverviewComponent::cacheTimestampForEntity(CEntityFlags::Entity entity)
        {
            const QDateTime ts = sGui->getWebDataServices()->getCacheTimestamp(entity);
            return formattedTimestamp(ts);
        }

        QString CDbLoadOverviewComponent::dbTimestampForEntity(CEntityFlags::Entity entity)
        {
            const QDateTime ts = sGui->getWebDataServices()->getDbLatestEntityTimestamp(entity);
            return formattedTimestamp(ts);
        }

        QString CDbLoadOverviewComponent::cacheCountForEntity(CEntityFlags::Entity entity)
        {
            const int c = sGui->getWebDataServices()->getCacheCount(entity);
            return c < 0 ? "-" : QString::number(c);
        }

        QString CDbLoadOverviewComponent::dbCountForEntity(CEntityFlags::Entity entity)
        {
            const int c = sGui->getWebDataServices()->getDbInfoCount(entity);
            return c < 0 ? "-" : QString::number(c);
        }

        void CDbLoadOverviewComponent::syncronizeCaches()
        {
            sGui->getWebDataServices()->syncronizeDbCaches(CEntityFlags::AllDbEntities);
        }

        void CDbLoadOverviewComponent::ps_reloadPressed()
        {
            if (this->m_reloading) { return; }
            QObject *sender = QObject::sender();
            CEntityFlags::Entity entity = CEntityFlags::singleEntityByName(sender->objectName());
            CEntityFlags::Entity triggeredEntity = sGui->getWebDataServices()->triggerReloadFromDb(entity);
            if (triggeredEntity == CEntityFlags::NoEntity) { return; }
            this->m_reloading = true;
            this->showLoading();
        }

        void CDbLoadOverviewComponent::ps_dataLoaded(CEntityFlags::Entity entity, CEntityFlags::ReadState state, int number)
        {
            Q_UNUSED(number);
            if (!CEntityFlags::anySwiftDbEntity(entity)) { return; }
            if (state == CEntityFlags::ReadFinished || state == CEntityFlags::ReadFinishedRestricted)
            {
                if (this->m_loadIndicator) { this->m_loadIndicator->stopAnimation(); }
                this->m_reloading = false;
                this->ps_setValues();
            }
        }
    } // ns
} // ns
