// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSMATCHINGCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSMATCHINGCOMPONENT_H

#include "blackmisc/simulation/aircraftmatchersetup.h"
#include <QFrame>
#include <QScopedPointer>

namespace BlackCore::Context
{
    class IContextSimulator;
}
namespace Ui
{
    class CSettingsMatchingComponent;
}
namespace BlackGui::Components
{
    //! Settings for matching component
    class CSettingsMatchingComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSettingsMatchingComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSettingsMatchingComponent() override;

        //! Get setup
        BlackMisc::Simulation::CAircraftMatcherSetup getMatchingSetup() const;

        //! Set setup
        void setMatchingSetup(const BlackMisc::Simulation::CAircraftMatcherSetup &setup);

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
        static BlackCore::Context::IContextSimulator *simulatorContext();
    };
} // ns

#endif // guard
