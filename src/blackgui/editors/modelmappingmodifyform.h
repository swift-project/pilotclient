/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_MODELMAPPINGMODIFYFORM_H
#define BLACKGUI_EDITORS_MODELMAPPINGMODIFYFORM_H

#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include <QFrame>
#include <QScopedPointer>
#include <QCheckBox>
#include <QWidget>

namespace Ui { class CModelMappingModifyForm; }

namespace BlackGui
{
    namespace Editors
    {
        /*!
         * Allows to modify individual fields of the model form
         */
        class CModelMappingModifyForm : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CModelMappingModifyForm(QWidget *parent = nullptr);

            //! Destructor
            ~CModelMappingModifyForm();

            //! Get the values
            BlackMisc::CPropertyIndexVariantMap getValues() const;

            //! Set value
            void setValue(const BlackMisc::Simulation::CAircraftModel &model);

        private slots:
            //! Return pressed
            void ps_returnPressed();

            //! Data changed
            void ps_changed();

        private:
            //! Checkbox from its corresponding widgte
            QCheckBox *widgetToCheckbox(QObject *widget) const;

            QScopedPointer<Ui::CModelMappingModifyForm> ui;
        };
    } // ns
} // ns

#endif // guard
