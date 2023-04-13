/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_BACKGROUNDVALIDATION_H
#define BLACKMISC_SIMULATION_BACKGROUNDVALIDATION_H

#include "blackmisc/simulation/settings/modelmatchersettings.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/worker.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/blackmiscexport.h"

#include <QMap>
#include <QReadWriteLock>
#include <atomic>

namespace BlackMisc::Simulation
{
    //! Validate model files from the sets and check if the model still exists
    class BLACKMISC_EXPORT CBackgroundValidation : public CContinuousWorker
    {
        Q_OBJECT

    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! Constructor
        CBackgroundValidation(QObject *owner);

        //! Corresponding simulator
        //! \threadsafe
        void setCurrentSimulator(const CSimulatorInfo &simulator, const QString &simDirectory);

        //! Was already checked for simulator?
        //! \threadsafe
        bool wasAlreadyChecked(const CSimulatorInfo &simulator) const;

        //! Validation in progress
        //! \threadsafe
        bool isValidating() const { return m_inWork; }

        //! Reset checked for simulator
        //! \threadsafe
        void resetAlreadyChecked(const CSimulatorInfo &simulator);

        //! Corresponding simulator
        //! \threadsafe
        BlackMisc::Simulation::CSimulatorInfo getCurrentSimulator() const;

        //! Trigger a validation, returns false if "work in progress"
        //! \threadsafe
        bool triggerValidation(const CSimulatorInfo &simulator, const QString &simDirectory);

        //! Request last results (again), if there are any
        //! \remark emits CBackgroundValidation::validated signal
        //! \threadsafe
        bool requestLastValidationResults();

    signals:
        //! Validating
        void validating(bool running);

        //! Validated for simulator
        void validated(const CSimulatorInfo &simulator, const CAircraftModelList &validModels, const CAircraftModelList &invalidModels, bool stopped, const CStatusMessageList &msgs);

    protected:
        //! \copydoc CContinuousWorker::beforeQuit
        virtual void beforeQuit() noexcept override;

    private:
        mutable QReadWriteLock m_lock; //!< lock snapshot
        std::atomic_bool m_inWork { false }; //!< indicates a running update
        std::atomic_bool m_wasStopped { false }; //!< has been stopped or should be stopped
        CSimulatorInfo m_simulator; //!< simulator
        QString m_simDirectory; //!< corresponding sim directory

        // last result values, mostly needed when running in the distributed swift system and we want to get the values
        CAircraftModelList m_lastResultValid;
        CAircraftModelList m_lastResultInvalid;
        CSimulatorInfo m_lastResultSimulator;
        CStatusMessageList m_lastResultMsgs;
        bool m_lastResultWasStopped = false;
        std::atomic_int m_timerBasedRuns { 0 };

        QMap<CSimulatorInfo, CStatusMessageList> m_checkedSimulatorMsgs; //!< all simulators ever checked
        CSetting<Settings::TModelMatching> m_matchingSettings { this }; //!< settings

        // Set/caches as member as we are in own thread, central instance will not work
        Data::CModelSetCaches m_modelSets { false, this };

        //! Do the validation checks
        void doWork();
    };
} // ns
#endif // guard
