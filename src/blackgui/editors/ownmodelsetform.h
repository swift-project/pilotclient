/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_OWNMODELSETFORM_H
#define BLACKGUI_EDITORS_OWNMODELSETFORM_H

#include "form.h"
#include "blackcore/simulator/simulatorsettings.h"
#include "blackmisc/simulation/distributorlist.h"

#include <QObject>
#include <QScopedPointer>

namespace Ui { class COwnModelSetForm; }
namespace BlackGui
{
    namespace Editors
    {
        /*!
         * Selection for own model set
         */
        class COwnModelSetForm : public CForm
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit COwnModelSetForm(QWidget *parent = nullptr);

            //! Destructor
            ~COwnModelSetForm();

            //! Reload data
            void reloadData();

            //! Selected providers?
            bool selectedDistributors() const;

            //! Get selected providers
            BlackMisc::Simulation::CDistributorList getSelectedDistributors() const;

            //! Current simulator
            void setSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Only DB data
            bool dbDataOnly() const;

            //! DB ICAO codes
            bool dbIcaoCodesOnly() const;

            //! Request incremental build
            bool incrementalBuild() const;

            //! Distributors (from preferences or web)
            BlackMisc::Simulation::CDistributorList getDistributors() const;

            //! Preferences for given simulator
            bool hasDIstributorPreferences() const;

            //! \name Form functions, here not used
            //! \@{
            virtual void setReadOnly(bool readOnly) override { Q_UNUSED(readOnly); }
            //! \@}

        signals:
            //! Simulator changed
            void simulatorChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        private slots:
            //! Changed preferences
            void ps_preferencesChanged();

            //! Simulator changed
            void ps_simulatorChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        private:
            QScopedPointer<Ui::COwnModelSetForm>  ui;
            BlackMisc::Simulation::CSimulatorInfo m_simulator;
            BlackMisc::CSetting<BlackCore::Simulator::TDistributorListPreferences> m_distributorPreferences { this, &COwnModelSetForm::ps_preferencesChanged };
        };
    } // ns
} // ns

#endif // guard
