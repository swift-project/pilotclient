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
#include "blackmisc/aviation/aircraftparts.h"

#include <QObject>
#include <QScopedPointer>
#include <QWidget>

class QShowEvent;

namespace Ui { class CInternalsComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! Internals for debugging, statistics
        class BLACKGUI_EXPORT CInternalsComponent :
            public QWidget
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CInternalsComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CInternalsComponent();

        protected:
            //! \copydoc QWidget::showEvent
            virtual void showEvent(QShowEvent *event) override;

        private slots:
            //! Send aircraft parts
            void ps_sendAircraftParts();

            //! Current parts in UI
            void ps_setCurrentParts();

            //! Enable / disable debugging
            void ps_enableDebug(int state);

            //! Send the text message
            void ps_sendTextMessage();

            //! Send a dummy status message
            void ps_logStatusMessage();

            //! Show log files
            void ps_showLogFiles();

            //! Request parts (aka aircraft config) from network
            void ps_requestPartsFromNetwork();

            //! Selector has been changed
            void ps_selectorChanged();

        private:
            QScopedPointer<Ui::CInternalsComponent> ui;

            //! Set the context flags
            void contextFlagsToGui();
        };
    } // namespace
} // namespace

#endif // guard
