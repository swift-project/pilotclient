/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_LIVERYFORM_H
#define BLACKGUI_EDITORS_LIVERYFORM_H

#include "blackgui/blackguiexport.h"
#include "blackgui/editors/form.h"
#include "blackgui/components/dbliverycolorsearchdialog.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/variant.h"

#include <QObject>
#include <QScopedPointer>

class QWidget;

namespace Ui { class CLiveryForm; }
namespace BlackGui::Editors
{
    /*!
     * Livery form class
     */
    class BLACKGUI_EXPORT CLiveryForm : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CLiveryForm(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CLiveryForm() override;

        //! Value
        BlackMisc::Aviation::CLivery getValue() const;

        //! Embedded ariline
        BlackMisc::Aviation::CAirlineIcaoCode getValueAirlineIcao() const;

        //! Validate airline ICAO code only
        BlackMisc::CStatusMessageList validateAirlineIcao() const;

        //! Allow to drop
        void allowDrop(bool allowDrop);

        //! Is drop allowed?
        bool isDropAllowed() const;

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        virtual void setSelectOnly() override;
        virtual BlackMisc::CStatusMessageList validate(bool withNestedForms) const override;
        //! @}

        //! Clear data
        void clear();

        //! Value
        bool setValue(const BlackMisc::Aviation::CLivery &livery);

        //! Reset value to current value
        void resetValue();

    protected:
        //! \copydoc CForm::jsonPasted
        virtual void jsonPasted(const QString &json) override;

    private:
        //! Livery dropped
        void onDroppedLivery(const BlackMisc::CVariant &variantDropped);

        //! Airline of embedded form has changed
        void onAirlineChanged(const BlackMisc::Aviation::CAirlineIcaoCode &code);

        //! Id entered
        void onIdEntered();

        //! Set data for a temp.livery
        void setTemporaryLivery();

        //! Search for color
        void searchForColor();

        QScopedPointer<Ui::CLiveryForm> ui;
        BlackMisc::Aviation::CLivery m_originalLivery; //!< object allowing to override values
        BlackGui::Components::CDbLiveryColorSearchDialog *m_colorSearch = nullptr; //!< search for color
    };
} // ns

#endif // guard
