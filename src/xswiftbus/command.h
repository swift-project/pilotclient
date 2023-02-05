/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKSIM_XSWIFTBUS_COMMAND_H
#define BLACKSIM_XSWIFTBUS_COMMAND_H

//! \file

#include <XPLMUtilities.h>
#include <functional>

namespace XSwiftBus
{

    /*!
     * Class-based interface to X-Plane's custom command API.
     */
    class CCommand
    {
    public:
        //! Constructor.
        CCommand(const char *name, const char *description, std::function<void()> handler) :
            m_handler(handler),
            m_command(XPLMCreateCommand(name, description))
        {
            XPLMRegisterCommandHandler(m_command, callback, false, static_cast<void*>(this));
        }

        //! Destructor.
        ~CCommand()
        {
            XPLMUnregisterCommandHandler(m_command, callback, false, static_cast<void*>(this));
        }

        //! @{
        //! Not copyable.
        CCommand(const CCommand &) = delete;
        CCommand &operator =(const CCommand &) = delete;
        //! @}

    private:
        static int callback(XPLMCommandRef, XPLMCommandPhase phase, void *refcon)
        {
            if (phase == xplm_CommandBegin) { (static_cast<CCommand*>(refcon)->m_handler)(); }
            return 1;
        }

        std::function<void()> m_handler;
        XPLMCommandRef m_command;
    };

}

#endif
