/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_OWNMODELSETFORM_H
#define BLACKGUI_EDITORS_OWNMODELSETFORM_H

#include "blackgui/editors/form.h"
#include "blackmisc/simulation/settings/modelsettings.h"
#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class COwnModelSetForm;
}
namespace BlackGui::Editors
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
        virtual ~COwnModelSetForm() override;

        //! Reload data
        void reloadData();

        //! Current simulator
        void setSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        //! Selected distributors?
        bool optionUseSelectedDistributors() const;

        //! All distributors?
        bool optionUseAllDistributors() const;

        //! Only DB data
        bool optionDbDataOnly() const;

        //! DB ICAO codes
        bool optionDbIcaoCodesOnly() const;

        //! Request incremental build
        bool optionIncrementalBuild() const;

        //! Sort by distributor preferences
        bool optionSortByDistributorPreferences() const;

        //! Consolidate with DB data?
        bool optionConsolidateModelSetWithDbData() const;

        //! Get selected distributors
        BlackMisc::Simulation::CDistributorList getSelectedDistributors() const;

        //! Get shown distributors
        BlackMisc::Simulation::CDistributorList getShownDistributors() const;

        //! Distributors from preferences
        BlackMisc::Simulation::CDistributorList getDistributorsFromPreferences() const;

        //! All distributors
        BlackMisc::Simulation::CDistributorList getAllDistributors() const;

        //! Get distributors based on options
        BlackMisc::Simulation::CDistributorList getDistributorsBasedOnOptions() const;

        //! Preferences for given simulator?
        bool hasDistributorPreferences() const;

        //! \name Form functions, here not used
        //! @{
        virtual void setReadOnly(bool readOnly) override { Q_UNUSED(readOnly); }
        //! @}

    signals:
        //! Simulator changed
        void simulatorChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

    private:
        //! Changed preferences
        void onPreferencesChanged();

        //! Simulator changed
        void onSimulatorChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        //! Display distributors based on checkboxes
        void changeDistributorDisplay();

        //! Init the options which distributors are displayed
        void initDistributorDisplay();

        //! Set mode for view
        void setDistributorView(bool hasPreferences);

        QScopedPointer<Ui::COwnModelSetForm> ui;
        BlackMisc::Simulation::CSimulatorInfo m_simulator;
        BlackMisc::CSetting<BlackMisc::Simulation::Settings::TDistributorListPreferences> m_distributorPreferences { this, &COwnModelSetForm::onPreferencesChanged };
    };
} // ns

#endif // guard
