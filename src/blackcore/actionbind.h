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
#include "blackcoreexport.h"
#include <QPixmap>

namespace BlackCore
{
    /*!
     * CActionBind binds a member function to an action
     */
    class BLACKCORE_EXPORT CActionBind
    {
    public:
        //! Signature of receiving member function
        template <typename U>
        using MembFunc = void (U::*)(bool);

        //! Constructor
        template <typename Receiver>
        CActionBind(const QString &action, const QPixmap &icon, Receiver *receiver,
                    MembFunc<Receiver> slot = nullptr,
                    const std::function<void()> &deleteCallback = {}) :
            m_deleteCallback(deleteCallback)
        {
            const QString a = CActionBind::registerAction(action, icon);
            m_index = CInputManager::instance()->bind(a, receiver, slot);
        }

        //! Signature just to set an icon for an action
        CActionBind(const QString &action, const QPixmap &icon);

        //! Destructor
        ~CActionBind();

        //! Unbind from BlackCore::CInputManager
        void unbind();

        //! Bound with BlackCore::CInputManager
        bool isBound() const { return m_index >= 0; }

        //! Index
        int getIndex() const { return m_index; }

    private:
        //! Normalize the action string
        static QString normalizeAction(const QString &action);

        //! Register action
        static QString registerAction(const QString &action, const QPixmap &icon);

        int m_index = -1; //!< action indexx (unique)
        std::function<void()> m_deleteCallback; //!< called when deleted
    };

    //! Single binding
    using CActionBinding  = QSharedPointer<CActionBind>;

    //! List of bindings
    using CActionBindings = QList<CActionBinding>;
}

#endif
