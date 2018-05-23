/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_ABOUTHTMLCOMPONENT_H
#define BLACKGUI_COMPONENTS_ABOUTHTMLCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CAboutHtmlComponent; }
namespace BlackGui
{
    namespace Components
    {
        /**
         * Display the HTML info "about swift"
         */
        class BLACKGUI_EXPORT CAboutHtmlComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAboutHtmlComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CAboutHtmlComponent();

        private:
            //! Load credits and legal info
            void loadAbout();

            QScopedPointer<Ui::CAboutHtmlComponent> ui;
        };
    } // ns
} // ns

#endif // guard
