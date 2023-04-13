/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
