// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_EDITORS_AIRCRAFTICAOFORM_H
#define BLACKGUI_EDITORS_AIRCRAFTICAOFORM_H

#include "blackgui/editors/form.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/variant.h"

#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CAircraftIcaoForm;
}
namespace BlackGui::Editors
{
    /*!
     * Aircraft ICAO form
     */
    class BLACKGUI_EXPORT CAircraftIcaoForm : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftIcaoForm(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAircraftIcaoForm() override;

        //! Get value
        BlackMisc::Aviation::CAircraftIcaoCode getValue() const;

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

    public slots:
        //! Set value
        bool setValue(const BlackMisc::Aviation::CAircraftIcaoCode &icao);

    protected:
        //! \copydoc CForm::jsonPasted
        virtual void jsonPasted(const QString &json) override;

    private:
        QScopedPointer<Ui::CAircraftIcaoForm> ui;
        BlackMisc::Aviation::CAircraftIcaoCode m_originalCode;

        //! Variant has been dropped
        void droppedCode(const BlackMisc::CVariant &variantDropped);

        //! Id has been entered
        void idEntered();

        //! Key from GUI
        int getDbKeyFromGui() const;
    };
} // ns

#endif // guard
