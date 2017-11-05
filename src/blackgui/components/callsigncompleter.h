/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_CALLSIGNCOMPLETER_H
#define BLACKGUI_COMPONENTS_CALLSIGNCOMPLETER_H

#include "blackgui/blackguiexport.h"
#include "blackgui/sharedstringlistcompleter.h"
#include "blackmisc/digestsignal.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackcore/network.h"
#include <QFrame>
#include <QScopedPointer>
#include <QTimer>
#include <QCompleter>

namespace Ui { class CCallsignCompleter; }
namespace BlackGui
{
    namespace Components
    {
        /**
         * Completer for aircraft callsigns
         */
        class BLACKGUI_EXPORT CCallsignCompleter : public QFrame
        {
            Q_OBJECT

        public:
            //! Ctor
            explicit CCallsignCompleter(QWidget *parent = nullptr);

            //! Dtor
            virtual ~CCallsignCompleter();

            //! Get the entered callsign
            BlackMisc::Aviation::CCallsign getCallsign() const;

            //! String as entered
            QString getRawCallsignString() const;

            //! Is valid callsign?
            bool hasValidCallsign() const;

        signals:
            //! Callsign entered
            void validCallsignEntered();

            //! Editing finished
            void editingFinished();

        private:
            void updateCallsignsFromContext();
            void onEditingFinished();
            void onChangedAircraftInRange();
            void onChangedConnectionStatus(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to);
            bool isValidKnownCallsign(const QString &callsignString) const;

            //! Shared completer data
            static CSharedStringListCompleter *completer();

            QScopedPointer <Ui::CCallsignCompleter> ui;
            BlackMisc::CDigestSignal m_dsAircraftsInRangeChanged { this, &CCallsignCompleter::onChangedAircraftInRange, 5000, 5 };
            QString m_lastValue;
        };
    } // ns
} // ns
#endif // guard
