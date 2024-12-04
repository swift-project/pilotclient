// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBREDUCEMODELDUPLICATES_H
#define SWIFT_GUI_COMPONENTS_DBREDUCEMODELDUPLICATES_H

#include <QDialog>
#include <QScopedPointer>

#include "core/progress.h"
#include "misc/settingscache.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/settings/modelsettings.h"

namespace Ui
{
    class CDbReduceModelDuplicates;
}
namespace swift::gui::components
{
    //! Reduce modelss
    class CDbReduceModelDuplicates : public QDialog, public swift::core::IProgressIndicator
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CDbReduceModelDuplicates(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDbReduceModelDuplicates() override;

        //! Set the models
        void setModels(const swift::misc::simulation::CAircraftModelList &models,
                       const swift::misc::simulation::CSimulatorInfo &simulator);

        //! Process models
        void process();

        //! \copydoc swift::core::IProgressIndicator::updateProgressIndicator
        virtual void updateProgressIndicator(int percentage) override;

        //! The models to be removed
        const swift::misc::simulation::CAircraftModelList &getRemoveCandidates() const { return m_removeCandidates; }

        //! Simulator
        const swift::misc::simulation::CSimulatorInfo &getSimulator() const { return m_simulator; }

    private:
        //! Clear progress bar
        void clearProgressBar();

        //! Stop
        void stop() { m_stop = true; }

        QScopedPointer<Ui::CDbReduceModelDuplicates> ui;
        bool m_stop = false;
        swift::misc::simulation::CSimulatorInfo m_simulator;
        swift::misc::simulation::CAircraftModelList m_models;
        swift::misc::simulation::CAircraftModelList m_removeCandidates;
        swift::misc::CSettingReadOnly<swift::misc::simulation::settings::TDistributorListPreferences>
            m_distributorPreferences { this }; //!< distributor preferences
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_DBREDUCEMODELDUPLICATES_H
