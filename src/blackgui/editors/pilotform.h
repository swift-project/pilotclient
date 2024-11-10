// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_EDITORS_PILOTFORM_H
#define BLACKGUI_EDITORS_PILOTFORM_H

#include "blackgui/editors/form.h"
#include "blackgui/blackguiexport.h"
#include "misc/network/user.h"
#include "misc/network/entityflags.h"
#include <QFrame>
#include <QScopedPointer>
#include <QIntValidator>

namespace Ui
{
    class CPilotForm;
}
namespace BlackGui::Editors
{
    //! Pilot form (name, homebase)
    class BLACKGUI_EXPORT CPilotForm : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CPilotForm(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CPilotForm() override;

        //! Set VATSIM validation mode
        void setVatsimValidation(bool vatsim);

        //! Values as user
        swift::misc::network::CUser getUser() const;

        //! Set user values
        bool setUser(const swift::misc::network::CUser &user, bool ignoreEmptyUser = false);

        //! Clear values
        void clear();

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        virtual swift::misc::CStatusMessageList validate(bool nested = false) const override;
        //! @}

    private:
        //! Unhide password
        void unhidePassword();

        //! Call validation
        void doValidation();

        QScopedPointer<Ui::CPilotForm> ui;
        QScopedPointer<QIntValidator> m_vatsimIntValidator { new QIntValidator(100000, 9999999, this) };
        bool m_vatsim = false;
    };
} // ns

#endif // guard
