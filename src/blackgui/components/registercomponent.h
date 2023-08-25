// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef BLACKGUI_COMPONENTS_REGISTERCOMPONENT_H
#define BLACKGUI_COMPONENTS_REGISTERCOMPONENT_H

#include "blackgui/blackguiexport.h"

#include <QFrame>
#include <QObject>
#include <QTimer>
#include <QScopedPointer>

namespace Ui
{
    class CRegisterComponent;
}
namespace BlackGui::Components
{
    //! Show registered applications (registered with core) in the GUI
    //! \sa BlackCore::Context::IContextApplication::getRegisteredApplications
    class BLACKGUI_EXPORT CRegisterComponent : public QFrame
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
