// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_EDITORS_INTERPOLATIONSETUPFORM_H
#define BLACKGUI_EDITORS_INTERPOLATIONSETUPFORM_H

#include "blackgui/editors/form.h"
#include "misc/simulation/interpolation/interpolationrenderingsetup.h"
#include "misc/pq/angle.h"
#include "misc/statusmessagelist.h"

#include <QScopedPointer>
#include <QCheckBox>

namespace Ui
{
    class CInterpolationSetupForm;
}
namespace BlackGui::Editors
{
    //! Setup of interpolation and rendering
    class CInterpolationSetupForm : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CInterpolationSetupForm(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CInterpolationSetupForm() override;

        //! Set value
        void setValue(const swift::misc::simulation::CInterpolationAndRenderingSetupBase &setup);

        //! Get value
        swift::misc::simulation::CInterpolationAndRenderingSetupPerCallsign getValue() const;

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        virtual swift::misc::CStatusMessageList validate(bool nested = false) const override;
        //! @}

    signals:
        //! Value changed
        void valueChanged();

    private:
        QScopedPointer<Ui::CInterpolationSetupForm> ui;
        QList<QCheckBox *> m_allCheckBoxes;

        //! Checkbox has been changed
        void onCheckboxChanged(int state);

        //! Interpolator mode
        void onInterpolatorModeChanged(bool checked);

        //! Pitch entry
        void onPitchChanged();

        //! Pitch on ground
        swift::misc::physical_quantities::CAngle getPitchOnGround() const;

        //! Display pitch on ground
        void displayPitchOnGround(const swift::misc::physical_quantities::CAngle &pitchOnGround);

        //! Mode from UI
        swift::misc::simulation::CInterpolationAndRenderingSetupBase::InterpolatorMode getInterpolatorMode() const;

        //! Set mode
        void setInterpolatorMode(swift::misc::simulation::CInterpolationAndRenderingSetupBase::InterpolatorMode mode);
    };
} // ns
#endif // guard
