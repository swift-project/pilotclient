/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
