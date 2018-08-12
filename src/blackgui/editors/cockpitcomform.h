/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_COCKPITCOMFORM_H
#define BLACKGUI_EDITORS_COCKPITCOMFORM_H

#include "form.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/audio/voiceroomlist.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CCockpitComForm; }
namespace BlackGui
{
    namespace Editors
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

            //! Change the voice room status
            void setVoiceRoomStatus(const BlackMisc::Audio::CVoiceRoomList &selectedVoiceRooms);

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

        private:
            //! Init LEDs
            void initLeds();

            //! Cockpit values to aircraft
            BlackMisc::Simulation::CSimulatedAircraft cockpitValuesToAircraftObject();

            //! GUI values have been changed, will trigger CCockpitComForm::guiChangedCockpitValues
            void onGuiChangedCockpitValues();

            //! SELCAL value changed (in selector)
            void onSelcalChanged();

            //! Compare 2 frequencies (consider epsilon)
            static bool isFrequenceEqual(double f1, double f2);

            QScopedPointer<Ui::CCockpitComForm> ui;
        };
    } // ns
} // ns
#endif // guard
