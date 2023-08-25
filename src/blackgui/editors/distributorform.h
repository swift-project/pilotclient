// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_EDITORS_DISTRIBUTORFORM_H
#define BLACKGUI_EDITORS_DISTRIBUTORFORM_H

#include "blackgui/blackguiexport.h"
#include "blackgui/editors/form.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/variant.h"

#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CDistributorForm;
}
namespace BlackGui::Editors
{
    /*!
     * Distributor form
     */
    class BLACKGUI_EXPORT CDistributorForm : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDistributorForm(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDistributorForm() override;

        //! Get value
        BlackMisc::Simulation::CDistributor getValue() const;

        //! Allow to drop
        void allowDrop(bool allowDrop);

        //! Is drop allowed?
        bool isDropAllowed() const;

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        virtual void setSelectOnly() override;
        virtual BlackMisc::CStatusMessageList validate(bool nested = false) const override;
        //! @}

        //! Clear
        void clear();

        //! Set value
        bool setValue(const BlackMisc::Simulation::CDistributor &distributor = BlackMisc::Simulation::CDistributor());

    protected:
        //! \copydoc CForm::jsonPasted
        virtual void jsonPasted(const QString &json) override;

    private:
        //! Variant has been dropped
        void onDroppedCode(const BlackMisc::CVariant &variantDropped);

        //! Any UI values in the details fields
        bool hasAnyUiDetailsValues() const;

        QScopedPointer<Ui::CDistributorForm> ui;
        BlackMisc::Simulation::CDistributor m_currentDistributor;
    };
} // ns

#endif // guard
