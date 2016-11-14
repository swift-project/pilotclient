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
            admitCaches(); // in background

            ui->lbl_DatabaseUrl->setTextFormat(Qt::RichText);
            ui->lbl_DatabaseUrl->setTextInteractionFlags(Qt::TextBrowserInteraction);
            ui->lbl_DatabaseUrl->setOpenExternalLinks(true);
            ui->lbl_SharedUrls->setTextFormat(Qt::RichText);
            ui->lbl_SharedUrls->setTextInteractionFlags(Qt::TextBrowserInteraction);
            ui->lbl_SharedUrls->setOpenExternalLinks(true);

            connect(ui->tb_ReloadAircraft, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_reloadPressed);
            connect(ui->tb_ReloadAirlines, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_reloadPressed);
            connect(ui->tb_ReloadAirports, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_reloadPressed);
            connect(ui->tb_ReloadCountries, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_reloadPressed);
            connect(ui->tb_ReloadLiveries, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_reloadPressed);
            connect(ui->tb_ReloadModels, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_reloadPressed);
            connect(ui->tb_ReloadDistributors, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_reloadPressed);
            connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbLoadOverviewComponent::ps_dataLoaded);
        }

        CDbLoadOverviewComponent::~CDbLoadOverviewComponent()
        { }

        void CDbLoadOverviewComponent::display()
        {
            this->ps_setValues();
        }

        void CDbLoadOverviewComponent::ps_setValues()
        {
            if (!sGui) { return; }
            if (!sGui->hasWebDataServices()) { return; }

            ui->le_AircraftIcaoCacheTs->setText(cacheTimestampForEntity(CEntityFlags::AircraftIcaoEntity));
            ui->le_AirlinesIcaoCacheTs->setText(cacheTimestampForEntity(CEntityFlags::AirlineIcaoEntity));
            ui->le_AirportsCacheTs->setText(cacheTimestampForEntity(CEntityFlags::AirportEntity));
            ui->le_LiveriesCacheTs->setText(cacheTimestampForEntity(CEntityFlags::LiveryEntity));
            ui->le_ModelsCacheTs->setText(cacheTimestampForEntity(CEntityFlags::ModelEntity));
            ui->le_CountriesCacheTs->setText(cacheTimestampForEntity(CEntityFlags::CountryEntity));
            ui->le_DistributorsCacheTs->setText(cacheTimestampForEntity(CEntityFlags::DistributorEntity));

            ui->le_AircraftIcaoDbTs->setText(dbTimestampForEntity(CEntityFlags::AircraftIcaoEntity));
            ui->le_AirlinesIcaoDbTs->setText(dbTimestampForEntity(CEntityFlags::AirlineIcaoEntity));
            ui->le_AirportsDbTs->setText(dbTimestampForEntity(CEntityFlags::AirportEntity));
            ui->le_LiveriesDbTs->setText(dbTimestampForEntity(CEntityFlags::LiveryEntity));
            ui->le_ModelsDbTs->setText(dbTimestampForEntity(CEntityFlags::ModelEntity));
            ui->le_CountriesDbTs->setText(dbTimestampForEntity(CEntityFlags::CountryEntity));
            ui->le_DistributorsDbTs->setText(dbTimestampForEntity(CEntityFlags::DistributorEntity));

            ui->le_AircraftIcaoCacheCount->setText(cacheCountForEntity(CEntityFlags::AircraftIcaoEntity));
            ui->le_AirlinesIcaoCacheCount->setText(cacheCountForEntity(CEntityFlags::AirlineIcaoEntity));
            ui->le_AirportsCacheCount->setText(cacheCountForEntity(CEntityFlags::AirportEntity));
            ui->le_LiveriesCacheCount->setText(cacheCountForEntity(CEntityFlags::LiveryEntity));
            ui->le_ModelsCacheCount->setText(cacheCountForEntity(CEntityFlags::ModelEntity));
            ui->le_CountriesCacheCount->setText(cacheCountForEntity(CEntityFlags::CountryEntity));
            ui->le_DistributorsCacheCount->setText(cacheCountForEntity(CEntityFlags::DistributorEntity));

            ui->le_AircraftIcaoDbCount->setText(dbCountForEntity(CEntityFlags::AircraftIcaoEntity));
            ui->le_AirlinesIcaoDbCount->setText(dbCountForEntity(CEntityFlags::AirlineIcaoEntity));
            ui->le_AirportsDbCount->setText(dbCountForEntity(CEntityFlags::AirportEntity));
            ui->le_LiveriesDbCount->setText(dbCountForEntity(CEntityFlags::LiveryEntity));
            ui->le_ModelsDbCount->setText(dbCountForEntity(CEntityFlags::ModelEntity));
            ui->le_CountriesDbCount->setText(dbCountForEntity(CEntityFlags::CountryEntity));
            ui->le_DistributorsDbCount->setText(dbCountForEntity(CEntityFlags::DistributorEntity));

            ui->le_AircraftSharedFileTs->setText(sharedFileTimestampForEntity(CEntityFlags::AircraftIcaoEntity));
            ui->le_AirlinesSharedFileTs->setText(sharedFileTimestampForEntity(CEntityFlags::AirlineIcaoEntity));
            ui->le_AirportsSharedFileTs->setText(sharedFileTimestampForEntity(CEntityFlags::AirportEntity));
            ui->le_LiveriesSharedFileTs->setText(sharedFileTimestampForEntity(CEntityFlags::LiveryEntity));
            ui->le_ModelsSharedFileTs->setText(sharedFileTimestampForEntity(CEntityFlags::ModelEntity));
            ui->le_CountriesSharedFileTs->setText(sharedFileTimestampForEntity(CEntityFlags::CountryEntity));
            ui->le_DistributorsSharedFileTs->setText(sharedFileTimestampForEntity(CEntityFlags::DistributorEntity));

            // DB URL
            const QString dbUrlHtml("<a href=\"%1\">%2</a>");
            QString url = sGui->getGlobalSetup().getDbHomePageUrl().getFullUrl();
            ui->lbl_DatabaseUrl->setText(dbUrlHtml.arg(url, url));
            ui->lbl_DatabaseUrl->setToolTip(url);

            // Shared URLs
            const CUrlList sharedUrls(sGui->getGlobalSetup().getSwiftSharedUrls());
            const QString sharedUrlHtml("<a href=\"%1\">%2</a>");

            QString sharedUrlsHtml;
            for (const CUrl &url : sharedUrls)
            {
                sharedUrlsHtml += sharedUrlHtml.arg(url.getFullUrl(), url.getHost());
                sharedUrlsHtml += " ";
            }
            ui->lbl_SharedUrls->setText(sharedUrlsHtml.trimmed());
            ui->lbl_SharedUrls->setToolTip(sharedUrls.toQString());

            // Indicator
            if (this->m_loadIndicator) { this->m_loadIndicator->stopAnimation(); }
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

        bool CDbLoadOverviewComponent::isInitialized() const
        {
            return !ui->le_AircraftIcaoCacheCount->text().isEmpty();
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

        QString CDbLoadOverviewComponent::sharedFileTimestampForEntity(CEntityFlags::Entity entity)
        {
            const QDateTime ts = sGui->getWebDataServices()->getSharedFileTimestamp(entity);
            return formattedTimestamp(ts);
        }

        QString CDbLoadOverviewComponent::dbCountForEntity(CEntityFlags::Entity entity)
        {
            const int c = sGui->getWebDataServices()->getDbInfoCount(entity);
            return c < 0 ? "-" : QString::number(c);
        }

        void CDbLoadOverviewComponent::admitCaches()
        {
            sGui->getWebDataServices()->admitDbCaches(CEntityFlags::AllDbEntities);
        }

        void CDbLoadOverviewComponent::ps_reloadPressed()
        {
            if (this->m_reloading) { return; }
            QObject *sender = QObject::sender();
            CEntityFlags::Entity entity = CEntityFlags::singleEntityByName(sender->objectName());

            // DB entities
            CEntityFlags::Entity triggeredEntity = sGui->getWebDataServices()->triggerReloadFromDb(entity);
            if (triggeredEntity == CEntityFlags::NoEntity) { return; }
            this->m_reloading = true;
            this->showLoading();

            // shared files ts
            sGui->getWebDataServices()->triggerLoadingOfSharedFilesHeaders(entity);
        }

        void CDbLoadOverviewComponent::ps_dataLoaded(CEntityFlags::Entity entity, CEntityFlags::ReadState state, int number)
        {
            Q_UNUSED(number);
            if (!CEntityFlags::anySwiftDbEntity(entity)) { return; }
            if (state == CEntityFlags::ReadFinished || state == CEntityFlags::ReadFinishedRestricted)
            {
                this->m_reloading = false;
                this->ps_setValues();
            }
        }
    } // ns
} // ns
