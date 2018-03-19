/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_INTERPOLATIONCOMPONENT_H
#define BLACKGUI_COMPONENTS_INTERPOLATIONCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>
#include "blackgui/blackguiexport.h"

namespace Ui { class CInterpolationComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! Interpolation
        class BLACKGUI_EXPORT CInterpolationComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CInterpolationComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CInterpolationComponent();

        signals:
            //! Request redering restrictions widget
            void requestRenderingRestrictionsWidget();

        private:
            QScopedPointer<Ui::CInterpolationComponent> ui;
        };
    } // ns
} // ns

#endif // guard
