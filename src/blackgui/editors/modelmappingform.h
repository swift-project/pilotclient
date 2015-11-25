/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_MODELMAPPINGFORM_H
#define BLACKGUI_EDITORS_MODELMAPPINGFORM_H

#include "blackgui/editors/form.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include <QScopedPointer>

namespace Ui { class CModelMappingForm; }

namespace BlackGui
{
    namespace Editors
    {
        /*!
         * Model mapping form
         */
        class CModelMappingForm : public CForm
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CModelMappingForm(QWidget *parent = nullptr);

            //! Destructor
            ~CModelMappingForm();

            //! Value
            BlackMisc::Simulation::CAircraftModel getValue() const;

            //! Validate
            BlackMisc::CStatusMessageList validate(bool withNestedForms) const;

            //! \copydoc CEditor::setReadOnly
            virtual void setReadOnly(bool readOnly) override;

        public slots:
            //! Set model
            void setValue(BlackMisc::Simulation::CAircraftModel &model);

        signals:
            //! Request validation
            void requestValidation();

            //! Save
            void requestSave();

        private:
            QScopedPointer<Ui::CModelMappingForm> ui;
            BlackMisc::Simulation::CAircraftModel m_originalModel;
        };

    } // ns
} // ns

#endif // guard
