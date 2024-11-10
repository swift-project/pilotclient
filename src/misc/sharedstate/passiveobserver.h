// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SHAREDSTATE_PASSIVEOBSERVER_H
#define SWIFT_MISC_SHAREDSTATE_PASSIVEOBSERVER_H

#include "misc/variant.h"
#include "misc/valueobject.h"
#include "misc/swiftmiscexport.h"
#include <QObject>
#include <QSharedPointer>
#include <QMutex>

BLACK_DECLARE_VALUEOBJECT_MIXINS(swift::misc::shared_state, CAnyMatch)

namespace swift::misc::shared_state
{
    class CActiveObserver;

    /*!
     * Endpoint which can subscribe to events emitted by CPassiveMutator.
     * \ingroup shared_state
     */
    class SWIFT_MISC_EXPORT CPassiveObserver : public QObject, public QEnableSharedFromThis<CPassiveObserver>
    {
        Q_OBJECT
        friend CActiveObserver;
        friend QSharedPointer<CPassiveObserver>;

        template <typename T, typename F>
        CPassiveObserver(T *parent, F eventHandler) : QObject(parent),
                                                      m_eventHandler([=](const CVariant &param) { private_ns::invokeMethod(parent, eventHandler, param); })
        {}

    public:
        //! Factory method.
        template <typename T, typename F>
        static auto create(T *parent, F eventHandler)
        {
            return QSharedPointer<CPassiveObserver>::create(parent, eventHandler);
        }

        //! Set the object that determines which events are subscribed to.
        void setEventSubscription(const CVariant &param);

        //! Get the object that determines which events are subscribed to.
        CVariant eventSubscription() const;

        //! Called when a subscribed event is emitted.
        void handleEvent(const CVariant &param) const;

        //! Get a QWeakPointer pointing to this object.
        QWeakPointer<const CPassiveObserver> weakRef() const { return sharedFromThis(); }

    signals:
        //! Emitted by setEventSubscription.
        void eventSubscriptionChanged(const swift::misc::CVariant &param);

    private:
        const std::function<void(const CVariant &param)> m_eventHandler;

        mutable QMutex m_eventSubscriptionMutex;
        CVariant m_eventSubscription;
    };

    /*!
     * Dummy value class that matches any event.
     * \ingroup shared_state
     */
    class SWIFT_MISC_EXPORT CAnyMatch : public CValueObject<CAnyMatch>
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

Q_DECLARE_METATYPE(swift::misc::shared_state::CAnyMatch)

#endif
