// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBMODELMAPPINGMODIFYDIALOG_H
#define SWIFT_GUI_COMPONENTS_DBMODELMAPPINGMODIFYDIALOG_H

#include "gui/swiftguiexport.h"
#include "gui/components/dbmappingcomponentaware.h"
#include "misc/propertyindexvariantmap.h"

#include <QDialog>
#include <QObject>
#include <QScopedPointer>

namespace swift::misc::simulation
{
    class CAircraftModel;
}
namespace Ui
{
    class CDbModelMappingModifyDialog;
}
namespace swift::gui::components
{
    /*!
     * Modify model fields as dialog
     */
    class SWIFT_GUI_EXPORT CDbModelMappingModifyDialog :
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
        swift::misc::CPropertyIndexVariantMap getValues() const;

        //! Set default values
        void setValue(const swift::misc::simulation::CAircraftModel &model);

    private:
        QScopedPointer<Ui::CDbModelMappingModifyDialog> ui;
    };
} // ns
#endif // guard
