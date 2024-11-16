// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_EDITORS_SITUATIONSFORM_H
#define SWIFT_GUI_EDITORS_SITUATIONSFORM_H

#include <QScopedPointer>

#include "gui/editors/form.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/aircraftsituation.h"
#include "misc/statusmessagelist.h"

namespace Ui
{
    class CSituationForm;
}
namespace swift::gui::editors
{
    /*!
     * Selector / entry
     */
    class SWIFT_GUI_EXPORT CSituationForm : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSituationForm(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSituationForm() override;

        //! Set the situation
        void setSituation(const swift::misc::aviation::CAircraftSituation &situation);

        //! Get the situation
        swift::misc::aviation::CAircraftSituation getSituation() const;

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        virtual void setSelectOnly() override;
        virtual swift::misc::CStatusMessageList validate(bool nested = false) const override;
        //! @}

        //! Set button visible
        void showSetButton(bool visible);

    signals:
        //! Aircraft situation to be changed
        void changeAircraftSituation();

    private:
        static constexpr int RoundDigits = 6;

        //! Get bank angle
        swift::misc::physical_quantities::CAngle getBankAngle() const;

        //! Get bank angle
        double getBankAngleDegrees() const;

        //! Get pitch angle
        swift::misc::physical_quantities::CAngle getPitchAngle() const;

        //! Get pitch angle
        double getPitchAngleDegrees() const;

        //! Get heading angle
        swift::misc::physical_quantities::CAngle getHeadingAngle() const;

        //! Get heading angle
        double getHeadingAngleDegrees() const;

        //! Get barometric pressure at MSL (mean sea level)
        swift::misc::physical_quantities::CPressure getBarometricPressureMsl() const;

        //! Get ground speed
        swift::misc::physical_quantities::CSpeed getGroundSpeed() const;

        //! Get pressure at mean sea level
        double getBarometricPressureMslMillibar() const;

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
        void pressureEntered();
        void resetPressure();
        void presetOwnAircraftSituation();
        //! @}

        //! Coordinate has been changed
        void onCoordinateChanged();

        QScopedPointer<Ui::CSituationForm> ui;
    };
} // namespace swift::gui::editors
#endif // guard
