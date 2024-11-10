// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_CGSOURCESELECTOR_H
#define BLACKGUI_COMPONENTS_CGSOURCESELECTOR_H

#include "misc/simulation/settings/simulatorsettings.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CCGSourceSelector;
}
namespace BlackGui::Components
{
    //! CG (aka vertical offset) selector
    class CCGSourceSelector : public QFrame
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CCGSourceSelector(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CCGSourceSelector() override;

        //! @{
        //! Get/set value
        swift::misc::simulation::settings::CSimulatorSettings::CGSource getValue() const;
        void setValue(const swift::misc::simulation::settings::CSimulatorSettings &settings);
        void setValue(swift::misc::simulation::settings::CSimulatorSettings::CGSource source);
        //! @}

    private:
        //! Init the combobox
        void initComboBox();

        QScopedPointer<Ui::CCGSourceSelector> ui;
    };
} // ns

#endif // guard
