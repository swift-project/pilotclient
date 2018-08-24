/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_SITUATIONSFORM_H
#define BLACKGUI_EDITORS_SITUATIONSFORM_H

#include "blackgui/blackguiexport.h"
#include "blackgui/editors/form.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/statusmessagelist.h"
#include <QScopedPointer>

class QWidget;

namespace Ui { class CSituationForm; }
namespace BlackGui
{
    namespace Editors
    {
        /**
         * Selector / entry
         */
        class BLACKGUI_EXPORT CSituationForm : public CForm
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CSituationForm(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CSituationForm() override;

            //! Set the situation
            void setSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! Get the situation
            BlackMisc::Aviation::CAircraftSituation getSituation() const;

            //! \name Form class implementations
            //! @{
            virtual void setReadOnly(bool readonly) override;
            virtual void setSelectOnly() override;
            virtual BlackMisc::CStatusMessageList validate(bool nested = false) const override;
            //! @}

            //! Set button visible
            void showSetButton(bool visible);

        signals:
            //! Aircraft situation to be changed
            void changeAircraftSituation();

        private:
            static constexpr int RoundDigits = 6;

            //! Get bank angle
            BlackMisc::PhysicalQuantities::CAngle getBankAngle() const;

            //! Get bank angle
            double getBankAngleDegrees() const;

            //! Get pitch angle
            BlackMisc::PhysicalQuantities::CAngle getPitchAngle() const;

            //! Get pitch angle
            double getPitchAngleDegrees() const;

            //! Get barometric pressure at MSL (mean sea level)
            BlackMisc::PhysicalQuantities::CPressure getBarometricPressureMsl() const;

            //! Get pressure at mean sea level
            double getBarometricPressureMslMillibar() const;

            void bankSliderChanged(int value);
            void pitchSliderChanged(int value);
            void pressureSliderChanged(int value);
            void bankEntered();
            void resetBank();
            void pitchEntered();
            void resetPitch();
            void pressureEntered();
            void resetPressure();

            QScopedPointer<Ui::CSituationForm> ui;
        };
    } // ns
} // ns
#endif // guard
