// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_NETWORKAIRCRAFTVALUESCOMPONENT_H
#define BLACKGUI_COMPONENTS_NETWORKAIRCRAFTVALUESCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CNetworkAircraftValuesComponent;
}
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
