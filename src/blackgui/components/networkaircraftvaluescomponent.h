/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_NETWORKAIRCRAFTVALUESCOMPONENT_H
#define BLACKGUI_COMPONENTS_NETWORKAIRCRAFTVALUESCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CNetworkAircraftValuesComponent; }
namespace BlackGui::Components
{
    //! Display the network aircraft values as sent
    class CNetworkAircraftValuesComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CNetworkAircraftValuesComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CNetworkAircraftValuesComponent() override;

        //! Show the current values
        void showValues();

    private:
        QScopedPointer<Ui::CNetworkAircraftValuesComponent> ui;
    };
} // ns

#endif // guard
