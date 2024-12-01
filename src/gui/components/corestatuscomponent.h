// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_CORESTATUSCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_CORESTATUSCOMPONENT_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"

class QWidget;

namespace Ui
{
    class CCoreStatusComponent;
}

namespace swift::gui::components
{
    //! Display status information about the core
    class SWIFT_GUI_EXPORT CCoreStatusComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCoreStatusComponent(QWidget *parent = nullptr);

        //! Destructor
        ~CCoreStatusComponent();

    private:
        QScopedPointer<Ui::CCoreStatusComponent> ui;
    };

} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_CORESTATUSCOMPONENT_H
