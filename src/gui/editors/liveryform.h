// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_EDITORS_LIVERYFORM_H
#define SWIFT_GUI_EDITORS_LIVERYFORM_H

#include <QObject>
#include <QScopedPointer>

#include "gui/components/dbliverycolorsearchdialog.h"
#include "gui/editors/form.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/livery.h"
#include "misc/statusmessagelist.h"
#include "misc/variant.h"

class QWidget;

namespace Ui
{
    class CLiveryForm;
}
namespace swift::gui::editors
{
    /*!
     * Livery form class
     */
    class SWIFT_GUI_EXPORT CLiveryForm : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CLiveryForm(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CLiveryForm() override;

        //! Value
        swift::misc::aviation::CLivery getValue() const;

        //! Embedded ariline
        swift::misc::aviation::CAirlineIcaoCode getValueAirlineIcao() const;

        //! Validate airline ICAO code only
        swift::misc::CStatusMessageList validateAirlineIcao() const;

        //! Allow to drop
        void allowDrop(bool allowDrop);

        //! Is drop allowed?
        bool isDropAllowed() const;

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        virtual void setSelectOnly() override;
        virtual swift::misc::CStatusMessageList validate(bool withNestedForms) const override;
        //! @}

        //! Clear data
        void clear();

        //! Value
        bool setValue(const swift::misc::aviation::CLivery &livery);

        //! Reset value to current value
        void resetValue();

    protected:
        //! \copydoc CForm::jsonPasted
        virtual void jsonPasted(const QString &json) override;

    private:
        //! Livery dropped
        void onDroppedLivery(const swift::misc::CVariant &variantDropped);

        //! Airline of embedded form has changed
        void onAirlineChanged(const swift::misc::aviation::CAirlineIcaoCode &code);

        //! Id entered
        void onIdEntered();

        //! Set data for a temp.livery
        void setTemporaryLivery();

        //! Search for color
        void searchForColor();

        QScopedPointer<Ui::CLiveryForm> ui;
        swift::misc::aviation::CLivery m_originalLivery; //!< object allowing to override values
        swift::gui::components::CDbLiveryColorSearchDialog *m_colorSearch = nullptr; //!< search for color
    };
} // namespace swift::gui::editors

#endif // SWIFT_GUI_EDITORS_LIVERYFORM_H
