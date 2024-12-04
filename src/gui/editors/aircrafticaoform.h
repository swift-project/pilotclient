// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_EDITORS_AIRCRAFTICAOFORM_H
#define SWIFT_GUI_EDITORS_AIRCRAFTICAOFORM_H

#include <QObject>
#include <QScopedPointer>

#include "gui/editors/form.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/statusmessagelist.h"
#include "misc/variant.h"

namespace Ui
{
    class CAircraftIcaoForm;
}
namespace swift::gui::editors
{
    /*!
     * Aircraft ICAO form
     */
    class SWIFT_GUI_EXPORT CAircraftIcaoForm : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftIcaoForm(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAircraftIcaoForm() override;

        //! Get value
        swift::misc::aviation::CAircraftIcaoCode getValue() const;

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

        //! Reset value to current value
        void resetValue();

    public slots:
        //! Set value
        bool setValue(const swift::misc::aviation::CAircraftIcaoCode &icao);

    protected:
        //! \copydoc CForm::jsonPasted
        virtual void jsonPasted(const QString &json) override;

    private:
        QScopedPointer<Ui::CAircraftIcaoForm> ui;
        swift::misc::aviation::CAircraftIcaoCode m_originalCode;

        //! Variant has been dropped
        void droppedCode(const swift::misc::CVariant &variantDropped);

        //! Id has been entered
        void idEntered();

        //! Key from GUI
        int getDbKeyFromGui() const;
    };
} // namespace swift::gui::editors

#endif // SWIFT_GUI_EDITORS_AIRCRAFTICAOFORM_H
