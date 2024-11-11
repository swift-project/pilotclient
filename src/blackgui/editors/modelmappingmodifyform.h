// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_EDITORS_MODELMAPPINGMODIFYFORM_H
#define BLACKGUI_EDITORS_MODELMAPPINGMODIFYFORM_H

#include "blackgui/editors/form.h"
#include "blackgui/blackguiexport.h"
#include "misc/propertyindexvariantmap.h"
#include <QFrame>
#include <QObject>
#include <QScopedPointer>

class QCheckBox;

namespace Ui
{
    class CModelMappingModifyForm;
}
namespace swift::misc::simulation
{
    class CAircraftModel;
}
namespace BlackGui::Editors
{
    /*!
     * Allows to modify individual fields of the model form
     */
    class BLACKGUI_EXPORT CModelMappingModifyForm : public CFormDbUser
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CModelMappingModifyForm(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CModelMappingModifyForm() override;

        //! Get the values
        swift::misc::CPropertyIndexVariantMap getValues() const;

        //! Set value
        void setValue(const swift::misc::simulation::CAircraftModel &model);

        //! \copydoc CForm::setReadOnly
        virtual void setReadOnly(bool readOnly) override;

    protected:
        //! \copydoc CFormDbUser::userChanged
        virtual void userChanged() override;

    private:
        //! Return pressed
        void returnPressed();

        //! Data changed
        void changed();

        //! Checkbox from its corresponding widgte
        QCheckBox *widgetToCheckbox(QObject *widget) const;

        //! CG changed
        void onCGEdited();

        QScopedPointer<Ui::CModelMappingModifyForm> ui;
    };
} // ns

#endif // guard
