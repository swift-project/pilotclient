// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_AUDIOADVANCEDDISTRIBUTEDCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_AUDIOADVANCEDDISTRIBUTEDCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

#include "misc/audio/audiodeviceinfo.h"

namespace Ui
{
    class CAudioAdvancedDistributedComponent;
}
namespace swift::gui::components
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
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_AUDIOADVANCEDDISTRIBUTEDCOMPONENT_H
