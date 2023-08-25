// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_EDITORS_AIRCRAFTMODELFORM_H
#define BLACKGUI_EDITORS_AIRCRAFTMODELFORM_H

#include "blackgui/editors/form.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/simulation/distributor.h"

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
        virtual BlackMisc::CStatusMessageList validate(bool withNestedForms = true) const override;

        //! \copydoc BlackGui::Editors::CForm::validate
        virtual BlackMisc::CStatusMessageList validateLivery(bool withNestedForms = true) const;

        //! \copydoc BlackGui::Editors::CForm::validate
        virtual BlackMisc::CStatusMessageList validateAircraftIcao(bool withNestedForms = true) const;

        //! \copydoc BlackGui::Editors::CForm::validate
        virtual BlackMisc::CStatusMessageList validateDistributor(bool withNestedForms = true) const;

        //! Livery
        BlackMisc::Aviation::CLivery getLivery() const;

        //! Aircraft ICAO
        BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcao() const;

        //! Distributor
        BlackMisc::Simulation::CDistributor getDistributor() const;

        //! Livery
        bool setLivery(const BlackMisc::Aviation::CLivery &livery);

        //! Aircraft
        bool setAircraftIcao(const BlackMisc::Aviation::CAircraftIcaoCode &icao);

        //! Distributor
        bool setDistributor(const BlackMisc::Simulation::CDistributor &distributor);

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
