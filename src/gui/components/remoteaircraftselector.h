// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_REMOTEAIRCRAFTSELECTOR_H
#define SWIFT_GUI_REMOTEAIRCRAFTSELECTOR_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"
#include "misc/aviation/callsign.h"
#include "misc/digestsignal.h"
#include "misc/simulation/simulatedaircraftlist.h"

class QShowEvent;
class QWidget;

namespace swift::misc::simulation
{
    class CSimulatedAircraft;
}
namespace Ui
{
    class CRemoteAircraftSelector;
}
namespace swift::gui::components
{
    //! Select a remote aircraft
    //! \deprecated list gets too long with many aircraft
    //! \remark CCallsignCompleter as a better alternative
    class SWIFT_GUI_EXPORT CRemoteAircraftSelector : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CRemoteAircraftSelector(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CRemoteAircraftSelector() override;

        //! Selected callsign
        swift::misc::aviation::CCallsign getSelectedCallsign() const;

        //! Indicate if aircraft parts enabled aircraft
        void indicatePartsEnabled(bool indicate);

    signals:
        //! Callsign changed
        void changedCallsign();

    protected:
        //! \copydoc QWidget::showEvent
        virtual void showEvent(QShowEvent *event) override;

    private:
        //! Added aircraft, change content of combobox
        void onAddedAircraft(const swift::misc::simulation::CSimulatedAircraft &aircraft);

        //! Removed aircraft, change content of combobox
        void onRemovedAircraft(const swift::misc::aviation::CCallsign &callsign);

        //! Combo box has been changed
        void comboBoxChanged(const QString &text);

        //! Set combobox items
        void fillComboBox();

        QScopedPointer<Ui::CRemoteAircraftSelector> ui;
        QString m_currentText;
        bool m_showPartsEnabled = false;
        swift::misc::simulation::CSimulatedAircraftList m_aircraft;
        swift::misc::CDigestSignal m_dsFillComboBox { this, &CRemoteAircraftSelector::fillComboBox,
                                                      std::chrono::seconds(3), 5 };
    };
} // namespace swift::gui::components

#endif // guard
