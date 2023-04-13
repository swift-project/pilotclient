/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_CALLSIGNCOMPLETER_H
#define BLACKGUI_COMPONENTS_CALLSIGNCOMPLETER_H

#include "blackgui/blackguiexport.h"
#include "blackgui/sharedstringlistcompleter.h"
#include "blackmisc/digestsignal.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/network/connectionstatus.h"
#include <QFrame>
#include <QScopedPointer>
#include <QTimer>
#include <QCompleter>

namespace Ui
{
    class CCallsignCompleter;
}
namespace BlackGui::Components
{
    /*!
     * Completer for aircraft callsigns
     */
    class BLACKGUI_EXPORT CCallsignCompleter : public QFrame
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CCallsignCompleter(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CCallsignCompleter() override;

        //! Get the entered callsign
        BlackMisc::Aviation::CCallsign getCallsign(bool onlyKnownCallsign = true) const;

        //! Prefill wit callsign
        void setCallsign(const BlackMisc::Aviation::CCallsign &cs);

        //! String as entered
        QString getRawCallsignString() const;

        //! Is valid callsign?
        bool hasValidCallsign() const;

        //! Set read only
        void setReadOnly(bool readOnly);

        //! Add own callsign
        void addOwnCallsign(bool add) { m_addOwnCallsign = add; }

        //! Only with parts
        void onlyWithParts(bool partsOnly) { m_onlyWithParts = partsOnly; }

    signals:
        //! Changed callsign entered
        void validChangedCallsignEntered();

        //! Changed callsign entered
        void validCallsignEnteredDigest();

        //! Editing finished
        void editingFinishedDigest();

    private:
        void updateCallsignsFromContext();
        void onEditingFinished();
        void onChangedAircraftInRange();
        void onChangedConnectionStatus(const BlackMisc::Network::CConnectionStatus &from, const BlackMisc::Network::CConnectionStatus &to);
        bool isValidKnownCallsign(const QString &callsignString) const;

        //! Shared completer data
        static CSharedStringListCompleter *completer();

        QScopedPointer<Ui::CCallsignCompleter> ui;
        BlackMisc::CDigestSignal m_dsAircraftsInRangeChanged { this, &CCallsignCompleter::onChangedAircraftInRange, 5000, 5 };
        BlackMisc::CDigestSignal m_dsEditingFinished { this, &CCallsignCompleter::editingFinishedDigest, 500, 3 };
        BlackMisc::CDigestSignal m_dsValidCallsignEntered { this, &CCallsignCompleter::validCallsignEnteredDigest, 500, 3 };
        QString m_lastValue;

        bool m_addOwnCallsign = false;
        bool m_onlyWithParts = false;
    };
} // ns
#endif // guard
