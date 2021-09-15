/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBMODELMAPPINGMODIFYDIALOG_H
#define BLACKGUI_COMPONENTS_DBMODELMAPPINGMODIFYDIALOG_H

#include "blackgui/blackguiexport.h"
#include "blackgui/components/dbmappingcomponentaware.h"
#include "blackmisc/propertyindexvariantmap.h"

#include <QDialog>
#include <QObject>
#include <QScopedPointer>

namespace BlackMisc::Simulation { class CAircraftModel; }
namespace Ui { class CDbModelMappingModifyDialog; }
namespace BlackGui::Components
{
    /*!
     * Modify model fields as dialog
     */
    class BLACKGUI_EXPORT CDbModelMappingModifyDialog :
        public QDialog,
        public CDbMappingComponentAware
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDbModelMappingModifyDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDbModelMappingModifyDialog();

        //! Get the values
        BlackMisc::CPropertyIndexVariantMap getValues() const;

        //! Set default values
        void setValue(const BlackMisc::Simulation::CAircraftModel &model);

    private:
        QScopedPointer<Ui::CDbModelMappingModifyDialog> ui;
    };
} // ns
#endif // guard
