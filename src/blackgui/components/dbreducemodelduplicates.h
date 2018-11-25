/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBREDUCEMODELDUPLICATES_H
#define BLACKGUI_COMPONENTS_DBREDUCEMODELDUPLICATES_H

#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/settings/modelsettings.h"
#include "blackmisc/settingscache.h"

#include <QDialog>
#include <QScopedPointer>

namespace Ui { class CDbReduceModelDuplicates; }
namespace BlackGui
{
    namespace Components
    {
        //! Reduce modelss
        class CDbReduceModelDuplicates : public QDialog
        {
            Q_OBJECT

        public:
            //! Ctor
            explicit CDbReduceModelDuplicates(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbReduceModelDuplicates();

            //! Set the models
            void setModels(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Process models
            void process();

            //! The models to be removed
            const BlackMisc::Simulation::CAircraftModelList &getRemoveCandidates() const { return m_removeCandidates; }

            //! Simulator
            const BlackMisc::Simulation::CSimulatorInfo &getSimulator() const { return m_simulator; }

        private:
            QScopedPointer<Ui::CDbReduceModelDuplicates> ui;
            BlackMisc::Simulation::CSimulatorInfo     m_simulator;
            BlackMisc::Simulation::CAircraftModelList m_models;
            BlackMisc::Simulation::CAircraftModelList m_removeCandidates;
            BlackMisc::CSettingReadOnly<BlackMisc::Simulation::Settings::TDistributorListPreferences> m_distributorPreferences { this }; //!< distributor preferences
        };
    } //ns
} //ns

#endif // guard
