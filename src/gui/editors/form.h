// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_EDITORS_FORM_H
#define SWIFT_GUI_EDITORS_FORM_H

#include <QFrame>
#include <QObject>

#include "core/data/authenticateduser.h"
#include "gui/overlaymessagesframe.h"
#include "gui/swiftguiexport.h"
#include "misc/datacache.h"

class QWidget;

namespace swift::misc::network
{
    class CAuthenticatedUser;
}
namespace swift::gui::editors
{
    //! Form base class
    class SWIFT_GUI_EXPORT CForm : public COverlayMessagesFrame
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
        swift::misc::CStatusMessageList
        validateAsOverlayMessage(bool withNestedObjects = true, bool appendOldMessages = false,
                                 std::chrono::milliseconds timeout = std::chrono::milliseconds(0));

        //! Is read only?
        bool isReadOnly() const { return m_readOnly; }

    protected:
        //! JSON string has been pasted
        //! \remark The JSON string has been pre-checked
        virtual void jsonPasted(const QString &json);

        //! Pasted from clipboard
        void pasted();

        //! \copydoc swift::gui::CGuiUtility::forceStyleSheetUpdate
        void forceStyleSheetUpdate();

        bool m_readOnly = false; //!< read only
    };

    //! Form base class
    class SWIFT_GUI_EXPORT CFormDbUser : public CForm
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

        swift::misc::CDataReadOnly<swift::core::data::TAuthenticatedDbUser> m_swiftDbUser {
            this, &CFormDbUser::userChanged
        }; //!< authenticated user
    };
} // namespace swift::gui::editors

#endif // SWIFT_GUI_EDITORS_FORM_H
