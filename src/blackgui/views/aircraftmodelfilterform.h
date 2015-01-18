/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AIRCRAFTMODELFILTERFORM_H
#define BLACKGUI_AIRCRAFTMODELFILTERFORM_H

#include "filterdialog.h"
#include "blackgui/models/aircraftmodelfilter.h"
#include <QDialog>
#include <QScopedPointer>
#include <iostream>
#include <memory>

namespace Ui { class CAircraftModelFilterForm; }

namespace BlackGui
{
    namespace Views
    {
        //! Form for a aircraft model filter
        class CAircraftModelFilterForm : public CFilterDialog
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAircraftModelFilterForm(QWidget *parent = nullptr);

            //! Destructor
            ~CAircraftModelFilterForm();

            //! Get created filter
            std::unique_ptr<BlackGui::Models::CAircraftModelFilter> getFilter() const;

        private:
            QScopedPointer<Ui::CAircraftModelFilterForm> ui;
        };

    } // namespace
} // namespace

#endif // guard
