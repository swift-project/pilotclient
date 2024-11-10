// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_EDITORS_COCKPITCOMFORM_H
#define SWIFT_GUI_EDITORS_COCKPITCOMFORM_H

#include "gui/editors/form.h"
#include "gui/swiftguiexport.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/aviation/atcstationlist.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CCockpitComForm;
}
namespace swift::gui::editors
{
    /*!
     * COM elements
     */
    class SWIFT_GUI_EXPORT CCockpitComForm : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCockpitComForm(QWidget *parent = nullptr);

        //! Destrutor
        virtual ~CCockpitComForm() override;

        //! COM frequencies displayed
        void setFrequencies(const swift::misc::aviation::CComSystem &com1, const swift::misc::aviation::CComSystem &com2);

        //! Set the XPDR values
        void setTransponder(const swift::misc::aviation::CTransponder &transponder);

        //! Set to swift::misc::aviation::CTransponder::StateIdent
        void setTransponderModeStateIdent();

        //! Set all values
        void setValue(const swift::misc::simulation::CSimulatedAircraft &aircraft);

        //! Get SELCAL
        swift::misc::aviation::CSelcal getSelcal() const;

        //! Set SELCAL
        void setSelcal(const swift::misc::aviation::CSelcal &selcal);

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        virtual void setSelectOnly() override;
        virtual swift::misc::CStatusMessageList validate(bool nested = false) const override;
        //! @}

    signals:
        //! \copydoc swift::gui::components::CTransponderModeSelector::transponderModeChanged
        void transponderModeChanged(swift::misc::aviation::CTransponder::TransponderMode newMode);

        //! \copydoc swift::gui::components::CTransponderModeSelector::transponderStateIdentEnded
        void transponderStateIdentEnded();

        //! Request to test SELCAL
        void testSelcal();

        //! SELCAL value changed
        void changedSelcal(const swift::misc::aviation::CSelcal &selcal);

        //! GUI values changed
        void changedCockpitValues(const swift::misc::simulation::CSimulatedAircraft &aircraft);

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
        swift::misc::simulation::CSimulatedAircraft cockpitValuesToAircraftObject();

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
