// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_REMOTEAIRCRAFTSELECTOR_H
#define BLACKGUI_REMOTEAIRCRAFTSELECTOR_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/digestsignal.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

class QShowEvent;
class QWidget;

namespace BlackMisc::Simulation
{
    class CSimulatedAircraft;
}
namespace Ui
{
    class CRemoteAircraftSelector;
}
namespace BlackGui::Components
{
    //! Select a remote aircraft
    //! \deprecated list gets too long with many aircraft
    //! \remark CCallsignCompleter as a better alternative
    class BLACKGUI_EXPORT CRemoteAircraftSelector : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CRemoteAircraftSelector(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CRemoteAircraftSelector() override;

        //! Selected callsign
        BlackMisc::Aviation::CCallsign getSelectedCallsign() const;

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
        void onAddedAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        //! Removed aircraft, change content of combobox
        void onRemovedAircraft(const BlackMisc::Aviation::CCallsign &callsign);

        //! Combo box has been changed
        void comboBoxChanged(const QString &text);

        //! Set combobox items
        void fillComboBox();

        QScopedPointer<Ui::CRemoteAircraftSelector> ui;
        QString m_currentText;
        bool m_showPartsEnabled = false;
        BlackMisc::Simulation::CSimulatedAircraftList m_aircraft;
        BlackMisc::CDigestSignal m_dsFillComboBox { this, &CRemoteAircraftSelector::fillComboBox, 3000, 5 };
    };
} // namespace

#endif // guard
