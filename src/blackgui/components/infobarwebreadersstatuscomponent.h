// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_INFOBARWEBREADERSSTATUSCOMPONENT_H
#define BLACKGUI_INFOBARWEBREADERSSTATUSCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/led.h"
#include "misc/network/url.h"
#include "misc/network/entityflags.h"

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
            void setLedReadStates(const QList<CLedWidget *> &leds, swift::misc::network::CEntityFlags::ReadState readState);

            //! Set the LED read state
            void setLedReadState(CLedWidget *led, swift::misc::network::CEntityFlags::ReadState readState);

            //! Maps entity to its id
            QList<CLedWidget *> entitiesToLeds(swift::misc::network::CEntityFlags::Entity entities) const;

            //! All data read
            bool hasAllData() const;

            //! Initial setup of leds
            void setLeds(BlackGui::CLedWidget *ledDataReady, CLedWidget *ledConsolidation,
                         BlackGui::CLedWidget *ledIcaoAircraft, BlackGui::CLedWidget *ledIcaoAirline, BlackGui::CLedWidget *ledCountries,
                         BlackGui::CLedWidget *ledDistributors, BlackGui::CLedWidget *ledLiveries, BlackGui::CLedWidget *ledModels);

            //! Data have been read
            void dataRead(swift::misc::network::CEntityFlags::Entity entities, swift::misc::network::CEntityFlags::ReadState readState, int count);

            //! Check data status
            void checkData();

            //! Show the consolidation status
            virtual void showConsolidationStatus(bool show);

        private:
            QTimer m_timer; //!< check timer
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
