/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_COPYCONFIGURATION_H
#define BLACKGUI_COMPONENTS_COPYCONFIGURATION_H

#include "blackgui/blackguiexport.h"
#include <QFrame>

namespace Ui { class CCopyConfigurationComponent; }
namespace BlackGui
{
    namespace Components
    {
        /**
         * Copy configuration (ie settings and cache files)
         */
        class BLACKGUI_EXPORT CCopyConfigurationComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CCopyConfigurationComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CCopyConfigurationComponent();

        private:
            QScopedPointer<Ui::CCopyConfigurationComponent> ui;
        };
    } // ns
} // ns

#endif // guard
