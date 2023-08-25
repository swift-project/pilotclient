// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_MODELCONVERTERXSETUPCOMPONENT_H
#define BLACKGUI_COMPONENTS_MODELCONVERTERXSETUPCOMPONENT_H

#include "blackmisc/simulation/modelconverterx.h"
#include <QFrame>

namespace Ui
{
    class CModelConverterXSetupComponent;
}
namespace BlackGui::Components
{
    /*!
     * Setup to start ModelConverterX
     */
    class CModelConverterXSetupComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CModelConverterXSetupComponent(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CModelConverterXSetupComponent();

    private:
        //! File dialog
        void selectBinary();

        //! Save the settings
        void saveSettings();

        QScopedPointer<Ui::CModelConverterXSetupComponent> ui;
        BlackMisc::CSetting<BlackMisc::Simulation::Settings::TModelConverterXBinary> m_setting { this };
    };
} // ns

#endif // guard
