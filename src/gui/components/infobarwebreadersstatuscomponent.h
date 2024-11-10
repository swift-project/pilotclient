// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_INFOBARWEBREADERSSTATUSCOMPONENT_H
#define SWIFT_GUI_INFOBARWEBREADERSSTATUSCOMPONENT_H

#include "gui/swiftguiexport.h"
#include "gui/led.h"
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
namespace swift::gui
{
    class CLedWidget;
    namespace components
    {
        //! Info bar displaying status of web readers(swift DB, ...)
        class SWIFT_GUI_EXPORT CInfoBarWebReadersStatusBase : public QFrame
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
            void setLeds(swift::gui::CLedWidget *ledDataReady, CLedWidget *ledConsolidation,
                         swift::gui::CLedWidget *ledIcaoAircraft, swift::gui::CLedWidget *ledIcaoAirline, swift::gui::CLedWidget *ledCountries,
                         swift::gui::CLedWidget *ledDistributors, swift::gui::CLedWidget *ledLiveries, swift::gui::CLedWidget *ledModels);

            //! Data have been read
            void dataRead(swift::misc::network::CEntityFlags::Entity entities, swift::misc::network::CEntityFlags::ReadState readState, int count);

            //! Check data status
            void checkData();

            //! Show the consolidation status
            virtual void showConsolidationStatus(bool show);

        private:
            QTimer m_timer; //!< check timer
            swift::gui::CLedWidget *m_ledDataReady = nullptr;
            swift::gui::CLedWidget *m_ledConsolidation = nullptr;
            swift::gui::CLedWidget *m_ledIcaoAircraft = nullptr;
            swift::gui::CLedWidget *m_ledIcaoAirline = nullptr;
            swift::gui::CLedWidget *m_ledCountries = nullptr;
            swift::gui::CLedWidget *m_ledDistributors = nullptr;
            swift::gui::CLedWidget *m_ledLiveries = nullptr;
            swift::gui::CLedWidget *m_ledModels = nullptr;
        };

        //! Info bar displaying status of web readers(swift DB, ...)
        class SWIFT_GUI_EXPORT CInfoBarWebReadersStatusComponent : public CInfoBarWebReadersStatusBase
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
