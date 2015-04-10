/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_INTERNALSCOMPONENT_H
#define BLACKGUI_INTERNALSCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "enableforruntime.h"
#include <QWidget>
#include <QScopedArrayPointer>

namespace Ui { class CInternalsComponent; }

namespace BlackGui
{
    namespace Components
    {

        //! Internals for debugging, statistics
        class BLACKGUI_EXPORT CInternalsComponent :
            public QWidget,
            public CEnableForRuntime
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CInternalsComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CInternalsComponent();

        protected:
            //! \copydoc CEnableForRuntime::runtimeHasBeenSet
            virtual void runtimeHasBeenSet() override;

            //! \copydoc QWidget::showEvent
            virtual void showEvent(QShowEvent *event) override;

        private slots:
            //! Send aircraft parts
            void ps_sendAircraftParts();

            //! Set all lights
            void ps_setAllLights();

            //! Set all engines
            void ps_setAllEngines();

            //! GUI to Json
            void ps_guiToJson();

            //! Enable / disable debugging
            void ps_enableDebug(int state);

        private:
            QScopedPointer<Ui::CInternalsComponent> ui;

            //! Get parts object
            BlackMisc::Aviation::CAircraftParts guiToAircraftParts() const;

            //! GUI set by parts
            void partsToGui(const BlackMisc::Aviation::CAircraftParts &parts);

            //! Set the context flags
            void contextFlagsToGui();
        };

    } // namespace
} // namespace

#endif // guard
