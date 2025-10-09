// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_EDITORS_AIRLINEICAOFORM_H
#define SWIFT_GUI_EDITORS_AIRLINEICAOFORM_H

#include <QObject>
#include <QScopedPointer>

#include "gui/editors/form.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/digestsignal.h"
#include "misc/statusmessagelist.h"
#include "misc/variant.h"

namespace Ui
{
    class CAirlineIcaoForm;
}
namespace swift::gui::editors
{
    /*!
     * Airline ICAO code form
     */
    class SWIFT_GUI_EXPORT CAirlineIcaoForm : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAirlineIcaoForm(QWidget *parent = nullptr);

        //! Destructor
        ~CAirlineIcaoForm() override;

        //! Set value
        void setValue(const swift::misc::aviation::CAirlineIcaoCode &icao = swift::misc::aviation::CAirlineIcaoCode());

        //! Get value
        swift::misc::aviation::CAirlineIcaoCode getValue() const;

        //! Allow to drop
        void allowDrop(bool allowDrop);

        //! Is drop allowed?
        bool isDropAllowed() const;

        //! \name Form class implementations
        //! @{
        void setReadOnly(bool readonly) override;
        void setSelectOnly() override;
        swift::misc::CStatusMessageList validate(bool nested = false) const override;
        //! @}

        //! Clear
        void clear();

        //! Reset value to current value
        void resetValue();

    signals:
        //! Airline has been changed
        void airlineChanged(const swift::misc::aviation::CAirlineIcaoCode &airlineIcao);

        //! Airline has been changed
        void airlineChangedDigest(const swift::misc::aviation::CAirlineIcaoCode &airlineIcao);

    protected:
        //! \copydoc CForm::jsonPasted
        void jsonPasted(const QString &json) override;

        //! Variant has been dropped
        void onDroppedCode(const swift::misc::CVariant &variantDropped);

        //! Id entered
        void onIdEntered();

        //! Emit the digest signal
        void emitAirlineChangedDigest();

    private:
        QScopedPointer<Ui::CAirlineIcaoForm> ui;
        swift::misc::CDigestSignal m_digestChanges { this, &CAirlineIcaoForm::airlineChanged,
                                                     &CAirlineIcaoForm::emitAirlineChangedDigest,
                                                     std::chrono::milliseconds(500), 3 };
        swift::misc::aviation::CAirlineIcaoCode m_currentCode; //!< object allowing to override values
    };
} // namespace swift::gui::editors

#endif // SWIFT_GUI_EDITORS_AIRLINEICAOFORM_H
