// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "miniloginwidget.h"

#include <QPushButton>

#include "core/context/contextnetwork.h"
#include "core/context/contextsimulator.h"
#include "gui/guiapplication.h"

using namespace swift::core;
using namespace swift::core::context;

CMiniLoginWidget::CMiniLoginWidget(QWidget *parent) : CLoginComponent(parent)
{
    // Basisklasse hat im Konstruktor bereits updateUiConnectState() aufgerufen
    // (Button evtl. deaktiviert, wenn kein Simulator verbunden) -> sofort korrigieren
    this->forceEnableLoginButton();

    // Basisklasse verbindet ihre eigenen (privaten) Slots bereits mit Qt::QueuedConnection
    // auf dieselben Signale. Da unsere Verbindung hier NACH der Basisklasse registriert wird,
    // wird unser Slot in der Qt-Ereignisschlange nach dem Slot der Basis ausgefuehrt
    // (Verbindungen werden in Registrierungsreihenfolge abgearbeitet) -> garantiert korrekte Reihenfolge.
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

    // Modell-Gueltigkeit (gruener Rahmen) hat sich geaendert -> Connect-Button-Status neu bewerten
    connect(this, &CLoginComponent::modelValidityChanged, this, &CMiniLoginWidget::forceEnableLoginButton,
            Qt::QueuedConnection);
}

CMiniLoginWidget::~CMiniLoginWidget() {}

void CMiniLoginWidget::forceEnableLoginButton()
{
    // pb_Ok ist Teil des (privaten) Ui::CLoginComponent, aber als QObject-Kind
    // ueber den Objektnamen aus dem Designer-UI-File weiterhin auffindbar.
    QPushButton *okButton = this->findChild<QPushButton *>(QStringLiteral("pb_Ok"));
    if (!okButton) { return; }

    // Wenn bereits verbunden (Disconnect-Button), immer aktiv lassen.
    if (this->isNetworkConnected())
    {
        okButton->setDisabled(false);
        return;
    }

    // Im Connect-Zustand: unabhaengig vom Simulatorverbindungsstatus aktivieren,
    // ABER weiterhin blockieren, wenn das Modell nicht gueltig ist (kein gruener Rahmen).
    okButton->setDisabled(!this->hasValidModelForConnect());
}