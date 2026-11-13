// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "miniguiwindow.h"

#include <QAction>
#include <QCloseEvent>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPointer>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "miniMiscSettingsDialog.h"
#include "miniflightplandialog.h"
#include "miniloginsettingsdialog.h"
#include "miniloginwidget.h"
#include "minisimulatorsettingsdialog.h"
#include "ministatusbarwidget.h"

#include "core/context/contextnetwork.h"
#include "gui/components/miniclientswidget.h"
#include "gui/components/minicockpitwidget.h"
#include "gui/components/miniradarwidget.h"
#include "gui/guiapplication.h"
#include "gui/stylesheetutility.h"
#include "misc/icons.h"

using namespace swift::core::context;
using namespace swift::gui;
using namespace swift::gui::components;
using namespace swift::misc;
using namespace swift::misc::network;

CMiniGuiWindow::CMiniGuiWindow(QWidget *parent) : QMainWindow(parent)
{
    this->setWindowTitle("swift Mini GUI");
    this->resize(1000, 700);

    this->initTabs();
    this->initMenus();
    this->initStyleSheet();

    if (sGui && sGui->getIContextNetwork())
    {
        connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this,
                &CMiniGuiWindow::onNetworkStatusChanged, Qt::QueuedConnection);
    }

    if (sGui)
    {
        // Live reload: .qss files are watched by CStyleSheetUtility (QFileSystemWatcher)
        // and automatically reloaded on change -> reapply the stylesheet here.
        connect(&sGui->getStyleSheetUtility(), &CStyleSheetUtility::styleSheetsChanged, this,
                &CMiniGuiWindow::initStyleSheet, Qt::QueuedConnection);
    }
}

CMiniGuiWindow::~CMiniGuiWindow() {}

void CMiniGuiWindow::initStyleSheet()
{
    if (!sGui || sGui->isShuttingDown()) { return; }
    const QString s = sGui->getStyleSheetUtility().styles({ CStyleSheetUtility::fileNameFonts(),
                                                            CStyleSheetUtility::fileNameStandardWidget(),
                                                            CStyleSheetUtility::fileNameSwiftMiniGui() });
    this->setStyleSheet(""); // see swiftguistdinit.cpp: avoids flicker/crash workaround on 2nd set
    this->setStyleSheet(s);
}

void CMiniGuiWindow::initTabs()
{
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_statusBarWidget = new CMiniStatusBarWidget(central);
    mainLayout->addWidget(m_statusBarWidget);

    m_tabs = new QTabWidget(central);
    mainLayout->addWidget(m_tabs, 1);

    central->setLayout(mainLayout);
    this->setCentralWidget(central);

    m_loginComponent = new CMiniLoginWidget(m_tabs);
    m_loginTabIndex = m_tabs->addTab(m_loginComponent, CIcons::globe16(), tr("Connect"));

    m_clientsWidget = new CMiniClientsWidget(m_tabs);
    m_clientsTabIndex = m_tabs->addTab(m_clientsWidget, CIcons::appAtc16(), tr("Clients"));

    m_radarWidget = new CMiniRadarWidget(m_tabs);
    m_tabs->addTab(m_radarWidget, CIcons::appRadar16(), tr("Radar"));

    m_cockpitWidget = new CMiniCockpitWidget(m_tabs);
    m_cockpitTabIndex = m_tabs->addTab(m_cockpitWidget, CIcons::appCockpit16(), tr("Cockpit"));

    // Empty tab without content: selecting it collapses the window down to the tab bar,
    // effectively acting as a "minimize GUI" option without actually minimizing the window.
    QWidget *emptyTab = new QWidget(m_tabs);
    m_minimizeTabIndex = m_tabs->addTab(emptyTab, tr("Minimize"));

    connect(m_tabs, &QTabWidget::currentChanged, this, &CMiniGuiWindow::onTabChanged);

    // Create eagerly so the flightPlanDataChanged signal can be wired to the status bar
    // right away, instead of only when the dialog is first opened via showFlightPlan().
    m_flightPlanDialog = new CMiniFlightPlanDialog(this);
    if (m_statusBarWidget)
    {
        connect(m_flightPlanDialog, &CMiniFlightPlanDialog::flightPlanDataChanged, m_statusBarWidget,
                &CMiniStatusBarWidget::setFlightPlanLoaded);
    }
    if (m_loginComponent)
    {
        // Gate the "Connect" button on flight plan availability (TMiniRequireFlightPlanForConnect)
        connect(m_flightPlanDialog, &CMiniFlightPlanDialog::flightPlanDataChanged, m_loginComponent,
                &CMiniLoginWidget::setFlightPlanAvailable);
        // Compare callsign/aircraft type between flight plan and "Own Aircraft" (mismatch warning)
        connect(m_flightPlanDialog, &CMiniFlightPlanDialog::flightPlanContentChanged, m_loginComponent,
                &CMiniLoginWidget::setFlightPlanContent);
        // A flight plan was loaded via the quick-access SimBrief button in "Own Aircraft" -> forward
        // it to the "Flight Plan Management" dialog, so it remains the single source of truth.
        connect(m_loginComponent, &CMiniLoginWidget::simBriefFlightPlanLoaded, m_flightPlanDialog,
                &CMiniFlightPlanDialog::fillWithFlightPlanData);
    }

    // The dialog's constructor may already have restored and emitted a locally persisted flight plan
    // (see CMiniFlightPlanDialog), which happened before the connections above were wired up. Propagate
    // that initial status now, so a flight plan loaded from the last Mini-GUI session is reflected too.
    if (m_flightPlanDialog->hasFlightPlanLoaded())
    {
        if (m_statusBarWidget) { m_statusBarWidget->setFlightPlanLoaded(true); }
        if (m_loginComponent)
        {
            m_loginComponent->setFlightPlanAvailable(true);
            m_loginComponent->setFlightPlanContent(m_flightPlanDialog->getCurrentFlightPlanData());
        }
    }

    // Create the dialog already here (not lazily in showLoginSettings()), so that
    // CMiniLoginWidget::moveNetworkAndPilotGroupsTo() is executed BEFORE the
    // Connect tab becomes visible for the first time. This way the Connect tab only
    // ever shows "Own Aircraft" (and, while connected, the server info) from the start.
    m_loginSettingsDialog = new CMiniLoginSettingsDialog(m_loginComponent, this);

    // In swiftguistandard, CLoginComponent::mainInfoAreaChanged() is triggered via
    // CMainInfoAreaComponent's "currentMainInfoAreaChanged" signal whenever the Login
    // page becomes visible; this is what populates "Own Aircraft" (comp_OwnAircraft)
    // with the current model/ICAO data. The Mini-GUI has no such info-area concept,
    // so we trigger the same update manually: once right after the tab is created,
    // and again every time the "Connect" tab is selected (e.g. after the simulator
    // model has changed in the meantime). mainInfoAreaChanged() alone is not sufficient
    // though: it internally calls setOwnModelAndIcaoValues() with an EMPTY model unless
    // a simulator is actively "simulating", which would clear the ICAO/callsign/model
    // fields while still updating the frame color -> additionally call
    // CMiniLoginWidget::refreshOwnAircraftDisplay() to force the real current model in.
    connect(m_tabs, &QTabWidget::currentChanged, this, [this](int index) {
        if (index == m_loginTabIndex && m_loginComponent)
        {
            m_loginComponent->mainInfoAreaChanged(m_loginComponent);
            m_loginComponent->refreshOwnAircraftDisplay();
        }
    });

    if (m_loginComponent)
    {
        QPointer<CMiniLoginWidget> loginWidget(m_loginComponent);
        QTimer::singleShot(0, this, [loginWidget] {
            if (!loginWidget) { return; }
            loginWidget->mainInfoAreaChanged(loginWidget);
            loginWidget->refreshOwnAircraftDisplay();
        });
    }
}

void CMiniGuiWindow::initMenus()
{
    QMenu *menuSettings = menuBar()->addMenu(tr("Settings"));

    QAction *actionLoginSettings = menuSettings->addAction(tr("Login Configuration"));
    connect(actionLoginSettings, &QAction::triggered, this, &CMiniGuiWindow::showLoginSettings);

    QAction *actionFlightPlan = menuSettings->addAction(tr("Flight Plan Management"));
    connect(actionFlightPlan, &QAction::triggered, this, &CMiniGuiWindow::showFlightPlan);

    QAction *actionSimSettings = menuSettings->addAction(tr("Simulator Configuration"));
    connect(actionSimSettings, &QAction::triggered, this, &CMiniGuiWindow::showSimulatorSettings);

    QAction *actionMiscSettings = menuSettings->addAction(tr("Miscellaneous"));
    connect(actionMiscSettings, &QAction::triggered, this, &CMiniGuiWindow::showMiscSettings);

    menuSettings->addSeparator();

    QAction *actionQuit = menuSettings->addAction(tr("Exit"));
    actionQuit->setShortcut(QKeySequence::Quit);
    connect(actionQuit, &QAction::triggered, this, &CMiniGuiWindow::close);
}

void CMiniGuiWindow::showLoginSettings()
{
    if (!m_loginSettingsDialog) { m_loginSettingsDialog = new CMiniLoginSettingsDialog(m_loginComponent, this); }
    m_loginSettingsDialog->show();
    m_loginSettingsDialog->raise();
    m_loginSettingsDialog->activateWindow();
}

void CMiniGuiWindow::showFlightPlan()
{
    if (!m_flightPlanDialog) { m_flightPlanDialog = new CMiniFlightPlanDialog(this); }
    m_flightPlanDialog->show();
    m_flightPlanDialog->raise();
    m_flightPlanDialog->activateWindow();
}

void CMiniGuiWindow::showSimulatorSettings()
{
    if (!m_simSettingsDialog) { m_simSettingsDialog = new CMiniSimulatorSettingsDialog(this); }
    m_simSettingsDialog->show();
    m_simSettingsDialog->raise();
    m_simSettingsDialog->activateWindow();
}

void CMiniGuiWindow::showMiscSettings()
{
    if (!m_miscSettingsDialog) { m_miscSettingsDialog = new CMiniMiscSettingsDialog(this); }
    m_miscSettingsDialog->show();
    m_miscSettingsDialog->raise();
    m_miscSettingsDialog->activateWindow();
}

void CMiniGuiWindow::onNetworkStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
{
    Q_UNUSED(from)
    if (to.isConnected())
    {
        if (m_tabs && m_cockpitTabIndex >= 0) { m_tabs->setCurrentIndex(m_cockpitTabIndex); }

        // TMiniRequireFlightPlanForConnect: automatically send the flight plan once connected
        if (m_loginComponent && m_loginComponent->shouldAutoSendFlightPlan() && m_flightPlanDialog)
        {
            m_flightPlanDialog->sendFlightPlan();
        }
    }
}

void CMiniGuiWindow::onTabChanged(int index)
{
    if (!m_tabs || m_minimizeTabIndex < 0) { return; }

    if (index == m_minimizeTabIndex)
    {
        // remember the current height so it can be restored, then collapse down to the tab bar
        m_expandedHeight = this->height();

        // QTabWidget/QStackedWidget sizes itself (and thus enforces a minimum window size)
        // based on the largest of ALL tab pages, not just the currently visible (empty) one,
        // to avoid flicker when switching pages. Override this so the window can actually
        // shrink below that enforced minimum.
        this->setMinimumHeight(0);
        m_tabs->setMinimumHeight(0);

        const int collapsedHeight = m_tabs->tabBar()->height() + m_statusBarWidget->height() + menuBar()->height();
        this->resize(this->width(), collapsedHeight);

        // make the whole (collapsed) window semi-transparent while in "Minimize" mode,
        // degree of transparency configurable via TMiniMinimizeOpacity (Miscellaneous settings)
        this->setWindowOpacity(qBound(10, m_minimizeOpacitySetting.get(), 100) / 100.0);
    }
    else
    {
        // restore full opacity as soon as any other tab is selected
        this->setWindowOpacity(1.0);

        if (m_expandedHeight > 0)
        {
            this->resize(this->width(), m_expandedHeight);
            m_expandedHeight = -1;
        }
    }
}

void CMiniGuiWindow::closeEvent(QCloseEvent *event)
{
    const bool connected = sGui && sGui->getIContextNetwork() && sGui->getIContextNetwork()->isConnected();
    if (connected)
    {
        if (m_loginComponent) { m_loginComponent->blinkDisconnectButton(); }
        event->ignore();
        return;
    }
    QMainWindow::closeEvent(event);
}