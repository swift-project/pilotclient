/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_INFOBARWEBREADERSSTATUSCOMPONENT_H
#define BLACKGUI_INFOBARWEBREADERSSTATUSCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/led.h"
#include "blackmisc/network/webdataservicesprovider.h"
#include <QFrame>
#include <QScopedPointer>
#include <QTimer>

namespace Ui { class CInfoBarWebReadersStatusComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! Info bar displaying status of web readers(swift DB, ...)
        class BLACKGUI_EXPORT CInfoBarWebReadersStatusComponent :
            public QFrame,
            public BlackMisc::Network::CWebDataServicesAware
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CInfoBarWebReadersStatusComponent(QWidget *parent = nullptr);

            //!Destructor
            ~CInfoBarWebReadersStatusComponent();

            //! Init the LEDs
            void initLeds();

            //! Set the provider
            virtual void setProvider(BlackMisc::Network::IWebDataServicesProvider *webDataReaderProvider) override;

        private slots:
            //! Data have been read
            void ps_dataRead(BlackMisc::Network::CDbFlags::Entity entity, BlackMisc::Network::CDbFlags::ReadState readState, int count);

            //! Check server status
            void ps_checkServerAndData();

        private:
            QScopedPointer<Ui::CInfoBarWebReadersStatusComponent> ui;
            QTimer m_timer { this };

            //! Set LED states
            void setLedReadStates(const QList<CLedWidget *> &leds, BlackMisc::Network::CDbFlags::ReadState readState);

            //! Set the LED read state
            void setLedReadState(CLedWidget *led, BlackMisc::Network::CDbFlags::ReadState readState);

            //! Maps entity to its id
            QList<CLedWidget *> entityToLeds(BlackMisc::Network::CDbFlags::Entity entity) const;

            //! All data read
            bool hasAllData() const;
        };
    }
}
#endif // guard
