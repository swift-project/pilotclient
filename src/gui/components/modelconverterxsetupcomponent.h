// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_MODELCONVERTERXSETUPCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_MODELCONVERTERXSETUPCOMPONENT_H

#include <QFrame>

#include "misc/simulation/modelconverterx.h"

namespace Ui
{
    class CModelConverterXSetupComponent;
}
namespace swift::gui::components
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
        swift::misc::CSetting<swift::misc::simulation::settings::TModelConverterXBinary> m_setting { this };
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_MODELCONVERTERXSETUPCOMPONENT_H
