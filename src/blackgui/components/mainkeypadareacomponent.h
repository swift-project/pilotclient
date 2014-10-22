/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MAINKEYPADAREACOMPONENT_H
#define BLACKGUI_MAINKEYPADAREACOMPONENT_H

#include <QFrame>
#include <QScopedPointer>
#include "maininfoareacomponent.h"

namespace Ui { class CMainKeypadAreaComponent; }
namespace BlackGui
{
    namespace Components
    {

        //! Main keypad area as used with main info area
        //! \sa CMainInfoAreaComponent
        class CMainKeypadAreaComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CMainKeypadAreaComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CMainKeypadAreaComponent();

        signals:
            //! Button to select main info area has been pressed
            //! \sa CMainInfoAreaComponent
            void selectMainInfoAreaDockWidget(CMainInfoAreaComponent::InfoArea infoArea);

        private slots:
            //! Button was clicked
            void ps_buttonPressed();

            //! Button was double clicked
            void ps_buttonDoubleClicked();

        private:
            // if button is info area, identify it
            CMainInfoAreaComponent::InfoArea buttonToMainInfoArea(const QObject *button) const;

            QScopedPointer<Ui::CMainKeypadAreaComponent> ui;
        };

    } // namespace
} // namespace

#endif // guard
