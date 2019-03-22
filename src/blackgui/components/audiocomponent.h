/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AUDIOCOMPONENT_H
#define BLACKGUI_AUDIOCOMPONENT_H

#include "blackgui/blackguiexport.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace Ui { class CAudioComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! Audio component, volume, ...
        class BLACKGUI_EXPORT CAudioComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAudioComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CAudioComponent() override;

        private:
            QScopedPointer<Ui::CAudioComponent> ui;
        };
    } // namespace
} // namespace


#endif // guard
