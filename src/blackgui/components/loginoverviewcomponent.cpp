/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "ui_loginoverviewcomponent.h"
#include "loginoverviewcomponent.h"
#include "blackgui/components/serverlistselector.h"
#include "blackgui/components/dbquickmappingwizard.h"
#include "blackgui/editors/serverform.h"
#include "blackgui/editors/pilotform.h"
#include "blackgui/guiapplication.h"
#include "blackgui/loginmodebuttons.h"
#include "blackgui/ticklabel.h"
#include "blackgui/uppercasevalidator.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/webdataservices.h"
#include "blackcore/simulator.h"
#include "blackmisc/simulation/simulatorinternals.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/icons.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/crashhandler.h"
#include "blackconfig/buildconfig.h"

#include <QDialogButtonBox>
#include <QMessageBox>
#include <QGroupBox>
#include <QIntValidator>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QTabWidget>
#include <QTimer>
#include <QToolButton>
#include <QStringBuilder>
#include <QtGlobal>
#include <QPointer>
#include <QPair>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackCore;
using namespace BlackCore::Data;
using namespace BlackCore::Context;
using namespace BlackGui;

namespace BlackGui::Components
{
    const QStringList &CLoginOverviewComponent::getLogCategories()
    {
        static const QStringList cats { CLogCategories::guiComponent() };
        return cats;
    }

    CLoginOverviewComponent::CLoginOverviewComponent(QWidget *parent) :
        COverlayMessagesFrame(parent),
        ui(new Ui::CLoginOverviewComponent)
    {
        ui->setupUi(this);

        connect(ui->pb_Cancel,     &QPushButton::clicked, this, &CLoginOverviewComponent::cancel,          Qt::QueuedConnection);
        connect(ui->pb_Disconnect, &QPushButton::clicked, this, &CLoginOverviewComponent::toggleNetworkConnection, Qt::QueuedConnection);

        // overlay
        this->setOverlaySizeFactors(0.8, 0.5);
        this->setReducedInfo(true);
        this->setForceSmall(true);
        this->showKillButton(false);

        // forms
        ui->form_Pilot->setReadOnly(true);
        ui->form_Server->setReadOnly(true);

        // auto logoff
        // we decided to make it difficult for users to disable it
        if (!CBuildConfig::isLocalDeveloperDebugBuild())
        {
            ui->cb_AutoLogoff->setChecked(true);
        }

        // inital setup, if data already available
        ui->form_Pilot->validate();
        ui->cb_AutoLogoff->setChecked(m_networkSetup.useAutoLogoff());
    }

    CLoginOverviewComponent::~CLoginOverviewComponent()
    { }

    void CLoginOverviewComponent::setAutoLogoff(bool autoLogoff)
    {
        ui->cb_AutoLogoff->setChecked(autoLogoff);
    }

    void CLoginOverviewComponent::cancel()
    {
        this->closeOverlay();
        emit this->closeOverview();
    }

    void CLoginOverviewComponent::toggleNetworkConnection()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (!sGui->getIContextNetwork() || !sGui->getIContextAudio()) { return; }

        const bool isConnected = sGui && sGui->getIContextNetwork()->isConnected();
        m_networkSetup.setAutoLogoff(ui->cb_AutoLogoff->isChecked());

        CStatusMessage msg;
        if (!isConnected)
        {
            // void
        }
        else
        {
            // disconnect from network
            sGui->setExtraWindowTitle("");
            msg = sGui->getIContextNetwork()->disconnectFromNetwork();
        }

        // log message and trigger events
        msg.addCategories(this);
        CLogMessage::preformatted(msg);
        if (msg.isSuccess())
        {
            emit loginOrLogoffSuccessful();
        }
        else
        {
            this->cancel();
        }
    }

    void CLoginOverviewComponent::showCurrentValues()
    {
        if (!this->hasValidContexts()) { return; }
        const CServer server = sGui->getIContextNetwork()->getConnectedServer();
        ui->form_Server->setServer(server);
        ui->form_Server->resetToFirstTab();
        ui->form_Pilot->setUser(server.getUser());
        ui->le_LoginMode->setText(sGui->getIContextNetwork()->getLoginModeAsString());
        ui->le_PartnerCallsign->setText(sGui->getIContextNetwork()->getPartnerCallsign().asString());
        ui->comp_NetworkAircraft->showValues();
    }

    bool CLoginOverviewComponent::hasValidContexts() const
    {
        if (!sGui || !sGui->supportsContexts()) { return false; }
        if (sGui->isShuttingDown())          { return false; }
        if (!sGui->getIContextSimulator())   { return false; }
        if (!sGui->getIContextNetwork())     { return false; }
        if (!sGui->getIContextOwnAircraft()) { return false; }
        return true;
    }

} // namespace
