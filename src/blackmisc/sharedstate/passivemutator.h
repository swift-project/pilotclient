// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SHAREDSTATE_PASSIVEMUTATOR_H
#define BLACKMISC_SHAREDSTATE_PASSIVEMUTATOR_H

#include "blackmisc/variant.h"
#include "blackmisc/blackmiscexport.h"
#include <QObject>
#include <QSharedPointer>

namespace BlackMisc::SharedState
{
    class CActiveMutator;

    /*!
     * Endpoint which can emit events to subscribers.
     * \ingroup SharedState
     */
    class BLACKMISC_EXPORT CPassiveMutator : public QObject, public QEnableSharedFromThis<CPassiveMutator>
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
        void eventPosted(const BlackMisc::CVariant &param);
    };
}

#endif
