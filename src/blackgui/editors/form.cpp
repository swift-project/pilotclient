// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/editors/form.h"
#include "blackgui/guiutility.h"
#include <QApplication>
#include <QClipboard>
#include <QShortcut>

using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackGui::Editors
{
    CForm::CForm(QWidget *parent) : COverlayMessagesFrame(parent)
    {}

    CForm::~CForm() {}

    void CForm::setSelectOnly()
    {
        this->setReadOnly(true);
    }

    CStatusMessageList CForm::validate(bool withNestedObjects) const
    {
        Q_UNUSED(withNestedObjects);
        return CStatusMessageList();
    }

    CStatusMessageList CForm::validateAsOverlayMessage(bool withNestedObjects, bool appendOldMessages, int timeOutMs)
    {
        const CStatusMessageList msgs = this->validate(withNestedObjects);
        this->showOverlayMessages(msgs, appendOldMessages, timeOutMs);
        return msgs;
    }

    void CForm::jsonPasted(const QString &json)
    {
        Q_UNUSED(json);
    }

    void CForm::pasted()
    {
        if (!QApplication::clipboard()) { return; }
        const QString data = QApplication::clipboard()->text();
        if (!Json::looksLikeSwiftJson(data)) { return; }
        this->jsonPasted(data);
    }

    void CForm::forceStyleSheetUpdate()
    {
        CGuiUtility::forceStyleSheetUpdate(this);
    }

    CFormDbUser::CFormDbUser(QWidget *parent) : CForm(parent)
    {}

    CFormDbUser::~CFormDbUser()
    {}

    CAuthenticatedUser CFormDbUser::getSwiftDbUser() const
    {
        return m_swiftDbUser.get();
    }

    void CFormDbUser::userChanged()
    {
        // void
    }
} // ns
