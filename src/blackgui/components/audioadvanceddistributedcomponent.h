// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_AUDIOADVANCEDDISTRIBUTEDCOMPONENT_H
#define BLACKGUI_COMPONENTS_AUDIOADVANCEDDISTRIBUTEDCOMPONENT_H

#include "blackmisc/audio/audiodeviceinfo.h"

#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CAudioAdvancedDistributedComponent;
}
namespace BlackGui::Components
{
    //! Adv. and sitributed audio setup
    class CAudioAdvancedDistributedComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CAudioAdvancedDistributedComponent(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CAudioAdvancedDistributedComponent() override;

        //! Reload registered devices
        void reloadRegisteredDevices();

    private:
        QScopedPointer<Ui::CAudioAdvancedDistributedComponent> ui;

        //! Contexts
        static bool hasContexts();
    };
} // ns

#endif // guard
