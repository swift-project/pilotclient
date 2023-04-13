/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_INFOBARWEBREADERSSTATUSCOMPONENT_H
#define BLACKGUI_INFOBARWEBREADERSSTATUSCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/led.h"
#include "blackmisc/network/url.h"
#include "blackmisc/network/entityflags.h"

#include <QFrame>
#include <QList>
#include <QObject>
#include <QScopedPointer>
#include <QTimer>

namespace Ui
{
    class CInfoBarWebReadersStatusComponent;
}
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
            //! Destructor
            virtual ~CInfoBarWebReadersStatusBase();

            //! Consolidation is running
            virtual void consolidationRunning(bool running);

        protected:
            //! Constructor
            explicit CInfoBarWebReadersStatusBase(QWidget *parent = nullptr);

            //! Init the LEDs
            void initLeds();

            //! Init
            void init();

            //! Set LED states
            void setLedReadStates(const QList<CLedWidget *> &leds, BlackMisc::Network::CEntityFlags::ReadState readState);

            //! Set the LED read state
            void setLedReadState(CLedWidget *led, BlackMisc::Network::CEntityFlags::ReadState readState);

            //! Maps entity to its id
            QList<CLedWidget *> entitiesToLeds(BlackMisc::Network::CEntityFlags::Entity entities) const;

            //! All data read
            bool hasAllData() const;

            //! Initial setup of leds
            void setLeds(BlackGui::CLedWidget *ledDb, BlackGui::CLedWidget *ledDataReady, CLedWidget *ledConsolidation,
                         BlackGui::CLedWidget *ledIcaoAircraft, BlackGui::CLedWidget *ledIcaoAirline, BlackGui::CLedWidget *ledCountries,
                         BlackGui::CLedWidget *ledDistributors, BlackGui::CLedWidget *ledLiveries, BlackGui::CLedWidget *ledModels);

            //! Data have been read
            void dataRead(BlackMisc::Network::CEntityFlags::Entity entities, BlackMisc::Network::CEntityFlags::ReadState readState, int count);

            //! Network accessibility changed
            void networkAccessibilityChanged(bool accessible);

            //! DB accessibility changed
            void dbAccessibilityChanged(bool accessible, const BlackMisc::Network::CUrl &testedUrl);

            //! Check server status
            void checkServerAndData();

            //! Show the consolidation status
            virtual void showConsolidationStatus(bool show);

        private:
            QTimer m_timer; //!< check timer
            BlackGui::CLedWidget *m_ledSwiftDb = nullptr;
            BlackGui::CLedWidget *m_ledDataReady = nullptr;
            BlackGui::CLedWidget *m_ledConsolidation = nullptr;
            BlackGui::CLedWidget *m_ledIcaoAircraft = nullptr;
            BlackGui::CLedWidget *m_ledIcaoAirline = nullptr;
            BlackGui::CLedWidget *m_ledCountries = nullptr;
            BlackGui::CLedWidget *m_ledDistributors = nullptr;
            BlackGui::CLedWidget *m_ledLiveries = nullptr;
            BlackGui::CLedWidget *m_ledModels = nullptr;
        };

        //! Info bar displaying status of web readers(swift DB, ...)
        class BLACKGUI_EXPORT CInfoBarWebReadersStatusComponent : public CInfoBarWebReadersStatusBase
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CInfoBarWebReadersStatusComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CInfoBarWebReadersStatusComponent() override;

            //! Show the consolidation status
            virtual void showConsolidationStatus(bool show) override;

        private:
            QScopedPointer<Ui::CInfoBarWebReadersStatusComponent> ui;
        };
    } // ns
} // ns
#endif // guard
