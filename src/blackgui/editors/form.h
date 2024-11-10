// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_EDITORS_FORM_H
#define BLACKGUI_EDITORS_FORM_H

#include "blackgui/overlaymessagesframe.h"
#include "blackgui/blackguiexport.h"
#include "core/data/authenticateduser.h"
#include "misc/datacache.h"
#include <QFrame>
#include <QObject>

class QWidget;

namespace swift::misc::network
{
    class CAuthenticatedUser;
}
namespace BlackGui::Editors
{
    //! Form base class
    class BLACKGUI_EXPORT CForm : public COverlayMessagesFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CForm(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CForm() override;

        //! Set editable
        virtual void setReadOnly(bool readOnly) = 0;

        //! Read only, but entity can be selected (normally used in mapping).
        //! Use setReadOnly to reset this very state
        virtual void setSelectOnly();

        //! Validate, empty list means OK
        virtual swift::misc::CStatusMessageList validate(bool withNestedObjects = true) const;

        //! Validate as overlay message
        //! \note uses CForm::validate and displays the messages as overlay messages
        swift::misc::CStatusMessageList validateAsOverlayMessage(bool withNestedObjects = true, bool appendOldMessages = false, int timeOutMs = -1);

        //! Is read only?
        bool isReadOnly() const { return m_readOnly; }

    protected:
        //! JSON string has been pasted
        //! \remark The JSON string has been pre-checked
        virtual void jsonPasted(const QString &json);

        //! Pasted from clipboard
        void pasted();

        //! \copydoc BlackGui::CGuiUtility::forceStyleSheetUpdate
        void forceStyleSheetUpdate();

        bool m_readOnly = false; //!< read only
    };

    //! Form base class
    class BLACKGUI_EXPORT CFormDbUser : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CFormDbUser(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CFormDbUser() override;

        //! Authenticated DB user
        swift::misc::network::CAuthenticatedUser getSwiftDbUser() const;

    protected:
        //! User has been changed
        virtual void userChanged();

        swift::misc::CDataReadOnly<swift::core::data::TAuthenticatedDbUser> m_swiftDbUser { this, &CFormDbUser::userChanged }; //!< authenticated user
    };
} // ns

#endif // guard
