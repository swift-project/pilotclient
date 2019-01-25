/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "backgroundvalidation.h"
#include "blackmisc/simulation/aircraftmodelutils.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/threadutils.h"
#include "blackmisc/eventloop.h"
#include "blackmisc/logmessage.h"

#include <QDateTime>

using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation::Data;

namespace BlackMisc
{
    namespace Simulation
    {
        const CLogCategoryList &CBackgroundValidation::getLogCategories()
        {
            static const CLogCategoryList cats({ CLogCategory::worker(), CLogCategory::modelSetCache() });
            return cats;
        }

        CBackgroundValidation::CBackgroundValidation(QObject *owner) :
            CContinuousWorker(owner, "Background validation")
        {
            connect(&m_updateTimer, &QTimer::timeout, this, &CBackgroundValidation::doWork);
            m_updateTimer.setInterval(60 * 1000);
        }

        void CBackgroundValidation::setCurrentSimulator(const CSimulatorInfo &simulator)
        {
            QWriteLocker l(&m_lock);
            m_simulator = simulator;
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

        bool CBackgroundValidation::triggerValidation(const CSimulatorInfo &simulator)
        {
            const QPointer<CBackgroundValidation> myself(this);
            if (simulator.isNoSimulator())
            {
                return this->requestLastResults();
            }

            {
                QWriteLocker l(&m_lock);
                if (m_inWork) { return false; }
                m_simulator = simulator;
                m_checkedSimulatorMsgs.remove(simulator);
            }
            QTimer::singleShot(0, this, [ = ]
            {
                if (!myself) { return; }
                myself->doWork();
            });
            return true;
        }

        bool CBackgroundValidation::requestLastResults()
        {

            CAircraftModelList valid;
            CAircraftModelList invalid;
            CAircraftModelList models;
            CStatusMessageList msgs;
            CSimulatorInfo simulator;
            bool wasStopped = false;
            {
                QReadLocker l(&m_lock);
                simulator  = m_lastResultSimulator;
                valid      = m_lastResultValid;
                invalid    = m_lastResultInvalid;
                msgs       = m_lastResultMsgs;
                wasStopped = m_lastResultWasStopped;
            }
            if (m_lastResultSimulator.isUnspecified()) { return false; }
            emit this->validated(simulator, valid, invalid, wasStopped, msgs);
            return true;
        }

        void CBackgroundValidation::doWork()
        {
            if (m_inWork) { return; }
            m_inWork = true;
            emit this->validating(true);

            CAircraftModelList valid;
            CAircraftModelList invalid;
            CStatusMessageList msgs;
            bool wasStopped = false;
            bool validated  = false;
            const CSimulatorInfo simulator = this->getCurrentSimulator();
            const qint64 started = QDateTime::currentMSecsSinceEpoch();

            do
            {
                if (!simulator.isSingleSimulator())     { break; }
                if (this->wasAlreadyChecked(simulator)) { break; }

                const CAircraftMatcherSetup setup = m_matchingSettings.get();
                if (!setup.doVerificationAtStartup()) { break; }

                const CAircraftModelList models = m_modelSets.getCachedModels(simulator);
                msgs = CAircraftModelUtilities::validateModelFiles(models, valid, invalid, false, 25, wasStopped);

                const qint64 deltaTimeMs = QDateTime::currentMSecsSinceEpoch() - started;
                msgs.push_back(CStatusMessage(this, CStatusMessage::SeverityInfo, QStringLiteral("Validated in %1ms").arg(deltaTimeMs)));

                validated = true;

                QWriteLocker l(&m_lock);
                m_lastResultValid   = valid;
                m_lastResultInvalid = invalid;
                m_lastResultWasStopped = wasStopped;
                m_lastResultSimulator = simulator;
                m_lastResultMsgs = msgs;
                m_checkedSimulatorMsgs.insert(simulator, msgs);
            }
            while (false);


            m_inWork = false;
            emit this->validating(false);
            if (validated)
            {
                emit this->validated(simulator, valid, invalid, wasStopped, msgs);
            }
        }
    } // ns
} // ns
