/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SHAREDSTATE_ACTIVEOBSERVER_H
#define BLACKMISC_SHAREDSTATE_ACTIVEOBSERVER_H

#include "blackmisc/sharedstate/passiveobserver.h"
#include "blackmisc/promise.h"
#include "blackmisc/variant.h"
#include "blackmisc/blackmiscexport.h"
#include <QObject>
#include <QSharedPointer>

namespace BlackMisc::SharedState
{
    /*!
     * Extends CPassiveObserver with the ability to send requests and receive replies.
     * \ingroup SharedState
     */
    class BLACKMISC_EXPORT CActiveObserver final : public CPassiveObserver
    {
        Q_OBJECT
        friend QSharedPointer<CActiveObserver>;

        template <typename T, typename F>
        CActiveObserver(T *parent, F eventHandler) : CPassiveObserver(parent, eventHandler) {}

    public:
        //! Factory method.
        template <typename T, typename F>
        static auto create(T *parent, F eventHandler) { return QSharedPointer<CActiveObserver>::create(parent, eventHandler); }

        //! Send a request and receive a synchronous reply.
        CVariant request(const CVariant &param);

        //! Send a request and receive an asynchronous reply.
        //! The callback will not be called if the reply is received after the observer has been destroyed.
        void requestAsync(const CVariant &param, std::function<void(const CVariant &)> callback);

        //! Get a QWeakPointer pointing to this object.
        QWeakPointer<const CActiveObserver> weakRef() const { return qSharedPointerCast<const CActiveObserver>(CPassiveObserver::weakRef()); }

    signals:
        //! Emitted by request and requestAsync.
        void requestPosted(const BlackMisc::CVariant &param, BlackMisc::CPromise<BlackMisc::CVariant> o_reply);
    };
}

#endif
