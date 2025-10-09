// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_EDITORS_MODELMAPPINGFORM_H
#define SWIFT_GUI_EDITORS_MODELMAPPINGFORM_H

#include <QObject>
#include <QScopedPointer>

#include "gui/editors/form.h"
#include "gui/swiftguiexport.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/statusmessagelist.h"

namespace Ui
{
    class CModelMappingForm;
}
namespace swift::gui::editors
{
    /*!
     * Model mapping form
     */
    class SWIFT_GUI_EXPORT CModelMappingForm : public CFormDbUser
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CModelMappingForm(QWidget *parent = nullptr);

        //! Destructor
        ~CModelMappingForm() override;

        //! Value
        swift::misc::simulation::CAircraftModel getValue() const;

        //! Validate

        //! \name Form class implementations
        //! @{
        void setReadOnly(bool readonly) override;
        void setSelectOnly() override;
        swift::misc::CStatusMessageList validate(bool withNestedObjects) const override;
        //! @}

        //! Set model
        void setValue(swift::misc::simulation::CAircraftModel &model);

    signals:
        //! Request stashing for model
        void requestStash();

    protected:
        //! \copydoc CFormDbUser::userChanged
        void userChanged() override;

    private:
        //! CG (aka vertical offset) from UI
        swift::misc::physical_quantities::CLength getCGFromUI() const;

        //! Set CG value properly formatted
        void setCGtoUI(const swift::misc::physical_quantities::CLength &cg);

        //! CG edited
        void onCgEditFinished();

        QScopedPointer<Ui::CModelMappingForm> ui;
        swift::misc::simulation::CAircraftModel m_originalModel;
    };
} // namespace swift::gui::editors

#endif // SWIFT_GUI_EDITORS_MODELMAPPINGFORM_H
