// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_EDITORS_PILOTFORM_H
#define SWIFT_GUI_EDITORS_PILOTFORM_H

#include <QFrame>
#include <QIntValidator>
#include <QScopedPointer>

#include "gui/editors/form.h"
#include "gui/swiftguiexport.h"
#include "misc/network/entityflags.h"
#include "misc/network/user.h"

namespace Ui
{
    class CPilotForm;
}
namespace swift::gui::editors
{
    //! Pilot form (name, homebase)
    class SWIFT_GUI_EXPORT CPilotForm : public CForm
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
} // namespace swift::gui::editors

#endif // guard
