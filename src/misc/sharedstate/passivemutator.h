// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SHAREDSTATE_PASSIVEMUTATOR_H
#define SWIFT_MISC_SHAREDSTATE_PASSIVEMUTATOR_H

#include <QObject>
#include <QSharedPointer>

#include "misc/swiftmiscexport.h"
#include "misc/variant.h"

namespace swift::misc::shared_state
{
    class CActiveMutator;

    /*!
     * Endpoint which can emit events to subscribers.
     * \ingroup shared_state
     */
    class SWIFT_MISC_EXPORT CPassiveMutator : public QObject, public QEnableSharedFromThis<CPassiveMutator>
    {
        Q_OBJECT
        friend CActiveMutator;
        friend QSharedPointer<CPassiveMutator>;

        CPassiveMutator(QObject *parent) : QObject(parent) {}

    public:
        //! Factory method.
        static auto create(QObject *parent) { return QSharedPointer<CPassiveMutator>::create(parent); }

        //! Emit an event.
        void postEvent(const CVariant &param);

        //! Get a QWeakPointer pointing to this object.
        QWeakPointer<const CPassiveMutator> weakRef() const { return sharedFromThis(); }

    signals:
        //! Emitted by postEvent.
        void eventPosted(const swift::misc::CVariant &param);
    };
} // namespace swift::misc::shared_state

#endif
