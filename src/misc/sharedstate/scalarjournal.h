// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SHAREDSTATE_SCALARJOURNAL_H
#define SWIFT_MISC_SHAREDSTATE_SCALARJOURNAL_H

#include <QMutex>
#include <QObject>

#include "misc/sharedstate/activemutator.h"
#include "misc/sharedstate/passiveobserver.h"
#include "misc/swiftmiscexport.h"
#include "misc/variant.h"

namespace swift::misc::shared_state
{
    class IDataLink;

    /*!
     * Non-template base class for CScalarJournal.
     * \ingroup shared_state
     */
    class SWIFT_MISC_EXPORT CGenericScalarJournal : public QObject
    {
        Q_OBJECT

    public:
        //! Publish using the given transport mechanism.
        void initialize(IDataLink *);

    protected:
        //! Constructor.
        CGenericScalarJournal(QObject *parent) : QObject(parent) {}

    private:
        CVariant handleRequest(const CVariant &param);
        void handleEvent(const CVariant &param);

        QSharedPointer<CActiveMutator> m_mutator = CActiveMutator::create(this, &CGenericScalarJournal::handleRequest);
        QSharedPointer<CPassiveObserver> m_observer = CPassiveObserver::create(this, &CGenericScalarJournal::handleEvent);
        CVariant m_value;
    };

    /*!
     * Base class for an object that shares state with a corresponding CScalarObserver subclass object.
     * \tparam T Datatype encapsulating the state to be shared.
     * \ingroup shared_state
     */
    template <typename T>
    class CScalarJournal : public CGenericScalarJournal
    {
    protected:
        //! Constructor.
        CScalarJournal(QObject *parent) : CGenericScalarJournal(parent) {}
    };
} // namespace swift::misc::shared_state

#endif
