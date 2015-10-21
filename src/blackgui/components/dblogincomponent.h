/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBLOGINCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBLOGINCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackcore/data/globalsetup.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CDbLoginComponent; }

namespace BlackGui
{
    namespace Components
    {
        /**
         * Login to DB
         */
        class BLACKGUI_EXPORT CDbLoginComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbLoginComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CDbLoginComponent();

        private:
            QScopedPointer<Ui::CDbLoginComponent> ui;
            BlackCore::CData<BlackCore::Data::GlobalSetup> m_setup {this}; //!< data cache

            //! Overlay messages
            void displayOverlayMessages(const BlackMisc::CStatusMessageList &msgs);

        private slots:
            //! Login
            void ps_onLoginClicked();

        };

    } // ns
} // ns

#endif // guard
