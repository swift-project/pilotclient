// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_EDITORS_DISTRIBUTORFORM_H
#define BLACKGUI_EDITORS_DISTRIBUTORFORM_H

#include "blackgui/blackguiexport.h"
#include "blackgui/editors/form.h"
#include "misc/simulation/distributor.h"
#include "misc/statusmessagelist.h"
#include "misc/variant.h"

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
        swift::misc::simulation::CDistributor getValue() const;

        //! Allow to drop
        void allowDrop(bool allowDrop);

        //! Is drop allowed?
        bool isDropAllowed() const;

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        virtual void setSelectOnly() override;
        virtual swift::misc::CStatusMessageList validate(bool nested = false) const override;
        //! @}

        //! Clear
        void clear();

        //! Set value
        bool setValue(const swift::misc::simulation::CDistributor &distributor = swift::misc::simulation::CDistributor());

    protected:
        //! \copydoc CForm::jsonPasted
        virtual void jsonPasted(const QString &json) override;

    private:
        //! Variant has been dropped
        void onDroppedCode(const swift::misc::CVariant &variantDropped);

        //! Any UI values in the details fields
        bool hasAnyUiDetailsValues() const;

        QScopedPointer<Ui::CDistributorForm> ui;
        swift::misc::simulation::CDistributor m_currentDistributor;
    };
} // ns

#endif // guard
