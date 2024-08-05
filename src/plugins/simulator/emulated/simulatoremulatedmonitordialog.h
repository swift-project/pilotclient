// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_EMULATED_SIMULATOREMULATEDMONITORDIALOG_H
#define BLACKSIMPLUGIN_EMULATED_SIMULATOREMULATEDMONITORDIALOG_H

#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/identifiable.h"

#include <QDialog>
#include <QScopedPointer>
#include <QTimer>

namespace Ui
{
    class CSimulatorEmulatedMonitorDialog;
}
namespace BlackSimPlugin::Emulated
{
    class CSimulatorEmulated;

    /*!
     * Monitor widget for the pseudo driver
     */
    class CSimulatorEmulatedMonitorDialog : public QDialog, public BlackMisc::CIdentifiable
    {
        Q_OBJECT

    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! Ctor
        explicit CSimulatorEmulatedMonitorDialog(CSimulatorEmulated *simulator, QWidget *parent = nullptr);

        //! Dtor
        virtual ~CSimulatorEmulatedMonitorDialog() override;

        //! Append status message
        void appendStatusMessageToList(const BlackMisc::CStatusMessage &statusMessage);

        //! Append status messages
        void appendStatusMessagesToList(const BlackMisc::CStatusMessageList &statusMessages);

        //! Receiving call to be written in log widget
        void appendReceivingCall(const QString &function, const QString &p1 = {}, const QString &p2 = {}, const QString &p3 = {});

        //! Sending call to be written in log widget
        void appendSendingCall(const QString &function, const QString &p1 = {}, const QString &p2 = {}, const QString &p3 = {});

        //! Display status message
        void displayStatusMessage(const BlackMisc::CStatusMessage &message);

        //! Display text message
        void displayTextMessage(const BlackMisc::Network::CTextMessage &message);

        //! The title
        void updateWindowTitleAndUiValues(const BlackMisc::Simulation::CSimulatorInfo &info);

    private:
        static int constexpr MaxLogMessages = 500; //!< desired log message number

        //! Append a function call as status message
        void appendFunctionCall(const QString &function, const QString &p1 = {}, const QString &p2 = {}, const QString &p3 = {});

        //! UI values changed
        void onSimulatorValuesChanged();

        //! Transmission values (COM unit volume etc.)
        void onSavedComTransmissionValues(BlackMisc::Aviation::CComSystem::ComUnit unit);

        //! Cockpit COM values changed
        void changeComFromUi(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        //! SELCAL values changed
        void changeSelcalFromUi(const BlackMisc::Aviation::CSelcal &selcal);

        //! Update aircraft situation
        void changeSituationFromUi();

        //! Change the aircraft parts
        void changePartsFromUi();

        //! UI values
        void setSimulatorUiValues();

        //! Set values from internal aircraft
        void setInternalAircraftUiValues();

        //! Timer based UI updates (pulling)
        void timerBasedUiUpdates();

        //! Reset statistics
        void resetStatistics();

        //! Interpolator log button pressed
        void interpolatorLogButton();

        //! Enable/disable the interpolation log buttons
        void enableInterpolationLogButtons(bool enable);

        //! Emit a simulator signal
        void emitSignal();

        //! Test data for auto publishing
        void addAutoPublishTestData();

        //! Can use the emulated simulator
        bool canUseSimulator() const;

        QScopedPointer<Ui::CSimulatorEmulatedMonitorDialog> ui;
        CSimulatorEmulated *m_simulator = nullptr;
        QTimer m_uiUpdateTimer;
    };
} // ns

#endif // guard
