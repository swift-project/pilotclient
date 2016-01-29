/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBMODELMAPPINGMODIFYCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBMODELMAPPINGMODIFYCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "dbmappingcomponentaware.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui { class CDbModelMappingModifyComponent; }

namespace BlackGui
{
    namespace Components
    {
        /*!
         * Modify model fields as dialog
         */
        class BLACKGUI_EXPORT CDbModelMappingModifyComponent :
            public QDialog,
            public CDbMappingComponentAware
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbModelMappingModifyComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CDbModelMappingModifyComponent();

            //! Get the values
            BlackMisc::CPropertyIndexVariantMap getValues() const;

            //! Set default values
            void setValue(const BlackMisc::Simulation::CAircraftModel &model);

        private:
            QScopedPointer<Ui::CDbModelMappingModifyComponent> ui;
        };
    } // ns
} // ns
#endif // guard
