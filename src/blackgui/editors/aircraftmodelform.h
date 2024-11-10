// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_EDITORS_AIRCRAFTMODELFORM_H
#define BLACKGUI_EDITORS_AIRCRAFTMODELFORM_H

#include "blackgui/editors/form.h"
#include "misc/aviation/livery.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/simulation/distributor.h"

namespace Ui
{
    class CAircraftModelForm;
}
namespace BlackGui::Editors
{
    /*!
     * Combined form of Livery, ICAOs, distributor
     */
    class CAircraftModelForm : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftModelForm(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAircraftModelForm() override;

        //! Allow to drop data
        void allowDrop(bool allowDrop);

        //! \copydoc BlackGui::Editors::CForm::setReadOnly
        virtual void setReadOnly(bool readOnly) override;

        //! \copydoc BlackGui::Editors::CForm::setSelectOnly
        virtual void setSelectOnly() override;

        //! \copydoc BlackGui::Editors::CForm::validate
        virtual swift::misc::CStatusMessageList validate(bool withNestedForms = true) const override;

        //! \copydoc BlackGui::Editors::CForm::validate
        virtual swift::misc::CStatusMessageList validateLivery(bool withNestedForms = true) const;

        //! \copydoc BlackGui::Editors::CForm::validate
        virtual swift::misc::CStatusMessageList validateAircraftIcao(bool withNestedForms = true) const;

        //! \copydoc BlackGui::Editors::CForm::validate
        virtual swift::misc::CStatusMessageList validateDistributor(bool withNestedForms = true) const;

        //! Livery
        swift::misc::aviation::CLivery getLivery() const;

        //! Aircraft ICAO
        swift::misc::aviation::CAircraftIcaoCode getAircraftIcao() const;

        //! Distributor
        swift::misc::simulation::CDistributor getDistributor() const;

        //! Livery
        bool setLivery(const swift::misc::aviation::CLivery &livery);

        //! Aircraft
        bool setAircraftIcao(const swift::misc::aviation::CAircraftIcaoCode &icao);

        //! Distributor
        bool setDistributor(const swift::misc::simulation::CDistributor &distributor);

        //! Clear entire form
        void clear();

        //! \copydoc BlackGui::Editors::CLiveryForm::clear
        void clearLivery();

        //! \copydoc BlackGui::Editors::CAircraftIcaoForm::clear
        void clearAircraftIcao();

        //! \copydoc BlackGui::Editors::CDistributorForm::clear
        void clearDistributor();

    private:
        QScopedPointer<Ui::CAircraftModelForm> ui;
    };
} // ns

#endif // guard
