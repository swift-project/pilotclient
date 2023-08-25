// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_CORESTATUSCOMPONENT_H
#define BLACKGUI_COMPONENTS_CORESTATUSCOMPONENT_H

#include "blackgui/blackguiexport.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

class QWidget;

namespace Ui
{
    class CCoreStatusComponent;
}

namespace BlackGui::Components
{
    //! Display status information about the core
    class BLACKGUI_EXPORT CCoreStatusComponent : public QFrame
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

} // namespace

#endif // guard
