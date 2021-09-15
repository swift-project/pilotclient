/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_AIRLINEICAOFORM_H
#define BLACKGUI_EDITORS_AIRLINEICAOFORM_H

#include "blackgui/blackguiexport.h"
#include "blackgui/editors/form.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/digestsignal.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/variant.h"

#include <QObject>
#include <QScopedPointer>

namespace Ui { class CAirlineIcaoForm; }
namespace BlackGui::Editors
{
    /*!
     * Airline ICAO code form
     */
    class BLACKGUI_EXPORT CAirlineIcaoForm : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAirlineIcaoForm(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAirlineIcaoForm() override;

        //! Set value
        void setValue(const BlackMisc::Aviation::CAirlineIcaoCode &icao = BlackMisc::Aviation::CAirlineIcaoCode());

        //! Get value
        BlackMisc::Aviation::CAirlineIcaoCode getValue() const;

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

        //! Reset value to current value
        void resetValue();

    signals:
        //! Airline has been changed
        void airlineChanged(const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcao);

        //! Airline has been changed
        void airlineChangedDigest(const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcao);

    protected:
        //! \copydoc CForm::jsonPasted
        virtual void jsonPasted(const QString &json) override;

        //! Variant has been dropped
        void onDroppedCode(const BlackMisc::CVariant &variantDropped);

        //! Id entered
        void onIdEntered();

        //! Emit the digest signal
        void emitAirlineChangedDigest();

    private:
        QScopedPointer<Ui::CAirlineIcaoForm>  ui;
        BlackMisc::CDigestSignal m_digestChanges { this, &CAirlineIcaoForm::airlineChanged, &CAirlineIcaoForm::emitAirlineChangedDigest, 500, 3 };
        BlackMisc::Aviation::CAirlineIcaoCode m_currentCode; //!< object allowing to override values
    };
} //ns

#endif // guard
