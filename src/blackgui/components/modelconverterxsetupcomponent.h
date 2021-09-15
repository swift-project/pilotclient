/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_MODELCONVERTERXSETUPCOMPONENT_H
#define BLACKGUI_COMPONENTS_MODELCONVERTERXSETUPCOMPONENT_H

#include "blackmisc/simulation/modelconverterx.h"
#include <QFrame>

namespace Ui { class CModelConverterXSetupComponent; }
namespace BlackGui::Components
{
    /**
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
