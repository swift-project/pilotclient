// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SHAREDSTATE_LISTJOURNAL_H
#define SWIFT_MISC_SHAREDSTATE_LISTJOURNAL_H

#include <QMutex>
#include <QObject>

#include "misc/sharedstate/activemutator.h"
#include "misc/sharedstate/passiveobserver.h"
#include "misc/swiftmiscexport.h"
#include "misc/variantlist.h"

namespace swift::misc::shared_state
{
    class IDataLink;

    /*!
     * Non-template base class for CListJournal.
     * \ingroup shared_state
     */
    class SWIFT_MISC_EXPORT CGenericListJournal : public QObject
    {
        Q_OBJECT

    public:
        //! Publish using the given transport mechanism.
        void initialize(IDataLink *);

    protected:
        //! Constructor.
        CGenericListJournal(QObject *parent) : QObject(parent) {}

    private:
        CVariant handleRequest(const CVariant &filter);
        void handleEvent(const CVariant &param);

        QSharedPointer<CActiveMutator> m_mutator = CActiveMutator::create(this, &CGenericListJournal::handleRequest);
        QSharedPointer<CPassiveObserver> m_observer = CPassiveObserver::create(this, &CGenericListJournal::handleEvent);
        CVariantList m_value;
    };

    /*!
     * Base class for an object that shares state with a corresponding CListObserver subclass object.
     * \tparam T Datatype encapsulating the state to be shared.
     * \ingroup shared_state
     */
    template <typename T>
    class CListJournal : public CGenericListJournal
    {
    protected:
        //! Constructor.
        CListJournal(QObject *parent) : CGenericListJournal(parent) {}
    };
} // namespace swift::misc::shared_state

#endif // SWIFT_MISC_SHAREDSTATE_LISTJOURNAL_H
