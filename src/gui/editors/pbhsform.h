// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_EDITORS_PBHSFORM_H
#define SWIFT_GUI_EDITORS_PBHSFORM_H

#include <QFrame>
#include <QScopedPointer>

#include "gui/editors/form.h"
#include "misc/aviation/aircraftsituation.h"
#include "misc/aviation/heading.h"
#include "misc/pq/angle.h"
#include "misc/pq/speed.h"

namespace Ui
{
    class CPbhsForm;
}
namespace swift::gui::editors
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
        swift::misc::physical_quantities::CAngle getBankAngle() const;

        //! Set bank angle
        void setBankAngle(const swift::misc::physical_quantities::CAngle &angle);

        //! Get pitch angle
        swift::misc::physical_quantities::CAngle getPitchAngle() const;

        //! Set pitch angle
        void setPitchAngle(const swift::misc::physical_quantities::CAngle &angle);

        //! Get heading angle
        swift::misc::physical_quantities::CAngle getHeadingAngle() const;

        //! Get heading
        swift::misc::aviation::CHeading getHeading() const;

        //! Set heading angle
        void setHeadingAngle(const swift::misc::physical_quantities::CAngle &angle);

        //! Get ground speed
        swift::misc::physical_quantities::CSpeed getGroundSpeed() const;

        //! Set situation
        void setSituation(const swift::misc::aviation::CAircraftSituation &situation);

        //! Update PBHs in situation
        void updateSituation(swift::misc::aviation::CAircraftSituation &situation);

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

        //! @{
        //! Values changed from UI
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
} // namespace swift::gui::editors

#endif // SWIFT_GUI_EDITORS_PBHSFORM_H
