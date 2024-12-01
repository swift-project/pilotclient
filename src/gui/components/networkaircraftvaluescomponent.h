// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_NETWORKAIRCRAFTVALUESCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_NETWORKAIRCRAFTVALUESCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CNetworkAircraftValuesComponent;
}
namespace swift::gui::components
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
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_NETWORKAIRCRAFTVALUESCOMPONENT_H
