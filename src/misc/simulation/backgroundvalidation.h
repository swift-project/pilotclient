// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_BACKGROUNDVALIDATION_H
#define SWIFT_MISC_SIMULATION_BACKGROUNDVALIDATION_H

#include <atomic>

#include <QMap>
#include <QReadWriteLock>

#include "misc/network/entityflags.h"
#include "misc/settingscache.h"
#include "misc/simulation/data/modelcaches.h"
#include "misc/simulation/settings/modelmatchersettings.h"
#include "misc/statusmessagelist.h"
#include "misc/swiftmiscexport.h"
#include "misc/worker.h"

namespace swift::misc::simulation
{
    //! Validate model files from the sets and check if the model still exists
    class SWIFT_MISC_EXPORT CBackgroundValidation : public CContinuousWorker
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
        swift::misc::simulation::CSimulatorInfo getCurrentSimulator() const;

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
        void validated(const CSimulatorInfo &simulator, const CAircraftModelList &validModels,
                       const CAircraftModelList &invalidModels, bool stopped, const CStatusMessageList &msgs);

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
        CSetting<settings::TModelMatching> m_matchingSettings { this }; //!< settings

        // Set/caches as member as we are in own thread, central instance will not work
        data::CModelSetCaches m_modelSets { false, this };

        //! Do the validation checks
        void doWork();
    };
} // namespace swift::misc::simulation
#endif // guard
