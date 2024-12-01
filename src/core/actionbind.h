// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_ACTIONBIND_H
#define SWIFT_CORE_ACTIONBIND_H

#include "core/application.h"
#include "core/swiftcoreexport.h"

namespace swift::core
{
    /*!
     * CActionBind binds a member function to an action
     */
    class SWIFT_CORE_EXPORT CActionBind : public QObject
    {
        Q_OBJECT

    public:
        //! Signature of receiving member function
        template <typename U>
        using MembFunc = void (U::*)(bool);

        //! Constructor
        template <typename Receiver>
        CActionBind(const QString &action, swift::misc::CIcons::IconIndex icon, Receiver *receiver,
                    MembFunc<Receiver> slot = nullptr, const std::function<void()> &deleteCallback = {},
                    QObject *parent = nullptr)
            : QObject(parent), m_deleteCallback(deleteCallback)
        {
            // workaround if a binding is taking place in an empty context
            if (!sApp || sApp->isShuttingDown()) { return; }
            if (sApp->getApplicationInfo().isUnitTest()) { return; }

            const QString a = CActionBind::registerAction(action, icon);
            Q_ASSERT_X(sApp && sApp->getInputManager(), Q_FUNC_INFO, "Missing input manager");
            m_index = sApp->getInputManager()->bind(a, receiver, slot);
            QObject::connect(sApp, &CApplication::aboutToShutdown, this, &CActionBind::shutdown);
        }

        //! Signature just to set an icon for an action
        CActionBind(const QString &action, swift::misc::CIcons::IconIndex icon, QObject *parent = nullptr);

        //! @{
        //! Not copyable
        CActionBind(const CActionBind &) = delete;
        CActionBind &operator=(const CActionBind &) = delete;
        //! @}

        //! Destructor
        virtual ~CActionBind();

        //! Unbind from swift::core::CInputManager
        void unbind();

        //! Bound with swift::core::CInputManager
        bool isBound() const { return m_index >= 0; }

        //! Index
        int getIndex() const { return m_index; }

    private:
        void shutdown();

        //! Normalize the action string
        static QString normalizeAction(const QString &action);

        //! Register action
        static QString registerAction(const QString &action, swift::misc::CIcons::IconIndex icon);

        int m_index = -1; //!< action index (unique)
        std::function<void()> m_deleteCallback; //!< called when deleted
    };

    //! Single binding
    using CActionBinding = QSharedPointer<CActionBind>;

    //! List of bindings
    using CActionBindings = QList<CActionBinding>;
} // namespace swift::core

#endif // SWIFT_CORE_ACTIONBIND_H
