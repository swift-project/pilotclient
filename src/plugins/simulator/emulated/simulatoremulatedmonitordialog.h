// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_EMULATED_SIMULATOREMULATEDMONITORDIALOG_H
#define BLACKSIMPLUGIN_EMULATED_SIMULATOREMULATEDMONITORDIALOG_H

#include "misc/simulation/simulatedaircraft.h"
#include "misc/network/textmessage.h"
#include "misc/statusmessagelist.h"
#include "misc/logcategories.h"
#include "misc/identifiable.h"

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
    class CSimulatorEmulatedMonitorDialog : public QDialog, public swift::misc::CIdentifiable
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
        void appendStatusMessageToList(const swift::misc::CStatusMessage &statusMessage);

        //! Append status messages
        void appendStatusMessagesToList(const swift::misc::CStatusMessageList &statusMessages);

        //! Receiving call to be written in log widget
        void appendReceivingCall(const QString &function, const QString &p1 = {}, const QString &p2 = {}, const QString &p3 = {});

        //! Sending call to be written in log widget
        void appendSendingCall(const QString &function, const QString &p1 = {}, const QString &p2 = {}, const QString &p3 = {});

        //! Display status message
        void displayStatusMessage(const swift::misc::CStatusMessage &message);

        //! Display text message
        void displayTextMessage(const swift::misc::network::CTextMessage &message);

        //! The title
        void updateWindowTitleAndUiValues(const swift::misc::simulation::CSimulatorInfo &info);

    private:
        static int constexpr MaxLogMessages = 500; //!< desired log message number

        //! Append a function call as status message
        void appendFunctionCall(const QString &function, const QString &p1 = {}, const QString &p2 = {}, const QString &p3 = {});

        //! UI values changed
        void onSimulatorValuesChanged();

        //! Transmission values (COM unit volume etc.)
        void onSavedComTransmissionValues(swift::misc::aviation::CComSystem::ComUnit unit);

        //! Cockpit COM values changed
        void changeComFromUi(const swift::misc::simulation::CSimulatedAircraft &aircraft);

        //! SELCAL values changed
        void changeSelcalFromUi(const swift::misc::aviation::CSelcal &selcal);

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
