// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/backgroundvalidation.h"

#include <QDateTime>

#include "misc/eventloop.h"
#include "misc/logmessage.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/aircraftmodelutils.h"
#include "misc/threadutils.h"

using namespace swift::misc::network;
using namespace swift::misc::simulation::data;

namespace swift::misc::simulation
{
    const QStringList &CBackgroundValidation::getLogCategories()
    {
        static const QStringList cats({ CLogCategories::worker(), CLogCategories::modelSetCache() });
        return cats;
    }

    CBackgroundValidation::CBackgroundValidation(QObject *owner) : CContinuousWorker(owner, "Background validation")
    {
        connect(&m_updateTimer, &QTimer::timeout, this, &CBackgroundValidation::doWork);
        m_updateTimer.setInterval(60 * 1000);
    }

    void CBackgroundValidation::setCurrentSimulator(const CSimulatorInfo &simulator, const QString &simDirectory,
                                                    const QStringList &modelDirList)
    {
        QWriteLocker l(&m_lock);
        m_simulator = simulator;
        m_simDirectory = simDirectory;
        m_modelDirList = modelDirList;
    }

    bool CBackgroundValidation::wasAlreadyChecked(const CSimulatorInfo &simulator) const
    {
        QReadLocker l(&m_lock);
        return m_checkedSimulatorMsgs.contains(simulator);
    }

    void CBackgroundValidation::resetAlreadyChecked(const CSimulatorInfo &simulator)
    {
        QWriteLocker l(&m_lock);
        m_checkedSimulatorMsgs.remove(simulator);
    }

    CSimulatorInfo CBackgroundValidation::getCurrentSimulator() const
    {
        QReadLocker l(&m_lock);
        return m_simulator;
    }

    bool CBackgroundValidation::triggerValidation(const CSimulatorInfo &simulator, const QString &simDirectory)
    {
        const QPointer<CBackgroundValidation> myself(this);
        if (simulator.isNoSimulator()) { return this->requestLastValidationResults(); }

        {
            QWriteLocker l(&m_lock);
            if (m_inWork) { return false; }
            m_simulator = simulator;
            m_simDirectory = simDirectory;
            m_checkedSimulatorMsgs.remove(simulator);
        }
        QTimer::singleShot(5, this, [=] {
            if (!myself) { return; }
            myself->doWork();
        });
        return true;
    }

    bool CBackgroundValidation::requestLastValidationResults()
    {
        CAircraftModelList valid;
        CAircraftModelList invalid;
        CAircraftModelList models;
        CStatusMessageList msgs;
        CSimulatorInfo simulator;
        bool wasStopped = false;
        {
            QReadLocker l(&m_lock);
            simulator = m_lastResultSimulator;
            valid = m_lastResultValid;
            invalid = m_lastResultInvalid;
            msgs = m_lastResultMsgs;
            wasStopped = m_lastResultWasStopped;
        }
        if (m_lastResultSimulator.isUnspecified()) { return false; }
        emit this->validated(simulator, valid, invalid, wasStopped, msgs);
        return true;
    }

    void CBackgroundValidation::beforeQuit() noexcept
    {
        m_wasStopped = true; // stop in utility functions
        this->stopUpdateTimer();
    }

    void CBackgroundValidation::doWork()
    {
        if (m_inWork) { return; }
        m_inWork = true;
        emit this->validating(true);

        const bool isTimerBased = (QObject::sender() == &m_updateTimer);
        CAircraftModelList valid;
        CAircraftModelList invalid;
        CStatusMessageList msgs;
        bool validated = false;
        bool onlyErrorsAndWarnings = false;
        const CSimulatorInfo simulator = this->getCurrentSimulator();
        const qint64 started = QDateTime::currentMSecsSinceEpoch();
        m_wasStopped = false;

        do {
            if (!simulator.isSingleSimulator()) { break; }
            if (this->wasAlreadyChecked(simulator)) { break; }

            const CAircraftMatcherSetup setup = m_matchingSettings.get();
            if (!setup.doVerificationAtStartup()) { break; }

            onlyErrorsAndWarnings = setup.onlyShowVerificationWarningsAndErrors();
            const CAircraftModelList models = m_modelSets.getCachedModels(simulator);
            const qint64 now = QDateTime::currentMSecsSinceEpoch();
            validated = true;

            if (models.isEmpty())
            {
                msgs.push_back(
                    CStatusMessage(this, CStatusMessage::SeverityWarning,
                                   QStringLiteral("No models in set for  '%1'").arg(simulator.toQString(true))));
            }
            else
            {
                msgs = CAircraftModelUtilities::validateModelFiles(simulator, models, valid, invalid, false, 25,
                                                                   m_wasStopped, m_simDirectory);
            }

            const qint64 deltaTimeMs = now - started;
            msgs.push_back(CStatusMessage(this, CStatusMessage::SeverityInfo,
                                          QStringLiteral("Validated in %1ms").arg(deltaTimeMs)));
            msgs.sortBySeverityHighestFirst();
            msgs.freezeOrder();

            QWriteLocker l(&m_lock);
            m_lastResultValid = valid;
            m_lastResultInvalid = invalid;
            m_lastResultWasStopped = m_wasStopped;
            m_lastResultSimulator = simulator;
            m_lastResultMsgs = msgs;
            m_checkedSimulatorMsgs.insert(simulator, msgs);
        }
        while (false);

        m_inWork = false;
        if (isTimerBased)
        {
            m_timerBasedRuns++;

            // stop timer after some runs
            if (m_timerBasedRuns > 3) { m_updateTimer.stop(); }
        }

        emit this->validating(false);
        if (validated)
        {
            const bool e = !onlyErrorsAndWarnings || (!invalid.isEmpty() || msgs.hasWarningOrErrorMessages());
            if (e || !isTimerBased) { emit this->validated(simulator, valid, invalid, m_wasStopped, msgs); }
        }
    }
} // namespace swift::misc::simulation
