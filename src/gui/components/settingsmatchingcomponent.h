// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SETTINGSMATCHINGCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_SETTINGSMATCHINGCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

#include "misc/simulation/aircraftmatchersetup.h"

namespace swift::core::context
{
    class IContextSimulator;
}
namespace Ui
{
    class CSettingsMatchingComponent;
}
namespace swift::gui::components
{
    //! Settings for matching component
    class CSettingsMatchingComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSettingsMatchingComponent(QWidget *parent = nullptr);

        //! Destructor
        ~CSettingsMatchingComponent() override;

        //! Get setup
        swift::misc::simulation::CAircraftMatcherSetup getMatchingSetup() const;

        //! Set setup
        void setMatchingSetup(const swift::misc::simulation::CAircraftMatcherSetup &setup);

        //! Show buttons
        void showButtons(bool show);

    private:
        QScopedPointer<Ui::CSettingsMatchingComponent> ui;

        //! Save pressed
        void onSavePressed() const;

        //! Reload pressed
        void onReloadPressed();

        //! Do re-matching
        void onMatchingsAgainPressed();

        //! Setup has been changed
        void onSetupChanged();

        //! Deferred reload
        void deferredReload(int deferMs);

        //! Network context
        static swift::core::context::IContextSimulator *simulatorContext();
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_SETTINGSMATCHINGCOMPONENT_H
