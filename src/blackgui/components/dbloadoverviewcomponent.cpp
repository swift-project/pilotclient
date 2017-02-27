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
#include "blackmisc/network/networkutils.h"

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
            this->admitCaches(); // in background

            ui->lbl_DatabaseUrl->setTextFormat(Qt::RichText);
            ui->lbl_DatabaseUrl->setTextInteractionFlags(Qt::TextBrowserInteraction);
            ui->lbl_DatabaseUrl->setOpenExternalLinks(true);
            ui->lbl_SharedUrls->setTextFormat(Qt::RichText);
            ui->lbl_SharedUrls->setTextInteractionFlags(Qt::TextBrowserInteraction);
            ui->lbl_SharedUrls->setOpenExternalLinks(true);

            connect(ui->tb_DbReloadAircraft, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_refreshDbPressed);
            connect(ui->tb_DbReloadAirlines, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_refreshDbPressed);
            connect(ui->tb_DbReloadAirports, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_refreshDbPressed);
            connect(ui->tb_DbReloadCountries, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_refreshDbPressed);
            connect(ui->tb_DbReloadLiveries, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_refreshDbPressed);
            connect(ui->tb_DbReloadModels, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_refreshDbPressed);
            connect(ui->tb_DbReloadDistributors, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_refreshDbPressed);

            connect(ui->tb_SharedReloadAircraft, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_refreshSharedPressed);
            connect(ui->tb_SharedReloadAirlines, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_refreshSharedPressed);
            connect(ui->tb_SharedReloadAirports, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_refreshSharedPressed);
            connect(ui->tb_SharedReloadCountries, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_refreshSharedPressed);
            connect(ui->tb_SharedReloadLiveries, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_refreshSharedPressed);
            connect(ui->tb_SharedReloadModels, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_refreshSharedPressed);
            connect(ui->tb_SharedReloadDistributors, &QToolButton::pressed, this, &CDbLoadOverviewComponent::ps_refreshSharedPressed);

            connect(ui->pb_LoadAllFromDB, &QPushButton::pressed, this, &CDbLoadOverviewComponent::loadAllFromDb);
            connect(ui->pb_LoadAllFromShared, &QPushButton::pressed, this, &CDbLoadOverviewComponent::loadAllFromShared);

            connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbLoadOverviewComponent::ps_dataLoaded);
            connect(this, &CDbLoadOverviewComponent::ps_triggerDigestGuiUpdate, this, &CDbLoadOverviewComponent::ps_setValues);
        }

        CDbLoadOverviewComponent::~CDbLoadOverviewComponent()
        { }

        void CDbLoadOverviewComponent::display()
        {
            this->ps_setValues();
        }

        void CDbLoadOverviewComponent::showVisibleDbRefreshButtons(bool visible)
        {
            ui->lbl_DbRefresh->setVisible(visible);
            ui->tb_DbReloadAircraft->setVisible(visible);
            ui->tb_DbReloadAirlines->setVisible(visible);
            ui->tb_DbReloadAirports->setVisible(visible);
            ui->tb_DbReloadCountries->setVisible(visible);
            ui->tb_DbReloadLiveries->setVisible(visible);
            ui->tb_DbReloadModels->setVisible(visible);
            ui->tb_DbReloadDistributors->setVisible(visible);
        }

        void CDbLoadOverviewComponent::showVisibleSharedRefreshButtons(bool visible)
        {
            ui->lbl_SharedRefresh->setVisible(visible);
            ui->tb_SharedReloadAircraft->setVisible(visible);
            ui->tb_SharedReloadAirlines->setVisible(visible);
            ui->tb_SharedReloadAirports->setVisible(visible);
            ui->tb_SharedReloadCountries->setVisible(visible);
            ui->tb_SharedReloadLiveries->setVisible(visible);
            ui->tb_SharedReloadModels->setVisible(visible);
            ui->tb_SharedReloadDistributors->setVisible(visible);
        }

        void CDbLoadOverviewComponent::resizeEvent(QResizeEvent *event)
        {
            if (this->isShowingLoadIndicator())
            {
                // re-center
                this->centerLoadIndicator();
            }
            QFrame::resizeEvent(event);
        }

        bool CDbLoadOverviewComponent::isShowingLoadIndicator() const
        {
            return m_loadIndicator && this->isVisible() && m_loadIndicator->isAnimated();
        }

        bool CDbLoadOverviewComponent::isLoadInProgress() const
        {
            return m_loadInProgress;
        }

        void CDbLoadOverviewComponent::showVisibleLoadAllButtons(bool shared, bool db)
        {
            const bool widget = shared || db;
            ui->wi_LoadAllButtons->setVisible(widget);
            ui->pb_LoadAllFromDB->setVisible(db);
            ui->pb_LoadAllFromShared->setVisible(shared);
        }

        void CDbLoadOverviewComponent::loadAllFromDb()
        {
            this->triggerLoadingFromDb(CEntityFlags::AllDbEntitiesNoInfoObjects);
        }

        void CDbLoadOverviewComponent::loadAllFromShared()
        {
            this->triggerLoadingFromSharedFiles(CEntityFlags::AllDbEntitiesNoInfoObjects);
        }

        void CDbLoadOverviewComponent::centerLoadIndicator()
        {
            if (!m_loadIndicator) { return; }
            const QPoint middle = this->visibleRegion().boundingRect().center();
            this->m_loadIndicator->centerLoadIndicator(middle);
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
            static const QString imgOk(":/diagona/icons/diagona/icons/tick.png");
            static const QString imgFailed(":/diagona/icons/diagona/icons/cross-script.png");
            const QString dbUrlHtml("<img src=\"%1\">&nbsp;&nbsp;<a href=\"%2\">%3</a>");
            QString url = sGui->getGlobalSetup().getDbHomePageUrl().getFullUrl();
            bool canConnect = CNetworkUtils::canConnect(sGui->getGlobalSetup().getDbHomePageUrl());
            ui->lbl_DatabaseUrl->setText(dbUrlHtml.arg(canConnect ? imgOk : imgFailed, url, url));
            ui->lbl_DatabaseUrl->setToolTip(url);

            // Shared URLs
            const CUrlList sharedUrls(sGui->getGlobalSetup().getSwiftSharedUrls());
            const QString tableHtml("<table>%1</table>");
            const QString rowHtml("<tr><td><img src=\"%1\">&nbsp;</td><td>%2</td></tr>");
            const QString urlLinkHtml("<a href=\"%1\">%2</a>");

            QString allRowsHtml;
            for (const CUrl &sharedUrl : sharedUrls)
            {
                canConnect = CNetworkUtils::canConnect(sharedUrl);
                allRowsHtml += rowHtml.arg(canConnect ? imgOk : imgFailed, urlLinkHtml.arg(sharedUrl.getFullUrl(), sharedUrl.getHost()));
            }
            ui->lbl_SharedUrls->setText(tableHtml.arg(allRowsHtml.trimmed()));
            ui->lbl_SharedUrls->setToolTip(sGui->getWebDataServices()->getDbReaderCurrentSharedDbDataUrl().toQString());
            ui->lbl_SharedUrls->setMinimumHeight(10 + (18 * sharedUrls.size()));

            // Indicator
            if (this->m_loadIndicator) { this->m_loadIndicator->stopAnimation(); }
        }

        void CDbLoadOverviewComponent::showLoading()
        {
            if (!this->m_loadIndicator)
            {
                this->m_loadIndicator = new CLoadIndicator(64, 64, this);
            }

            this->centerLoadIndicator();
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

        void CDbLoadOverviewComponent::ps_refreshDbPressed()
        {
            if (this->m_loadInProgress) { return; }
            const QObject *sender = QObject::sender();
            const CEntityFlags::Entity entity = CEntityFlags::singleEntityByName(sender->objectName());
            this->triggerLoadingFromDb(entity);
        }

        void CDbLoadOverviewComponent::triggerLoadingFromDb(CEntityFlags::Entity entities)
        {
            if (this->m_loadInProgress) { return; }

            // DB entities
            const CEntityFlags::Entity triggeredEntities = sGui->getWebDataServices()->triggerLoadingDirectlyFromDb(entities);
            if (triggeredEntities == CEntityFlags::NoEntity) { return; }
            this->m_loadInProgress = true;
            this->showLoading();

            // shared files ts
            sGui->getWebDataServices()->triggerLoadingOfSharedFilesHeaders(entities);
            sGui->getWebDataServices()->triggerReadOfInfoObjects();
        }

        void CDbLoadOverviewComponent::ps_refreshSharedPressed()
        {
            if (this->m_loadInProgress) { return; }
            const QObject *sender = QObject::sender();
            const CEntityFlags::Entity entity = CEntityFlags::singleEntityByName(sender->objectName());
            this->triggerLoadingFromSharedFiles(entity);
        }

        void CDbLoadOverviewComponent::triggerLoadingFromSharedFiles(CEntityFlags::Entity entities)
        {
            if (this->m_loadInProgress) { return; }

            // DB entities
            const CEntityFlags::Entity triggeredEntities = sGui->getWebDataServices()->triggerLoadingDirectlyFromSharedFiles(entities, true);
            if (triggeredEntities == CEntityFlags::NoEntity) { return; }
            this->m_loadInProgress = true;
            this->showLoading();

            // shared files ts
            sGui->getWebDataServices()->triggerReadOfInfoObjects();
        }

        void CDbLoadOverviewComponent::ps_dataLoaded(CEntityFlags::Entity entities, CEntityFlags::ReadState state, int number)
        {
            Q_UNUSED(number);
            if (!entities.testFlag(CEntityFlags::InfoObjectEntity) && !CEntityFlags::anySwiftDbEntity(entities)) { return; }
            if (state == CEntityFlags::ReadFinished || state == CEntityFlags::ReadFinishedRestricted)
            {
                this->m_loadInProgress = false;
                emit this->ps_triggerDigestGuiUpdate();
            }
        }
    } // ns
} // ns
