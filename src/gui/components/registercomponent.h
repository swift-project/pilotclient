// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_GUI_COMPONENTS_REGISTERCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_REGISTERCOMPONENT_H

#include "gui/swiftguiexport.h"

#include <QFrame>
#include <QObject>
#include <QTimer>
#include <QScopedPointer>

namespace Ui
{
    class CRegisterComponent;
}
namespace swift::gui::components
{
    //! Show registered applications (registered with core) in the GUI
    //! \sa swift::core::context::IContextApplication::getRegisteredApplications
    class SWIFT_GUI_EXPORT CRegisterComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CRegisterComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CRegisterComponent();

    private:
        QScopedPointer<Ui::CRegisterComponent> ui;
        QTimer m_updateTimer;

        //! Update data
        void update();
    };
} // ns

#endif // guard
