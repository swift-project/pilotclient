// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_INTERPOLATIONCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_INTERPOLATIONCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"

namespace Ui
{
    class CInterpolationComponent;
}
namespace swift::gui::components
{
    //! Interpolation
    class SWIFT_GUI_EXPORT CInterpolationComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CInterpolationComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CInterpolationComponent() override;

    signals:
        //! Request redering restrictions widget
        void requestRenderingRestrictionsWidget();

    private:
        QScopedPointer<Ui::CInterpolationComponent> ui;

        //! Display log messages
        void displayInterpolationMessages();
    };
} // namespace swift::gui::components

#endif // guard
