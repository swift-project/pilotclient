/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_application_impl.h"
#include "context_runtime.h"
#include "blackmisc/settingutilities.h"
#include <QtMsgHandler>

using namespace BlackMisc;

namespace BlackCore
{
    /*
     * Init this context
     */
    CContextApplication::CContextApplication(CRuntimeConfig::ContextMode mode, CRuntime *runtime) :
        CContextApplicationBase(mode, runtime)
    {}

    /*
     * Ping, is DBus alive?
     */
    qint64 CContextApplication::ping(qint64 token) const
    {
        return token;
    }

    /*
     * Status message
     */
    void CContextApplication::sendStatusMessage(const CStatusMessage &message)
    {
        emit this->statusMessage(message);
    }

    /*
     * Status messages
     */
    void CContextApplication::sendStatusMessages(const CStatusMessageList &messages)
    {
        emit this->statusMessages(messages);
    }

    /*
     * Component has changed
     */
    void CContextApplication::notifyAboutComponentChange(uint component, uint action)
    {
        this->componentChanged(component, action);
    }

} // namespace
