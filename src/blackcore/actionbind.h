/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_ACTIONBIND_H
#define BLACKCORE_ACTIONBIND_H

#include "blackcore/inputmanager.h"

namespace BlackCore
{
    /*!
     * CActionBind binds a member function to an action
     */
    class CActionBind
    {
    public:
        //! Signature of receiving member function
        template <typename U>
        using MembFunc = void (U::*)(bool);

        //! Constructor
        template <typename Receiver>
        CActionBind(const QString &action, Receiver *receiver, MembFunc<Receiver> slot = nullptr)
        {
            auto inputManger = CInputManager::instance();
            inputManger->registerAction(action);
            m_index = inputManger->bind(action, receiver, slot);
        }

        //! Destructor
        ~CActionBind()
        {
            auto inputManger = CInputManager::instance();
            inputManger->unbind(m_index);
        }

    private:
        int m_index;
    };
}

#endif

