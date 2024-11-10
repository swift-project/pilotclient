// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_EDITORS_MATCHINGFORM_H
#define SWIFT_GUI_EDITORS_MATCHINGFORM_H

#include "gui/editors/form.h"
#include "misc/simulation/aircraftmatchersetup.h"
#include "misc/directories.h"

#include <QScopedPointer>

namespace Ui
{
    class CMatchingForm;
}
namespace swift::gui::editors
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
        virtual swift::misc::CStatusMessageList validate(bool withNestedForms) const override;
        //! @}

        //! Set valued
        void setValue(const swift::misc::simulation::CAircraftMatcherSetup &setup);

        //! Value
        swift::misc::simulation::CAircraftMatcherSetup value() const;

        //! Clear data
        void clear();

    private:
        //! Algorithm
        swift::misc::simulation::CAircraftMatcherSetup::MatchingAlgorithm algorithm() const;

        //! Mode
        swift::misc::simulation::CAircraftMatcherSetup::MatchingMode matchingMode() const;

        //! Stragey
        swift::misc::simulation::CAircraftMatcherSetup::PickSimilarStrategy pickStrategy() const;

        //! Set the pick strategy
        void setPickStrategy(const swift::misc::simulation::CAircraftMatcherSetup &setup);

        //! Set the matching mode
        void setMatchingAlgorithm(const swift::misc::simulation::CAircraftMatcherSetup &setup);

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

        swift::misc::CSetting<swift::misc::settings::TDirectorySettings> m_directories { this }; //!< the swift directories
        QScopedPointer<Ui::CMatchingForm> ui;
    };
} // ns

#endif // guard
