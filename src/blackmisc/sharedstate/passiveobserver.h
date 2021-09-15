/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SHAREDSTATE_PASSIVEOBSERVER_H
#define BLACKMISC_SHAREDSTATE_PASSIVEOBSERVER_H

#include "blackmisc/variant.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/blackmiscexport.h"
#include <QObject>
#include <QSharedPointer>
#include <QMutex>

namespace BlackMisc::SharedState
{
    class CActiveObserver;

    /*!
     * Endpoint which can subscribe to events emitted by CPassiveMutator.
     * \ingroup SharedState
     */
    class BLACKMISC_EXPORT CPassiveObserver : public QObject, public QEnableSharedFromThis<CPassiveObserver>
    {
        Q_OBJECT
        friend CActiveObserver;
        friend QSharedPointer<CPassiveObserver>;

        template <typename T, typename F>
        CPassiveObserver(T *parent, F eventHandler) :
            QObject(parent),
            m_eventHandler([ = ](const CVariant &param) { Private::invokeMethod(parent, eventHandler, param); })
        {}

    public:
        //! Factory method.
        template <typename T, typename F>
        static auto create(T *parent, F eventHandler) { return QSharedPointer<CPassiveObserver>::create(parent, eventHandler); }

        //! Set the object that determines which events are subscribed to.
        void setEventSubscription(const CVariant &param);

        //! Get the object that determines which events are subscribed to.
        CVariant eventSubscription() const;

        //! Called when a subscribed event is emitted.
        void handleEvent(const CVariant& param) const;

        //! Get a QWeakPointer pointing to this object.
        QWeakPointer<const CPassiveObserver> weakRef() const { return sharedFromThis(); }

    signals:
        //! Emitted by setEventSubscription.
        void eventSubscriptionChanged(const BlackMisc::CVariant &param);

    private:
        const std::function<void(const CVariant &param)> m_eventHandler;

        mutable QMutex m_eventSubscriptionMutex;
        CVariant m_eventSubscription;
    };

    /*!
     * Dummy value class that matches any event.
     * \ingroup SharedState
     */
    class BLACKMISC_EXPORT CAnyMatch : public CValueObject<CAnyMatch>
    {
    public:
        //! Returns true.
        bool matches(const CVariant &) const { return true; }

        //! To string.
        QString convertToQString(bool = false) const { return QStringLiteral("any"); }

    private:
        int m_dummy = 0;

        BLACK_METACLASS(CAnyMatch, BLACK_METAMEMBER(dummy));
    };
}

Q_DECLARE_METATYPE(BlackMisc::SharedState::CAnyMatch)

#endif
