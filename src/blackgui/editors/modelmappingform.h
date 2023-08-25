// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_EDITORS_MODELMAPPINGFORM_H
#define BLACKGUI_EDITORS_MODELMAPPINGFORM_H

#include "blackgui/editors/form.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/statusmessagelist.h"

#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CModelMappingForm;
}
namespace BlackGui::Editors
{
    /*!
     * Model mapping form
     */
    class BLACKGUI_EXPORT CModelMappingForm : public CFormDbUser
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CModelMappingForm(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CModelMappingForm() override;

        //! Value
        BlackMisc::Simulation::CAircraftModel getValue() const;

        //! Validate

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        virtual void setSelectOnly() override;
        virtual BlackMisc::CStatusMessageList validate(bool withNestedObjects) const override;
        //! @}

        //! Set model
        void setValue(BlackMisc::Simulation::CAircraftModel &model);

    signals:
        //! Request stashing for model
        void requestStash();

    protected:
        //! \copydoc CFormDbUser::userChanged
        virtual void userChanged() override;

    private:
        //! CG (aka vertical offset) from UI
        BlackMisc::PhysicalQuantities::CLength getCGFromUI() const;

        //! Set CG value properly formatted
        void setCGtoUI(const BlackMisc::PhysicalQuantities::CLength &cg);

        //! CG edited
        void onCgEditFinished();

        QScopedPointer<Ui::CModelMappingForm> ui;
        BlackMisc::Simulation::CAircraftModel m_originalModel;
    };
} // ns

#endif // guard
