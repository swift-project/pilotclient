// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_EDITORS_OWNMODELSETFORM_H
#define SWIFT_GUI_EDITORS_OWNMODELSETFORM_H

#include <QObject>
#include <QScopedPointer>

#include "gui/editors/form.h"
#include "misc/simulation/settings/modelsettings.h"

namespace Ui
{
    class COwnModelSetForm;
}
namespace swift::gui::editors
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
        void setSimulator(const swift::misc::simulation::CSimulatorInfo &simulator);

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
        swift::misc::simulation::CDistributorList getSelectedDistributors() const;

        //! Get shown distributors
        swift::misc::simulation::CDistributorList getShownDistributors() const;

        //! Distributors from preferences
        swift::misc::simulation::CDistributorList getDistributorsFromPreferences() const;

        //! All distributors
        swift::misc::simulation::CDistributorList getAllDistributors() const;

        //! Get distributors based on options
        swift::misc::simulation::CDistributorList getDistributorsBasedOnOptions() const;

        //! Preferences for given simulator?
        bool hasDistributorPreferences() const;

        //! \name Form functions, here not used
        //! @{
        virtual void setReadOnly(bool readOnly) override { Q_UNUSED(readOnly); }
        //! @}

    signals:
        //! Simulator changed
        void simulatorChanged(const swift::misc::simulation::CSimulatorInfo &simulator);

    private:
        //! Changed preferences
        void onPreferencesChanged();

        //! Simulator changed
        void onSimulatorChanged(const swift::misc::simulation::CSimulatorInfo &simulator);

        //! Display distributors based on checkboxes
        void changeDistributorDisplay();

        //! Init the options which distributors are displayed
        void initDistributorDisplay();

        //! Set mode for view
        void setDistributorView(bool hasPreferences);

        QScopedPointer<Ui::COwnModelSetForm> ui;
        swift::misc::simulation::CSimulatorInfo m_simulator;
        swift::misc::CSetting<swift::misc::simulation::settings::TDistributorListPreferences> m_distributorPreferences { this, &COwnModelSetForm::onPreferencesChanged };
    };
} // namespace swift::gui::editors

#endif // guard
