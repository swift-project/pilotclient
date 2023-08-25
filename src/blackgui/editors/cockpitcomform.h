// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_EDITORS_COCKPITCOMFORM_H
#define BLACKGUI_EDITORS_COCKPITCOMFORM_H

#include "blackgui/editors/form.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/aviation/atcstationlist.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CCockpitComForm;
}
namespace BlackGui::Editors
{
    /*!
     * COM elements
     */
    class BLACKGUI_EXPORT CCockpitComForm : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCockpitComForm(QWidget *parent = nullptr);

        //! Destrutor
        virtual ~CCockpitComForm() override;

        //! COM frequencies displayed
        void setFrequencies(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2);

        //! Set the XPDR values
        void setTransponder(const BlackMisc::Aviation::CTransponder &transponder);

        //! Set selected stations
        void setSelectedAtcStations(const BlackMisc::Aviation::CAtcStationList &selectedStations);

        //! Set to BlackMisc::Aviation::CTransponder::StateIdent
        void setTransponderModeStateIdent();

        //! Set all values
        void setValue(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        //! Get SELCAL
        BlackMisc::Aviation::CSelcal getSelcal() const;

        //! Set SELCAL
        void setSelcal(const BlackMisc::Aviation::CSelcal &selcal);

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        virtual void setSelectOnly() override;
        virtual BlackMisc::CStatusMessageList validate(bool nested = false) const override;
        //! @}

    signals:
        //! \copydoc BlackGui::Components::CTransponderModeSelector::transponderModeChanged
        void transponderModeChanged(BlackMisc::Aviation::CTransponder::TransponderMode newMode);

        //! \copydoc BlackGui::Components::CTransponderModeSelector::transponderStateIdentEnded
        void transponderStateIdentEnded();

        //! Request to test SELCAL
        void testSelcal();

        //! SELCAL value changed
        void changedSelcal(const BlackMisc::Aviation::CSelcal &selcal);

        //! GUI values changed
        void changedCockpitValues(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        //! @{
        //! Request COM text messages
        void requestCom1TextMessage();
        void requestCom2TextMessage();
        //! @}

    private:
        bool m_integratedWithSim = false; //!< Synced with simulator

        //! Init LEDs
        void initLeds();

        //! Cockpit values to aircraft
        BlackMisc::Simulation::CSimulatedAircraft cockpitValuesToAircraftObject();

        //! GUI values have been changed, will trigger CCockpitComForm::guiChangedCockpitValues
        void onGuiChangedCockpitValues();

        //! SELCAL value changed (in selector)
        void onSelcalChanged();

        //! Align the heights
        void alignUiElementsHeight();

        //! Update UI for COM unit
        void updateActiveCOMUnitLEDs(bool integratedWithSim, bool com1T, bool com1R, bool com2T, bool com2R);

        //! Update from simulator context
        void updateIntegratedFlagFromSimulatorContext();

        //! Compare 2 frequencies (consider epsilon)
        static bool isFrequenceEqual(double f1, double f2);

        QScopedPointer<Ui::CCockpitComForm> ui;
    };
} // ns
#endif // guard
