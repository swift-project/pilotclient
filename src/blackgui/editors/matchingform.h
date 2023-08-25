// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_EDITORS_MATCHINGFORM_H
#define BLACKGUI_EDITORS_MATCHINGFORM_H

#include "blackgui/editors/form.h"
#include "blackmisc/simulation/aircraftmatchersetup.h"
#include "blackmisc/directories.h"

#include <QScopedPointer>

namespace Ui
{
    class CMatchingForm;
}
namespace BlackGui::Editors
{
    //! Matching form
    class CMatchingForm : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CMatchingForm(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CMatchingForm() override;

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        virtual BlackMisc::CStatusMessageList validate(bool withNestedForms) const override;
        //! @}

        //! Set valued
        void setValue(const BlackMisc::Simulation::CAircraftMatcherSetup &setup);

        //! Value
        BlackMisc::Simulation::CAircraftMatcherSetup value() const;

        //! Clear data
        void clear();

    private:
        //! Algorithm
        BlackMisc::Simulation::CAircraftMatcherSetup::MatchingAlgorithm algorithm() const;

        //! Mode
        BlackMisc::Simulation::CAircraftMatcherSetup::MatchingMode matchingMode() const;

        //! Stragey
        BlackMisc::Simulation::CAircraftMatcherSetup::PickSimilarStrategy pickStrategy() const;

        //! Set the pick strategy
        void setPickStrategy(const BlackMisc::Simulation::CAircraftMatcherSetup &setup);

        //! Set the matching mode
        void setMatchingAlgorithm(const BlackMisc::Simulation::CAircraftMatcherSetup &setup);

        //! Algorithm has been toggled
        void onAlgorithmChanged();

        //! Airline group changed
        void onAirlineGroupChanged();

        //! @{
        //! Reset
        void resetByAlgorithm();
        void resetAll();
        //! @}

        //! Directory browser
        void fileDialog();

        BlackMisc::CSetting<BlackMisc::Settings::TDirectorySettings> m_directories { this }; //!< the swift directories
        QScopedPointer<Ui::CMatchingForm> ui;
    };
} // ns

#endif // guard
