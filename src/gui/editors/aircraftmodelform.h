// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_EDITORS_AIRCRAFTMODELFORM_H
#define SWIFT_GUI_EDITORS_AIRCRAFTMODELFORM_H

#include "gui/editors/form.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/livery.h"
#include "misc/simulation/distributor.h"

namespace Ui
{
    class CAircraftModelForm;
}
namespace swift::gui::editors
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
        ~CAircraftModelForm() override;

        //! Allow to drop data
        void allowDrop(bool allowDrop);

        //! \copydoc swift::gui::editors::CForm::setReadOnly
        void setReadOnly(bool readOnly) override;

        //! \copydoc swift::gui::editors::CForm::setSelectOnly
        void setSelectOnly() override;

        //! \copydoc swift::gui::editors::CForm::validate
        swift::misc::CStatusMessageList validate(bool withNestedForms = true) const override;

        //! \copydoc swift::gui::editors::CForm::validate
        virtual swift::misc::CStatusMessageList validateLivery(bool withNestedForms = true) const;

        //! \copydoc swift::gui::editors::CForm::validate
        virtual swift::misc::CStatusMessageList validateAircraftIcao(bool withNestedForms = true) const;

        //! \copydoc swift::gui::editors::CForm::validate
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

        //! \copydoc swift::gui::editors::CLiveryForm::clear
        void clearLivery();

        //! \copydoc swift::gui::editors::CAircraftIcaoForm::clear
        void clearAircraftIcao();

        //! \copydoc swift::gui::editors::CDistributorForm::clear
        void clearDistributor();

    private:
        QScopedPointer<Ui::CAircraftModelForm> ui;
    };
} // namespace swift::gui::editors

#endif // SWIFT_GUI_EDITORS_AIRCRAFTMODELFORM_H
