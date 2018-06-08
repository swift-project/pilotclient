/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_INTERPOLATIONSETUPFORM_H
#define BLACKGUI_EDITORS_INTERPOLATIONSETUPFORM_H

#include "form.h"
#include "blackmisc/simulation/interpolationrenderingsetup.h"
#include "blackmisc/statusmessagelist.h"

#include <QScopedPointer>
#include <QCheckBox>

namespace Ui { class CInterpolationSetupForm; }
namespace BlackGui
{
    namespace Editors
    {
        //! Setup of interpolation and rendering
        class CInterpolationSetupForm : public CForm
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CInterpolationSetupForm(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CInterpolationSetupForm();

            //! Set value
            void setValue(const BlackMisc::Simulation::CInterpolationAndRenderingSetupBase &setup);

            //! Get value
            BlackMisc::Simulation::CInterpolationAndRenderingSetupPerCallsign getValue() const;

            //! \name Form class implementations
            //! @{
            virtual void setReadOnly(bool readonly) override;
            virtual BlackMisc::CStatusMessageList validate(bool nested = false) const override;
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

            //! Mode from UI
            BlackMisc::Simulation::CInterpolationAndRenderingSetupBase::InterpolatorMode getInterpolatorMode() const;

            //! Set mode
            void setInterpolatorMode(BlackMisc::Simulation::CInterpolationAndRenderingSetupBase::InterpolatorMode mode);
        };
    } // ns
} // ns
#endif // guard
