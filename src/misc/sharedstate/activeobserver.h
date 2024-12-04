// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SHAREDSTATE_ACTIVEOBSERVER_H
#define SWIFT_MISC_SHAREDSTATE_ACTIVEOBSERVER_H

#include <QObject>
#include <QSharedPointer>

#include "misc/promise.h"
#include "misc/sharedstate/passiveobserver.h"
#include "misc/swiftmiscexport.h"
#include "misc/variant.h"

namespace swift::misc::shared_state
{
    /*!
     * Extends CPassiveObserver with the ability to send requests and receive replies.
     * \ingroup shared_state
     */
    class SWIFT_MISC_EXPORT CActiveObserver final : public CPassiveObserver
    {
        Q_OBJECT
        friend QSharedPointer<CActiveObserver>;

        template <typename T, typename F>
        CActiveObserver(T *parent, F eventHandler) : CPassiveObserver(parent, eventHandler)
        {}

    public:
        //! Factory method.
        template <typename T, typename F>
        static auto create(T *parent, F eventHandler)
        {
            return QSharedPointer<CActiveObserver>::create(parent, eventHandler);
        }

        //! Send a request and receive a synchronous reply.
        CVariant request(const CVariant &param);

        //! Send a request and receive an asynchronous reply.
        //! The callback will not be called if the reply is received after the observer has been destroyed.
        void requestAsync(const CVariant &param, std::function<void(const CVariant &)> callback);

        //! Get a QWeakPointer pointing to this object.
        QWeakPointer<const CActiveObserver> weakRef() const
        {
            return qSharedPointerCast<const CActiveObserver>(CPassiveObserver::weakRef());
        }

    signals:
        //! Emitted by request and requestAsync.
        void requestPosted(const swift::misc::CVariant &param, swift::misc::CPromise<swift::misc::CVariant> o_reply);
    };
} // namespace swift::misc::shared_state

#endif // SWIFT_MISC_SHAREDSTATE_ACTIVEOBSERVER_H
