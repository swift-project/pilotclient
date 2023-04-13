/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
