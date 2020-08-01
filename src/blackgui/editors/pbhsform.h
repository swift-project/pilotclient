/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_PBHSFORM_H
#define BLACKGUI_EDITORS_PBHSFORM_H

#include "form.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/speed.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CPbhsForm; }
namespace BlackGui
{
    namespace Editors
    {
        //! Pitch, bank, heading and speed form
        class CPbhsForm : public CForm
        {
            Q_OBJECT

        public:
            //! Ctor
            explicit CPbhsForm(QWidget *parent = nullptr);

            //! Dtor
            virtual ~CPbhsForm() override;

            //! Get bank angle
            BlackMisc::PhysicalQuantities::CAngle getBankAngle() const;

            //! Set bank angle
            void setBankAngle(const BlackMisc::PhysicalQuantities::CAngle &angle);

            //! Get pitch angle
            BlackMisc::PhysicalQuantities::CAngle getPitchAngle() const;

            //! Set pitch angle
            void setPitchAngle(const BlackMisc::PhysicalQuantities::CAngle &angle);

            //! Get heading angle
            BlackMisc::PhysicalQuantities::CAngle getHeadingAngle() const;

            //! Get heading
            BlackMisc::Aviation::CHeading getHeading() const;

            //! Set heading angle
            void setHeadingAngle(const BlackMisc::PhysicalQuantities::CAngle &angle);

            //! Get ground speed
            BlackMisc::PhysicalQuantities::CSpeed getGroundSpeed() const;

            //! Set situation
            void setSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! Update PBHs in situation
            void updateSituation(BlackMisc::Aviation::CAircraftSituation &situation);

            //! \copydoc CForm::setReadOnly
            virtual void setReadOnly(bool readOnly) override;

            //! Show the "SET" button
            void showSetButton(bool visible);

        signals:
            //! Changed values
            void changeValues();

        private:
            static constexpr int RoundDigits = 6; //!< rounding

            //! Get bank angle
            double getBankAngleDegrees() const;

            //! Get pitch angle
            double getPitchAngleDegrees() const;

            //! Get heading angle
            double getHeadingAngleDegrees() const;

            //! Values changed from UI
            //! @{
            void bankSliderChanged(int value);
            void pitchSliderChanged(int value);
            void headingSliderChanged(int value);
            void pressureSliderChanged(int value);
            void bankEntered();
            void resetBank();
            void pitchEntered();
            void resetPitch();
            void headingEntered();
            void resetHeading();
            void presetOwnAircraftSituation();
            //! @}

            static int clampAngle(int in) { return qBound(-179, in, 180); }

            QScopedPointer<Ui::CPbhsForm> ui;
        };
    } // ns
} // ns

#endif // guard
