// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "dbloadoverviewcomponent.h"
#include "ui_dbloadoverviewcomponent.h"

#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackcore/webdataservices.h"
#include "blackcore/db/infodatareader.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/htmlutils.h"

#include <QPointer>
#include <QMessageBox>

using namespace BlackGui;
using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackGui::Components
{
    CDbLoadOverviewComponent::CDbLoadOverviewComponent(QWidget *parent) : QFrame(parent), CLoadIndicatorEnabled(this),
                                                                          ui(new Ui::CDbLoadOverviewComponent)
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "missing sGui");
        ui->setupUi(this);

        //! \fixme KB 201709 It is hard to judge if it is a good idea to trigger cache admit in a UI component
        //  althought admit happens in background, this component might trigger cache reads not needed (though it is not very likely)
        this->admitCaches();

        ui->lbl_DatabaseUrl->setTextFormat(Qt::RichText);
        ui->lbl_DatabaseUrl->setTextInteractionFlags(Qt::TextBrowserInteraction);
        ui->lbl_DatabaseUrl->setOpenExternalLinks(true);
        ui->lbl_SharedUrls->setTextFormat(Qt::RichText);
        ui->lbl_SharedUrls->setTextInteractionFlags(Qt::TextBrowserInteraction);
        ui->lbl_SharedUrls->setOpenExternalLinks(true);

        connect(ui->tb_DbReloadAircraft, &QToolButton::pressed, this, &CDbLoadOverviewComponent::refreshDbPressed);
        connect(ui->tb_DbReloadAirlines, &QToolButton::pressed, this, &CDbLoadOverviewComponent::refreshDbPressed);
        connect(ui->tb_DbReloadAirports, &QToolButton::pressed, this, &CDbLoadOverviewComponent::refreshDbPressed);
        connect(ui->tb_DbReloadCountries, &QToolButton::pressed, this, &CDbLoadOverviewComponent::refreshDbPressed);
        connect(ui->tb_DbReloadLiveries, &QToolButton::pressed, this, &CDbLoadOverviewComponent::refreshDbPressed);
        connect(ui->tb_DbReloadModels, &QToolButton::pressed, this, &CDbLoadOverviewComponent::refreshDbPressed);
        connect(ui->tb_DbReloadDistributors, &QToolButton::pressed, this, &CDbLoadOverviewComponent::refreshDbPressed);
        connect(ui->tb_DbReloadCategories, &QToolButton::pressed, this, &CDbLoadOverviewComponent::refreshDbPressed);

        connect(ui->tb_SharedReloadAircraft, &QToolButton::pressed, this, &CDbLoadOverviewComponent::refreshSharedPressed);
        connect(ui->tb_SharedReloadAirlines, &QToolButton::pressed, this, &CDbLoadOverviewComponent::refreshSharedPressed);
        connect(ui->tb_SharedReloadAirports, &QToolButton::pressed, this, &CDbLoadOverviewComponent::refreshSharedPressed);
        connect(ui->tb_SharedReloadCountries, &QToolButton::pressed, this, &CDbLoadOverviewComponent::refreshSharedPressed);
        connect(ui->tb_SharedReloadLiveries, &QToolButton::pressed, this, &CDbLoadOverviewComponent::refreshSharedPressed);
        connect(ui->tb_SharedReloadModels, &QToolButton::pressed, this, &CDbLoadOverviewComponent::refreshSharedPressed);
        connect(ui->tb_SharedReloadDistributors, &QToolButton::pressed, this, &CDbLoadOverviewComponent::refreshSharedPressed);
        connect(ui->tb_SharedReloadCategories, &QToolButton::pressed, this, &CDbLoadOverviewComponent::refreshSharedPressed);

        connect(ui->pb_LoadAllFromDB, &QPushButton::pressed, this, &CDbLoadOverviewComponent::loadAllFromDb);
        connect(ui->pb_LoadAllFromShared, &QPushButton::pressed, this, &CDbLoadOverviewComponent::loadAllFromShared);
        connect(ui->pb_LoadAllFromResources, &QPushButton::pressed, this, &CDbLoadOverviewComponent::loadAllFromResourceFiles);

        if (sGui->hasWebDataServices())
        {
            connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbLoadOverviewComponent::dataLoaded);
            QPointer<CDbLoadOverviewComponent> myself(this);
            QTimer::singleShot(10 * 1000, this, [=] {
                if (!myself || !sGui || sGui->isShuttingDown()) { return; }
                this->loadInfoObjects();
                this->adjustTextWidth();
            });
        }
    }

    CDbLoadOverviewComponent::~CDbLoadOverviewComponent()
    {}

    void CDbLoadOverviewComponent::display()
    {
        m_dsTriggerGuiUpdate.inputSignal();
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
        ui->tb_DbReloadCategories->setVisible(visible);
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
        ui->tb_SharedReloadCategories->setVisible(visible);
    }

    void CDbLoadOverviewComponent::resizeEvent(QResizeEvent *event)
    {
        if (this->isShowingLoadIndicator())
        {
            // re-center
            this->centerLoadIndicator();
            this->adjustTextWidth();
        }
        QFrame::resizeEvent(event);
    }

    void CDbLoadOverviewComponent::showVisibleLoadAllButtons(bool shared, bool db, bool disk)
    {
        const bool widget = shared || db;
        ui->wi_LoadAllButtons->setVisible(widget);
        ui->pb_LoadAllFromDB->setVisible(db);
        ui->pb_LoadAllFromShared->setVisible(shared);
        ui->pb_LoadAllFromResources->setVisible(disk);
    }

    void CDbLoadOverviewComponent::loadAllFromDb()
    {
        this->triggerLoadingFromDb(CEntityFlags::AllDbEntitiesNoInfoObjects);
    }

    void CDbLoadOverviewComponent::loadAllFromShared()
    {
        this->triggerLoadingFromSharedFiles(CEntityFlags::AllDbEntitiesNoInfoObjects);
    }

    void CDbLoadOverviewComponent::loadAllFromResourceFiles()
    {
        if (m_loadInProgress) { return; }
        if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
        const QMessageBox::StandardButton reply = QMessageBox::warning(this, "Load DB data", "You should only load DB from disk resources if loading from network does not work. Really load?", QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) { return; }
        sGui->getWebDataServices()->initDbCachesFromLocalResourceFiles(true);
    }

    void CDbLoadOverviewComponent::setGuiValues()
    {
        if (!sGui) { return; }
        if (!sGui->hasWebDataServices()) { return; }

        ui->le_AircraftIcaoCacheCount->setText(cacheCountForEntity(CEntityFlags::AircraftIcaoEntity));
        ui->le_AirlinesIcaoCacheCount->setText(cacheCountForEntity(CEntityFlags::AirlineIcaoEntity));
        ui->le_AirportsCacheCount->setText(cacheCountForEntity(CEntityFlags::AirportEntity));
        ui->le_LiveriesCacheCount->setText(cacheCountForEntity(CEntityFlags::LiveryEntity));
        ui->le_ModelsCacheCount->setText(cacheCountForEntity(CEntityFlags::ModelEntity));
        ui->le_CountriesCacheCount->setText(cacheCountForEntity(CEntityFlags::CountryEntity));
        ui->le_DistributorsCacheCount->setText(cacheCountForEntity(CEntityFlags::DistributorEntity));
        ui->le_CategoriesCacheCount->setText(cacheCountForEntity(CEntityFlags::AircraftCategoryEntity));

        ui->le_AircraftIcaoDbCount->setText(dbCountForEntity(CEntityFlags::AircraftIcaoEntity));
        ui->le_AirlinesIcaoDbCount->setText(dbCountForEntity(CEntityFlags::AirlineIcaoEntity));
        ui->le_AirportsDbCount->setText(dbCountForEntity(CEntityFlags::AirportEntity));
        ui->le_LiveriesDbCount->setText(dbCountForEntity(CEntityFlags::LiveryEntity));
        ui->le_ModelsDbCount->setText(dbCountForEntity(CEntityFlags::ModelEntity));
        ui->le_CountriesDbCount->setText(dbCountForEntity(CEntityFlags::CountryEntity));
        ui->le_DistributorsDbCount->setText(dbCountForEntity(CEntityFlags::DistributorEntity));
        ui->le_CategoriesDbCount->setText(dbCountForEntity(CEntityFlags::AircraftCategoryEntity));

        ui->le_AircraftIcaoSharedCount->setText(sharedCountForEntity(CEntityFlags::AircraftIcaoEntity));
        ui->le_AirlinesIcaoSharedCount->setText(sharedCountForEntity(CEntityFlags::AirlineIcaoEntity));
        ui->le_AirportsSharedCount->setText(sharedCountForEntity(CEntityFlags::AirportEntity));
        ui->le_LiveriesSharedCount->setText(sharedCountForEntity(CEntityFlags::LiveryEntity));
        ui->le_ModelsSharedCount->setText(sharedCountForEntity(CEntityFlags::ModelEntity));
        ui->le_CountriesSharedCount->setText(sharedCountForEntity(CEntityFlags::CountryEntity));
        ui->le_DistributorsSharedCount->setText(sharedCountForEntity(CEntityFlags::DistributorEntity));
        ui->le_CategoriesSharedCount->setText(sharedCountForEntity(CEntityFlags::AircraftCategoryEntity));

        ui->le_AircraftIcaoCacheTs->setText(cacheTimestampForEntity(CEntityFlags::AircraftIcaoEntity));
        ui->le_AirlinesIcaoCacheTs->setText(cacheTimestampForEntity(CEntityFlags::AirlineIcaoEntity));
        ui->le_AirportsCacheTs->setText(cacheTimestampForEntity(CEntityFlags::AirportEntity));
        ui->le_LiveriesCacheTs->setText(cacheTimestampForEntity(CEntityFlags::LiveryEntity));
        ui->le_ModelsCacheTs->setText(cacheTimestampForEntity(CEntityFlags::ModelEntity));
        ui->le_CountriesCacheTs->setText(cacheTimestampForEntity(CEntityFlags::CountryEntity));
        ui->le_DistributorsCacheTs->setText(cacheTimestampForEntity(CEntityFlags::DistributorEntity));
        ui->le_CategoriesCacheTs->setText(cacheTimestampForEntity(CEntityFlags::AircraftCategoryEntity));

        ui->le_AircraftIcaoDbTs->setText(dbTimestampForEntity(CEntityFlags::AircraftIcaoEntity));
        ui->le_AirlinesIcaoDbTs->setText(dbTimestampForEntity(CEntityFlags::AirlineIcaoEntity));
        ui->le_AirportsDbTs->setText(dbTimestampForEntity(CEntityFlags::AirportEntity));
        ui->le_LiveriesDbTs->setText(dbTimestampForEntity(CEntityFlags::LiveryEntity));
        ui->le_ModelsDbTs->setText(dbTimestampForEntity(CEntityFlags::ModelEntity));
        ui->le_CountriesDbTs->setText(dbTimestampForEntity(CEntityFlags::CountryEntity));
        ui->le_DistributorsDbTs->setText(dbTimestampForEntity(CEntityFlags::DistributorEntity));
        ui->le_CategoriesDbTs->setText(dbTimestampForEntity(CEntityFlags::AircraftCategoryEntity));

        ui->le_AircraftIcaoSharedFileTs->setText(sharedFileTimestampForEntity(CEntityFlags::AircraftIcaoEntity));
        ui->le_AirlinesIcaoSharedFileTs->setText(sharedFileTimestampForEntity(CEntityFlags::AirlineIcaoEntity));
        ui->le_AirportsSharedFileTs->setText(sharedFileTimestampForEntity(CEntityFlags::AirportEntity));
        ui->le_LiveriesSharedFileTs->setText(sharedFileTimestampForEntity(CEntityFlags::LiveryEntity));
        ui->le_ModelsSharedFileTs->setText(sharedFileTimestampForEntity(CEntityFlags::ModelEntity));
        ui->le_CountriesSharedFileTs->setText(sharedFileTimestampForEntity(CEntityFlags::CountryEntity));
        ui->le_DistributorsSharedFileTs->setText(sharedFileTimestampForEntity(CEntityFlags::DistributorEntity));
        ui->le_CategoriesSharedFileTs->setText(sharedFileTimestampForEntity(CEntityFlags::AircraftCategoryEntity));

        // DB URL
        const QString dbUrlHtml("<img src=\"%1\">&nbsp;&nbsp;<a href=\"%2\">%3</a>");
        const QString url = sGui->getGlobalSetup().getDbHomePageUrl().getFullUrl();
        bool canConnect = sGui->isSwiftDbAccessible();
        ui->lbl_DatabaseUrl->setText(dbUrlHtml.arg(canConnect ? m_imgOk : m_imgFailed, url, url));
        ui->lbl_DatabaseUrl->setToolTip(url);

        // Shared URLs
        this->setSharedUrlValues();

        // Indicator
        this->hideLoading();
    }

    void CDbLoadOverviewComponent::setSharedUrlValues()
    {
        if (m_sharedValueCheckInProgress) { return; }
        if ((QDateTime::currentMSecsSinceEpoch() - m_sharedLastCheck) < 5000) { return; } // do not check too often

        m_sharedValueCheckInProgress = true; // avoid processEvent (canConnect) calling this again before done

        const CUrlList sharedUrls(sGui->getGlobalSetup().getSwiftSharedUrls());
        const QString valueHtml("<img src=\"%1\">&nbsp;%2");
        const QString urlLinkHtml("<a href=\"%1\">%2</a>");

        QStringList values;
        for (const CUrl &sharedUrl : sharedUrls)
        {
            if (!sGui || sGui->isShuttingDown())
            {
                // shutdown during connect test
                m_sharedValueCheckInProgress = false;
                return;
            }
            const bool canConnect = CNetworkUtils::canConnect(sharedUrl);
            values.push_back(
                valueHtml.arg(canConnect ? m_imgOk : m_imgFailed, urlLinkHtml.arg(sharedUrl.getFullUrl(), sharedUrl.getHost())));
        }

        const QString sharedUrlTable = toHtmTable(values, 2);
        ui->lbl_SharedUrls->setText(sharedUrlTable);
        ui->lbl_SharedUrls->setMinimumHeight(10 + (18 * sharedUrls.size()));
        const QString currentlyUsedSharedUrl = sGui->getWorkingSharedUrl().toQString();
        ui->lbl_SharedUrls->setToolTip(
            currentlyUsedSharedUrl.isEmpty() ? "No shared URL" : "currently used: " + currentlyUsedSharedUrl);

        m_sharedLastCheck = QDateTime::currentMSecsSinceEpoch();
        m_sharedValueCheckInProgress = false;
    }

    bool CDbLoadOverviewComponent::isInitialized() const
    {
        return !ui->le_AircraftIcaoCacheCount->text().isEmpty();
    }

    QString CDbLoadOverviewComponent::formattedTimestamp(const QDateTime &dateTime)
    {
        if (!dateTime.isValid()) { return "-"; }
        return dateTime.toUTC().toString("yy-MM-dd hh:mm:ss");
    }

    QString CDbLoadOverviewComponent::cacheTimestampForEntity(CEntityFlags::Entity entity)
    {
        const QDateTime ts = sGui->getWebDataServices()->getCacheTimestamp(entity);
        return formattedTimestamp(ts);
    }

    QString CDbLoadOverviewComponent::dbTimestampForEntity(CEntityFlags::Entity entity)
    {
        const QDateTime ts = sGui->getWebDataServices()->getLatestDbEntityTimestamp(entity);
        return formattedTimestamp(ts);
    }

    QString CDbLoadOverviewComponent::cacheCountForEntity(CEntityFlags::Entity entity)
    {
        const int c = sGui->getWebDataServices()->getCacheCount(entity);
        return c < 0 ? "-" : QString::number(c);
    }

    QString CDbLoadOverviewComponent::sharedFileTimestampForEntity(CEntityFlags::Entity entity)
    {
        const QDateTime ts = sGui->getWebDataServices()->getLatestSharedInfoObjectTimestamp(entity);
        return formattedTimestamp(ts);
    }

    QString CDbLoadOverviewComponent::sharedCountForEntity(CEntityFlags::Entity entity)
    {
        const int c = sGui->getWebDataServices()->getSharedInfoObjectCount(entity);
        return c < 0 ? "-" : QString::number(c);
    }

    QString CDbLoadOverviewComponent::dbCountForEntity(CEntityFlags::Entity entity)
    {
        const int c = sGui->getWebDataServices()->getDbInfoObjectCount(entity);
        return c < 0 ? "-" : QString::number(c);
    }

    void CDbLoadOverviewComponent::admitCaches()
    {
        // remark admit only triggers, it does not wait like synchronize
        if (!sGui->hasWebDataServices()) { return; }
        sGui->getWebDataServices()->admitDbCaches(CEntityFlags::AllDbEntities);
    }

    void CDbLoadOverviewComponent::refreshDbPressed()
    {
        if (m_loadInProgress) { return; }
        const QObject *sender = QObject::sender();
        const CEntityFlags::Entity entity = CEntityFlags::singleEntityByName(sender->objectName());
        this->triggerLoadingFromDb(entity);
    }

    void CDbLoadOverviewComponent::triggerLoadingFromDb(CEntityFlags::Entity entities)
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
        if (m_loadInProgress) { return; }

        // DB entities
        const CEntityFlags::Entity triggeredEntities = sGui->getWebDataServices()->triggerLoadingDirectlyFromDb(entities);
        if (triggeredEntities == CEntityFlags::NoEntity) { return; }
        m_loadInProgress = true;
        this->showLoading();

        // shared files ts and DB info objects
        sGui->getWebDataServices()->triggerReadOfSharedInfoObjects();
        sGui->getWebDataServices()->triggerReadOfDbInfoObjects();
    }

    void CDbLoadOverviewComponent::refreshSharedPressed()
    {
        if (m_loadInProgress) { return; }
        const QObject *sender = QObject::sender();
        const CEntityFlags::Entity entity = CEntityFlags::singleEntityByName(sender->objectName());
        this->triggerLoadingFromSharedFiles(entity);
    }

    void CDbLoadOverviewComponent::triggerLoadingFromSharedFiles(CEntityFlags::Entity entities)
    {
        if (m_loadInProgress) { return; }

        // DB entities
        const CEntityFlags::Entity triggeredEntities = sGui->getWebDataServices()->triggerLoadingDirectlyFromSharedFiles(entities, true);
        if (triggeredEntities == CEntityFlags::NoEntity) { return; }
        m_loadInProgress = true;
        this->showLoading();

        // shared files ts
        sGui->getWebDataServices()->triggerReadOfSharedInfoObjects();
    }

    void CDbLoadOverviewComponent::dataLoaded(CEntityFlags::Entity entities, CEntityFlags::ReadState state, int number, const QUrl &url)
    {
        Q_UNUSED(number)
        Q_UNUSED(url)

        if (!CEntityFlags::isFinishedReadState(state)) return;
        if (!entities.testFlag(CEntityFlags::SharedInfoObjectEntity) && !entities.testFlag(CEntityFlags::DbInfoObjectEntity) && !CEntityFlags::anySwiftDbEntity(entities)) { return; }
        m_loadInProgress = false;
        m_dsTriggerGuiUpdate.inputSignal();
    }

    void CDbLoadOverviewComponent::loadInfoObjects()
    {
        if (m_loadInProgress) { return; }
        if (!sGui || !sGui->hasWebDataServices()) { return; }
        if (sGui->isShuttingDown()) { return; }
        bool direct = false;

        if (sGui->getWebDataServices()->getDbInfoDataReader() && sGui->getWebDataServices()->getDbInfoDataReader()->getInfoObjectCount() > 0)
        {
            direct = true;
        }
        else if (sGui->isSwiftDbAccessible())
        {
            // do not trigger if cannot be connected
            sGui->getWebDataServices()->triggerReadOfDbInfoObjects();
        }

        if (sGui->getWebDataServices()->getSharedInfoDataReader() && sGui->getWebDataServices()->getSharedInfoDataReader()->getInfoObjectCount() > 0)
        {
            direct = true;
        }
        else
        {
            sGui->getWebDataServices()->triggerReadOfSharedInfoObjects();
        }

        if (direct)
        {
            m_dsTriggerGuiUpdate.inputSignal();
        }
    }

    void CDbLoadOverviewComponent::adjustTextWidth()
    {
        const QSize s = this->size();
        const QSizeF sizeFont = CGuiUtility::fontMetricsLazyDog43Chars();
        const int thresholdW = qRound(sizeFont.width() * 2.0);
        if (s.width() > thresholdW)
        {
            ui->lbl_DbCount->setText("DB count");
            ui->lbl_CacheCount->setText("Cache count");
            ui->lbl_SharedCount->setText("Shared count");
        }
        else
        {
            ui->lbl_DbCount->setText("DB#");
            ui->lbl_CacheCount->setText("C#");
            ui->lbl_SharedCount->setText("S#");
        }
    }
} // ns
