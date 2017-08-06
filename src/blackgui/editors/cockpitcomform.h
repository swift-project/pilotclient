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
#include "blackmisc/simulation/simulatedaircraft.h"
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
        class CCockpitComForm : public CForm
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CCockpitComForm(QWidget *parent = nullptr);

            //! Destrutor
            virtual ~CCockpitComForm();

            //! Change the voice room status
            void changeVoiceRoomStatus(const BlackMisc::Audio::CVoiceRoomList &selectedVoiceRooms);

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

            //! GUI values changed
            void guiChangedCockpitValues(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        private:
            //! Init LEDs
            void initLeds();

            //! Cockpit values to aircraft
            BlackMisc::Simulation::CSimulatedAircraft cockpitValuesToAircraftObject();

            //! COM frequencies displayed
            void updateFrequencyDisplaysFromComSystems(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2);

            //! GUI values have been changed, will trigger CCockpitComForm::guiChangedCockpitValues
            void onGuiChangedCockpitValues();

            QScopedPointer<Ui::CCockpitComForm> ui;
        };
    } // ns
} // ns
#endif // guard
