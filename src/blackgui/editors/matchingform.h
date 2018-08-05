/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_MATCHINGFORM_H
#define BLACKGUI_EDITORS_MATCHINGFORM_H

#include "form.h"
#include "blackmisc/simulation/aircraftmatchersetup.h"
#include <QScopedPointer>

namespace Ui { class CMatchingForm; }
namespace BlackGui
{
    namespace Editors
    {
        //! Matching form
        class CMatchingForm : public CForm
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CMatchingForm(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CMatchingForm() override;

            //! \name Form class implementations
            //! @{
            virtual void setReadOnly(bool readonly) override;
            virtual BlackMisc::CStatusMessageList validate(bool withNestedForms) const override;
            //! @}

            //! Set valued
            void setValue(const BlackMisc::Simulation::CAircraftMatcherSetup &setup);

            //! Value
            BlackMisc::Simulation::CAircraftMatcherSetup value() const;

            //! Clear data
            void clear();

        private:
            //! Mode
            BlackMisc::Simulation::CAircraftMatcherSetup::MatchingMode matchingMode() const;

            QScopedPointer<Ui::CMatchingForm> ui;
        };
    } // ns
} // ns


#endif // guard
