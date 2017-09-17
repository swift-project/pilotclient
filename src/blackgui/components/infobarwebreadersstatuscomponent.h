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
#include "blackmisc/network/entityflags.h"

#include <QFrame>
#include <QList>
#include <QObject>
#include <QScopedPointer>
#include <QTimer>

class QWidget;

namespace Ui { class CInfoBarWebReadersStatusComponent; }
namespace BlackGui
{
    class CLedWidget;
    namespace Components
    {
        //! Info bar displaying status of web readers(swift DB, ...)
        class BLACKGUI_EXPORT CInfoBarWebReadersStatusBase : public QFrame
        {
            Q_OBJECT

        public:
            //!Destructor
            virtual ~CInfoBarWebReadersStatusBase();

            //! Init the LEDs
            void initLeds();

        protected:
            //! Constructor
            explicit CInfoBarWebReadersStatusBase(QWidget *parent = nullptr);

            //! Init
            void init();

            //! Set LED states
            void setLedReadStates(const QList<CLedWidget *> &leds, BlackMisc::Network::CEntityFlags::ReadState readState);

            //! Set the LED read state
            void setLedReadState(CLedWidget *led, BlackMisc::Network::CEntityFlags::ReadState readState);

            //! Maps entity to its id
            QList<CLedWidget *> entityToLeds(BlackMisc::Network::CEntityFlags::Entity entity) const;

            //! All data read
            bool hasAllData() const;

            //! Initial setup of leds
            void setLeds(BlackGui::CLedWidget *ledDb, BlackGui::CLedWidget *dataReady,
                         BlackGui::CLedWidget *led_IcaoAircraft, BlackGui::CLedWidget *led_IcaoAirline, BlackGui::CLedWidget *led_Countries,
                         BlackGui::CLedWidget *led_Distributors, BlackGui::CLedWidget *led_Liveries, BlackGui::CLedWidget *led_Models);

            //! Data have been read
            void dataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState readState, int count);

            //! DB or network accessibility changed
            void accessibilityChanged(bool accessible);

            //! Check server status
            void checkServerAndData();

        private:
            QTimer m_timer { this }; //!< check timer
            BlackGui::CLedWidget *led_SwiftDb = nullptr;
            BlackGui::CLedWidget *led_DataReady = nullptr;
            BlackGui::CLedWidget *led_IcaoAircraft = nullptr;
            BlackGui::CLedWidget *led_IcaoAirline = nullptr;
            BlackGui::CLedWidget *led_Countries = nullptr;
            BlackGui::CLedWidget *led_Distributors = nullptr;
            BlackGui::CLedWidget *led_Liveries = nullptr;
            BlackGui::CLedWidget *led_Models = nullptr;
        };

        //! Info bar displaying status of web readers(swift DB, ...)
        class BLACKGUI_EXPORT CInfoBarWebReadersStatusComponent : public CInfoBarWebReadersStatusBase
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CInfoBarWebReadersStatusComponent(QWidget *parent = nullptr);

            //!Destructor
            virtual ~CInfoBarWebReadersStatusComponent();

        private:
            QScopedPointer<Ui::CInfoBarWebReadersStatusComponent> ui;
        };
    } // ns
} // ns
#endif // guard
