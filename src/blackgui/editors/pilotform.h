/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_PILOTFORM_H
#define BLACKGUI_EDITORS_PILOTFORM_H

#include "blackgui/editors/form.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/network/user.h"
#include "blackmisc/network/entityflags.h"
#include <QFrame>
#include <QScopedPointer>
#include <QIntValidator>

namespace Ui { class CPilotForm; }
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
        BlackMisc::Network::CUser getUser() const;

        //! Set user values
        bool setUser(const BlackMisc::Network::CUser &user, bool ignoreEmptyUser = false);

        //! Clear values
        void clear();

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        virtual BlackMisc::CStatusMessageList validate(bool nested = false) const override;
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
