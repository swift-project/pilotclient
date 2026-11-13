// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "miniloginwidget.h"

#include <QBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QNetworkReply>
#include <QPointer>
#include <QPushButton>
#include <QScopedPointer>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "core/context/contextnetwork.h"
#include "core/context/contextownaircraft.h"
#include "core/context/contextsimulator.h"
#include "core/data/networksetup.h"
#include "core/webdataservices.h"
#include "gui/components/dbaircrafticaoselectorcomponent.h"
#include "gui/components/dbairlineicaoselectorcomponent.h"
#include "gui/components/networkdetailscomponent.h"
#include "gui/components/ownaircraftcomponent.h"
#include "gui/components/serverlistselector.h"
#include "gui/components/simbriefdownloaddialog.h"
#include "gui/guiapplication.h"
#include "gui/settings/minisettings.h"
#include "misc/aviation/flightplan.h"
#include "misc/network/server.h"
#include "misc/network/url.h"
#include "misc/settingscache.h"
#include "misc/simulation/aircraftmodel.h"

using namespace swift::core;
using namespace swift::core::context;
using namespace swift::core::data;
using namespace swift::gui::components;
using namespace swift::gui::settings;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::misc::simulation;

CMiniLoginWidget::CMiniLoginWidget(QWidget *parent) : CLoginComponent(parent)
{
    // The base class has already called updateUiConnectState() in its constructor
    // (button may be disabled if no simulator is connected) -> correct this immediately
    this->forceEnableLoginButton();

    // Make the "Connect"/"Disconnect" button wider and more prominent.
    // Calling this directly in the constructor has no visible effect for the initially shown
    // tab: the layout of pb_Ok has not been fully established yet at this point (it only takes
    // effect once the widget's layout is recalculated, e.g. triggered by a tab switch) -> defer
    // to the next event loop iteration, once the widget is actually shown.
    this->widenConnectButton();
    QPointer<CMiniLoginWidget> self(this);
    QTimer::singleShot(0, this, [self] {
        if (!self) { return; }
        self->widenConnectButton();
    });

    // Show "General"/"Advanced" of COwnAircraftComponent stacked directly on top of each
    // other instead of as tabs
    this->stackOwnAircraftSections();

    // Move "lbl_Model"/"le_SimulatorModel" (originally part of "Advanced") directly below
    // "Type"/"Airline" in the "General" section
    this->moveModelWidgetBelowTypeAndAirline();

    // Hide simulator model / livery send widgets not relevant for the Mini-GUI
    this->hideOwnAircraftModelLiveryWidgets();

    // Quick-access SimBrief button (plus Aircraft/Departure/Destination and Route info) below
    // comp_ModelStringCompleter
    this->insertSimBriefSection();

    // Red warning label below "Own Aircraft", shown when callsign/aircraft type differ from the flight plan
    this->insertAircraftMismatchWarning();
    {
        COwnAircraftComponent *ownAircraftComponent =
            this->findChild<COwnAircraftComponent *>(QStringLiteral("comp_OwnAircraft"));
        if (ownAircraftComponent)
        {
            connect(ownAircraftComponent, &COwnAircraftComponent::aircraftDataChanged, this,
                    &CMiniLoginWidget::onOwnAircraftDataChanged, Qt::QueuedConnection);
        }
    }

    // aircraftDataChanged() is only emitted from COwnAircraftComponent::onSimulatorModelChanged(),
    // which itself only runs on simulator events and returns early while connected -> plain GUI
    // edits (callsign, ICAO selectors) would otherwise never update the mismatch warning without a
    // prior/active connection. Connect directly to the relevant GUI elements instead.
    this->connectOwnAircraftChangeSignals();

    // Quick-access server selection box directly above "Own Aircraft"
    this->insertServerSelector();

    // VATSIM FSD servers are only available once the data file has been read (same as
    // CNetworkDetailsComponent::onWebServiceDataRead()) -> refresh the selector when ready,
    // and once immediately in case the data is already available.
    if (sGui && sGui->getWebDataServices())
    {
        connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CMiniLoginWidget::onWebServiceDataRead,
                Qt::QueuedConnection);
    }
    this->onWebServiceDataRead(CEntityFlags::VatsimDataFile, CEntityFlags::ReadFinished, -1, {});

    // The base class already connects its own (private) slots with Qt::QueuedConnection
    // to the same signals. Since our connection here is registered AFTER the base class,
    // our slot is executed after the base class's slot in the Qt event queue
    // (connections are processed in registration order) -> guaranteed correct order.
    if (sGui && sGui->getIContextSimulator())
    {
        connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorStatusChanged, this,
                &CMiniLoginWidget::forceEnableLoginButton, Qt::QueuedConnection);
    }
    if (sGui && sGui->getIContextNetwork())
    {
        connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this,
                &CMiniLoginWidget::forceEnableLoginButton, Qt::QueuedConnection);
    }

    // Model validity (green frame) has changed -> re-evaluate the connect button state
    connect(this, &CLoginComponent::modelValidityChanged, this, &CMiniLoginWidget::forceEnableLoginButton,
            Qt::QueuedConnection);
}

CMiniLoginWidget::~CMiniLoginWidget() {}

void CMiniLoginWidget::moveNetworkAndPilotGroupsTo(QLayout *targetLayout)
{
    if (m_groupsMoved || !targetLayout) { return; }

    QGroupBox *gbNetwork = this->findChild<QGroupBox *>(QStringLiteral("gb_Network"));
    QGroupBox *gbPilot = this->findChild<QGroupBox *>(QStringLiteral("gb_PilotsDetails"));

    // Capture the CNetworkDetailsComponent instance BEFORE it is reparented away, so it remains
    // reachable afterwards (findChild() from "this" would no longer find it once gb_Network has
    // been reparented into the settings dialog).
    if (!m_networkDetailsComponent)
    {
        m_networkDetailsComponent = this->findChild<CNetworkDetailsComponent *>(QStringLiteral("comp_NetworkDetails"));
    }

    if (gbNetwork)
    {
        gbNetwork->setParent(nullptr);
        targetLayout->addWidget(gbNetwork);
    }
    if (gbPilot)
    {
        gbPilot->setParent(nullptr);
        targetLayout->addWidget(gbPilot);
    }

    m_groupsMoved = true;
}

void CMiniLoginWidget::refreshOwnAircraftDisplay()
{
    if (!sGui || !sGui->getIContextOwnAircraft()) { return; }

    // CLoginComponent::mainInfoAreaChanged() calls COwnAircraftComponent::setOwnModelAndIcaoValues()
    // without an argument (empty default model). When no simulator is actively reporting a model
    // ("simulating" is false), this clears/leaves blank the ICAO/callsign/model fields while still
    // updating the frame color (highlightModelField()) -> explicitly pass the real current model.
    COwnAircraftComponent *ownAircraftComponent =
        this->findChild<COwnAircraftComponent *>(QStringLiteral("comp_OwnAircraft"));
    if (!ownAircraftComponent) { return; }

    const CAircraftModel model = sGui->getIContextOwnAircraft()->getOwnAircraft().getModel();
    ownAircraftComponent->setOwnModelAndIcaoValues(model);

    this->updateAircraftMismatchWarning();
}

void CMiniLoginWidget::stackOwnAircraftSections()
{
    // comp_OwnAircraft (COwnAircraftComponent) internally contains a QTabWidget "tabWidget"
    // with the pages "General" and "Advanced". For the Mini-GUI, both sections should be
    // visible directly on top of each other instead of being selected via tabs.
    QTabWidget *tabWidget = this->findChild<QTabWidget *>(QStringLiteral("tabWidget"));
    if (!tabWidget) { return; }

    QWidget *container = tabWidget->parentWidget();
    auto *boxLayout = container ? qobject_cast<QBoxLayout *>(container->layout()) : nullptr;
    if (!boxLayout) { return; }

    // Replacement container with a vertical layout into which all tab pages are moved.
    QWidget *stackedContainer = new QWidget(container);
    QVBoxLayout *stackedLayout = new QVBoxLayout(stackedContainer);
    stackedLayout->setContentsMargins(0, 0, 0, 0);

    // Detach all pages from the tab widget and arrange them on top of each other.
    // QTabWidget internally hides all pages except the currently active one (via its
    // internal QStackedWidget) -> removeTab() does NOT undo this, so the previously
    // inactive page(s) (e.g. "Advanced") would otherwise stay invisible -> force them visible.
    while (tabWidget->count() > 0)
    {
        QWidget *page = tabWidget->widget(0);
        tabWidget->removeTab(0);
        stackedLayout->addWidget(page);
        page->setVisible(true);
    }

    const int index = boxLayout->indexOf(tabWidget);
    boxLayout->removeWidget(tabWidget);
    if (index >= 0) { boxLayout->insertWidget(index, stackedContainer); }
    else { boxLayout->addWidget(stackedContainer); }

    tabWidget->setParent(nullptr);
    tabWidget->deleteLater();
}

void CMiniLoginWidget::moveModelWidgetBelowTypeAndAirline()
{
    // "lbl_Model"/"le_SimulatorModel" originally live in the "Advanced" tab's grid layout
    // (gl_advanced). After stackOwnAircraftSections() merged "General"/"Advanced" into a single
    // vertical stack, move both widgets into the "General" section's grid layout (gl_general),
    // directly below "Type" (row 1) / "Airline" (row 1), i.e. as a new row 2.
    COwnAircraftComponent *ownAircraftComponent =
        this->findChild<COwnAircraftComponent *>(QStringLiteral("comp_OwnAircraft"));
    if (!ownAircraftComponent) { return; }

    QLabel *lblModel = ownAircraftComponent->findChild<QLabel *>(QStringLiteral("lbl_Model"));
    QLineEdit *leSimulatorModel = ownAircraftComponent->findChild<QLineEdit *>(QStringLiteral("le_SimulatorModel"));
    if (!lblModel || !leSimulatorModel) { return; }

    auto *generalLayout = ownAircraftComponent->findChild<QGridLayout *>(QStringLiteral("gl_general"));
    auto *advancedLayout = ownAircraftComponent->findChild<QGridLayout *>(QStringLiteral("gl_advanced"));
    if (!generalLayout) { return; }

    if (advancedLayout)
    {
        advancedLayout->removeWidget(lblModel);
        advancedLayout->removeWidget(leSimulatorModel);
    }

    constexpr int newRow = 2;
    generalLayout->addWidget(lblModel, newRow, 0);
    generalLayout->addWidget(leSimulatorModel, newRow, 2, 1, 6);
}

void CMiniLoginWidget::insertSimBriefSection()
{
    // comp_ModelStringCompleter lives in "Advanced" tab's grid layout (gl_advanced), row 2, column 2
    // (see hideOwnAircraftModelLiveryWidgets(): it is hidden there, but the grid position is still
    // used as the anchor for inserting the new SimBrief button/info rows directly below it).
    COwnAircraftComponent *ownAircraftComponent =
        this->findChild<COwnAircraftComponent *>(QStringLiteral("comp_OwnAircraft"));
    if (!ownAircraftComponent) { return; }

    auto *advancedLayout = ownAircraftComponent->findChild<QGridLayout *>(QStringLiteral("gl_advanced"));
    if (!advancedLayout) { return; }

    auto *simBriefButton = new QPushButton(tr("Load SimBrief flight plan"), ownAircraftComponent);
    m_lblSimBriefInfo1 = new QLabel(ownAircraftComponent);
    m_lblSimBriefInfo2 = new QLabel(ownAircraftComponent);
    m_lblSimBriefInfo2->setWordWrap(true);

    const int row = advancedLayout->rowCount();
    advancedLayout->addWidget(simBriefButton, row, 0, 1, 4);
    advancedLayout->addWidget(m_lblSimBriefInfo1, row + 1, 0, 1, 4);
    advancedLayout->addWidget(m_lblSimBriefInfo2, row + 2, 0, 1, 4);

    connect(simBriefButton, &QPushButton::pressed, this, &CMiniLoginWidget::onLoadFromSimBrief);
}

void CMiniLoginWidget::onLoadFromSimBrief()
{
    if (!sGui || sGui->isShuttingDown()) { return; }
    if (!m_simBriefDialog) { m_simBriefDialog = new CSimBriefDownloadDialog(this); }
    const int rv = m_simBriefDialog->exec();
    if (rv != QDialog::Accepted) { return; }

    const CUrl url = m_simBriefDialog->getSimBriefData().getUrlAndUsername();
    sGui->getFromNetwork(url.toNetworkRequest(), { this, &CMiniLoginWidget::onSimBriefResponse });
}

void CMiniLoginWidget::onSimBriefResponse(QNetworkReply *nwReplyPtr)
{
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
    if (!sGui || sGui->isShuttingDown()) { return; }
    if (nwReply->error() != QNetworkReply::NoError) { return; }

    const QString simBriefFP(nwReply->readAll());
    if (simBriefFP.isEmpty()) { return; }

    const CFlightPlan flightPlan = CFlightPlan::fromSimBriefFormat(simBriefFP);
    this->updateSimBriefInfoLabels(flightPlan);
    emit this->simBriefFlightPlanLoaded(flightPlan);
}

void CMiniLoginWidget::updateSimBriefInfoLabels(const CFlightPlan &flightPlan)
{
    if (!m_lblSimBriefInfo1 || !m_lblSimBriefInfo2) { return; }

    const QString aircraft = flightPlan.getAircraftInfo().getAircraftIcao().getDesignator();
    const QString origin = flightPlan.getOriginAirportIcao().asString();
    const QString destination = flightPlan.getDestinationAirportIcao().asString();
    m_lblSimBriefInfo1->setText(
        tr("Aircraft: %1  Departure: %2  Destination: %3").arg(aircraft, origin, destination));
    m_lblSimBriefInfo2->setText(tr("Route: %1").arg(flightPlan.getRoute()));
}

void CMiniLoginWidget::hideOwnAircraftModelLiveryWidgets()
{
    COwnAircraftComponent *ownAircraftComponent =
        this->findChild<COwnAircraftComponent *>(QStringLiteral("comp_OwnAircraft"));
    if (!ownAircraftComponent) { return; }

    const QStringList namesToHide {
        QStringLiteral("lbl_LiverySend"),
        QStringLiteral("cb_Livery"),
        QStringLiteral("le_SendLivery"),
        QStringLiteral("lbl_ModelSent"),
        QStringLiteral("cb_Model"),
        QStringLiteral("comp_ModelStringCompleter"),
        QStringLiteral("pb_Clear"),
    };
    for (const QString &name : namesToHide)
    {
        if (QWidget *widget = ownAircraftComponent->findChild<QWidget *>(name)) { widget->setVisible(false); }
    }
}

void CMiniLoginWidget::insertServerSelector()
{
    QGroupBox *gbOwnAircraft = this->findChild<QGroupBox *>(QStringLiteral("gb_OwnAircraft"));
    if (!gbOwnAircraft) { return; }

    QWidget *container = gbOwnAircraft->parentWidget();
    auto *boxLayout = container ? qobject_cast<QBoxLayout *>(container->layout()) : nullptr;
    if (!boxLayout) { return; }

    m_serverSelector = new CServerListSelector(container);

    const int index = boxLayout->indexOf(gbOwnAircraft);
    if (index >= 0) { boxLayout->insertWidget(index, m_serverSelector); }
    else { boxLayout->addWidget(m_serverSelector); }

    connect(m_serverSelector, &CServerListSelector::serverChanged, this, &CMiniLoginWidget::onServerSelectionChanged);

    this->reloadServerSelector();
}

void CMiniLoginWidget::reloadServerSelector()
{
    if (!m_serverSelector) { return; }

    // VATSIM FSD servers (same source as CNetworkDetailsComponent's "VATSIM" tab)
    CServerList vatsimServers;
    if (sGui && sGui->getIContextNetwork()) { vatsimServers = sGui->getIContextNetwork()->getVatsimFsdServers(); }
    vatsimServers.sortBy(&CServer::getName);

    // Configured "Other" servers plus predefined test servers (same source as CNetworkDetailsComponent's
    // "Other Servers" tab / CSettingsNetworkServersComponent)
    const CNetworkSetup networkSetup;
    const CServerList otherServers(networkSetup.getOtherServersPlusPredefinedServers());

    CServerList allServers(vatsimServers);
    for (const CServer &server : otherServers) { allServers.push_back(server); }
    m_serverSelector->setServers(allServers);

    // Restore the last server chosen in the Mini-GUI (independent from swiftguistandard)
    const QString lastServerName = m_lastServerSetting.get();
    if (!lastServerName.isEmpty()) { m_serverSelector->preSelect(lastServerName); }
}

void CMiniLoginWidget::onWebServiceDataRead(CEntityFlags::Entity entity, CEntityFlags::ReadState state, int number,
                                            const QUrl &url)
{
    Q_UNUSED(number)
    Q_UNUSED(url)
    if (!CEntityFlags::isFinishedReadState(state)) { return; }
    if (entity != CEntityFlags::VatsimDataFile) { return; }
    this->reloadServerSelector();
}

void CMiniLoginWidget::onServerSelectionChanged(const CServer &server)
{
    // Keep CNetworkDetailsComponent in sync, so the selection actually made here is used when
    // CLoginComponent::toggleNetworkConnection() establishes the connection (which internally
    // reads ui->comp_NetworkDetails->getCurrentServer(), depending on which tab is selected).
    if (m_networkDetailsComponent)
    {
        const bool isVatsim = server.getEcosystem().isSystem(CEcosystem::VATSIM);

        auto *tabWidget = m_networkDetailsComponent->findChild<QTabWidget *>(QStringLiteral("tw_Network"));
        auto *vatsimSelector =
            m_networkDetailsComponent->findChild<CServerListSelector *>(QStringLiteral("comp_VatsimServers"));
        auto *otherSelector =
            m_networkDetailsComponent->findChild<CServerListSelector *>(QStringLiteral("comp_OtherServers"));

        if (isVatsim)
        {
            if (vatsimSelector) { vatsimSelector->preSelect(server.getName()); }
            if (tabWidget) { tabWidget->setCurrentIndex(CNetworkDetailsComponent::LoginVATSIM); }
        }
        else
        {
            if (otherSelector) { otherSelector->preSelect(server.getName()); }
            if (tabWidget) { tabWidget->setCurrentIndex(CNetworkDetailsComponent::LoginOthers); }
        }
    }

    // Persist the choice (Mini-GUI specific setting, restored on next start)
    m_lastServerSetting.set(server.getName());
}

void CMiniLoginWidget::forceEnableLoginButton()
{
    // pb_Ok is part of the (private) Ui::CLoginComponent, but can still be found as a
    // QObject child via the object name from the Designer UI file.
    QPushButton *okButton = this->findChild<QPushButton *>(QStringLiteral("pb_Ok"));
    if (!okButton) { return; }

    // If already connected (disconnect button), always keep it active.
    if (this->isNetworkConnected())
    {
        okButton->setDisabled(false);
        return;
    }

    // In the connect state: enable independent of the simulator connection status,
    // BUT still block it if the model is not valid (no green frame), or, if the
    // "require flight plan" option is enabled, as long as no flight plan is available yet.
    const bool flightPlanOk = !m_requireFlightPlanSetting.get() || m_hasFlightPlan;
    okButton->setDisabled(!this->hasValidModelForConnect() || !flightPlanOk);
}

void CMiniLoginWidget::setFlightPlanAvailable(bool available)
{
    m_hasFlightPlan = available;
    this->forceEnableLoginButton();
}

void CMiniLoginWidget::setFlightPlanContent(const CFlightPlan &flightPlan)
{
    m_fpCallsign = flightPlan.getCallsign();
    m_fpAircraftIcao = flightPlan.getAircraftInfo().getAircraftIcao().getDesignator();
    this->updateAircraftMismatchWarning();
}

void CMiniLoginWidget::onOwnAircraftDataChanged() { this->updateAircraftMismatchWarning(); }

void CMiniLoginWidget::connectOwnAircraftChangeSignals()
{
    COwnAircraftComponent *ownAircraftComponent =
        this->findChild<COwnAircraftComponent *>(QStringLiteral("comp_OwnAircraft"));
    if (!ownAircraftComponent) { return; }

    if (QLineEdit *leCallsign = ownAircraftComponent->findChild<QLineEdit *>(QStringLiteral("le_Callsign")))
    {
        connect(leCallsign, &QLineEdit::textChanged, this, &CMiniLoginWidget::onOwnAircraftDataChanged);
    }
    if (auto *selectorAircraftIcao = ownAircraftComponent->findChild<CDbAircraftIcaoSelectorComponent *>(
            QStringLiteral("selector_AircraftIcao")))
    {
        connect(selectorAircraftIcao, &CDbAircraftIcaoSelectorComponent::changedAircraftIcao, this,
                &CMiniLoginWidget::onOwnAircraftDataChanged);
    }
    if (auto *selectorAirlineIcao =
            ownAircraftComponent->findChild<CDbAirlineIcaoSelectorComponent *>(QStringLiteral("selector_AirlineIcao")))
    {
        connect(selectorAirlineIcao, &CDbAirlineIcaoSelectorComponent::changedAirlineIcao, this,
                &CMiniLoginWidget::onOwnAircraftDataChanged);
    }
}

void CMiniLoginWidget::insertAircraftMismatchWarning()
{
    QGroupBox *gbOwnAircraft = this->findChild<QGroupBox *>(QStringLiteral("gb_OwnAircraft"));
    if (!gbOwnAircraft) { return; }

    QWidget *container = gbOwnAircraft->parentWidget();
    auto *boxLayout = container ? qobject_cast<QBoxLayout *>(container->layout()) : nullptr;
    if (!boxLayout) { return; }

    m_mismatchWarningLabel = new QLabel(container);
    m_mismatchWarningLabel->setStyleSheet(QStringLiteral("color: red; font-weight: bold;"));
    m_mismatchWarningLabel->setWordWrap(true);
    m_mismatchWarningLabel->setVisible(false);

    const int index = boxLayout->indexOf(gbOwnAircraft);
    if (index >= 0) { boxLayout->insertWidget(index + 1, m_mismatchWarningLabel); }
    else { boxLayout->addWidget(m_mismatchWarningLabel); }
}

void CMiniLoginWidget::updateAircraftMismatchWarning()
{
    if (!m_mismatchWarningLabel) { return; }

    COwnAircraftComponent *ownAircraftComponent =
        this->findChild<COwnAircraftComponent *>(QStringLiteral("comp_OwnAircraft"));
    if (!ownAircraftComponent)
    {
        m_mismatchWarningLabel->setVisible(false);
        return;
    }

    // Only compare once both sides actually have a value, to avoid false positives while either
    // "Own Aircraft" or the flight plan is still empty/not yet filled in.
    if (m_fpCallsign.isEmpty() && m_fpAircraftIcao.isEmpty())
    {
        m_mismatchWarningLabel->setVisible(false);
        return;
    }

    const COwnAircraftComponent::CGuiAircraftValues values = ownAircraftComponent->getAircraftValuesFromGui();
    const QString ownAircraftIcao = values.ownAircraftIcao.getDesignator();

    const bool callsignMismatch = !m_fpCallsign.isEmpty() && !values.ownCallsign.isEmpty() &&
                                  values.ownCallsign.asString() != m_fpCallsign.asString();
    const bool aircraftMismatch =
        !m_fpAircraftIcao.isEmpty() && !ownAircraftIcao.isEmpty() && ownAircraftIcao != m_fpAircraftIcao;

    if (callsignMismatch || aircraftMismatch)
    {
        QStringList lines;
        if (callsignMismatch)
        {
            lines << tr("Callsign differs: Own Aircraft '%1' vs. flight plan '%2'")
                         .arg(values.ownCallsign.asString(), m_fpCallsign.asString());
        }
        if (aircraftMismatch)
        {
            lines << tr("Aircraft type differs: Own Aircraft '%1' vs. flight plan '%2'")
                         .arg(ownAircraftIcao, m_fpAircraftIcao);
        }
        m_mismatchWarningLabel->setText(lines.join(QChar::LineFeed));
        m_mismatchWarningLabel->setVisible(true);
    }
    else { m_mismatchWarningLabel->setVisible(false); }
}

void CMiniLoginWidget::widenConnectButton()
{
    // pb_Ok is part of the (private) Ui::CLoginComponent; its .ui layout gives it only a
    // narrow, content-based width. For the Mini-GUI it should be wider/more prominent,
    // without touching the shared logincomponent.ui (see CONTRIBUTING.md).
    QPushButton *okButton = this->findChild<QPushButton *>(QStringLiteral("pb_Ok"));
    if (!okButton) { return; }

    constexpr int MinButtonWidth = 180;
    okButton->setMinimumWidth(MinButtonWidth);
}