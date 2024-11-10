// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_CALLSIGNCOMPLETER_H
#define BLACKGUI_COMPONENTS_CALLSIGNCOMPLETER_H

#include "blackgui/blackguiexport.h"
#include "blackgui/sharedstringlistcompleter.h"
#include "misc/digestsignal.h"
#include "misc/aviation/callsignset.h"
#include "misc/network/connectionstatus.h"
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
        swift::misc::aviation::CCallsign getCallsign(bool onlyKnownCallsign = true) const;

        //! Prefill wit callsign
        void setCallsign(const swift::misc::aviation::CCallsign &cs);

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
        void onChangedConnectionStatus(const swift::misc::network::CConnectionStatus &from, const swift::misc::network::CConnectionStatus &to);
        bool isValidKnownCallsign(const QString &callsignString) const;

        //! Shared completer data
        static CSharedStringListCompleter *completer();

        QScopedPointer<Ui::CCallsignCompleter> ui;
        swift::misc::CDigestSignal m_dsAircraftsInRangeChanged { this, &CCallsignCompleter::onChangedAircraftInRange, 5000, 5 };
        swift::misc::CDigestSignal m_dsEditingFinished { this, &CCallsignCompleter::editingFinishedDigest, 500, 3 };
        swift::misc::CDigestSignal m_dsValidCallsignEntered { this, &CCallsignCompleter::validCallsignEnteredDigest, 500, 3 };
        QString m_lastValue;

        bool m_addOwnCallsign = false;
        bool m_onlyWithParts = false;
    };
} // ns
#endif // guard
