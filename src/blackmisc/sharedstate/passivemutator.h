/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SHAREDSTATE_PASSIVEMUTATOR_H
#define BLACKMISC_SHAREDSTATE_PASSIVEMUTATOR_H

#include "blackmisc/variant.h"
#include "blackmisc/blackmiscexport.h"
#include <QObject>
#include <QSharedPointer>

namespace BlackMisc
{
    namespace SharedState
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

            CPassiveMutator(QObject* parent) : QObject(parent) {}

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
}

#endif
