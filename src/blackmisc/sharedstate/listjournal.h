// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SHAREDSTATE_LISTJOURNAL_H
#define BLACKMISC_SHAREDSTATE_LISTJOURNAL_H

#include "blackmisc/sharedstate/activemutator.h"
#include "blackmisc/sharedstate/passiveobserver.h"
#include "blackmisc/variantlist.h"
#include "blackmisc/blackmiscexport.h"
#include <QObject>
#include <QMutex>

namespace BlackMisc::SharedState
{
    class IDataLink;

    /*!
     * Non-template base class for CListJournal.
     * \ingroup SharedState
     */
    class BLACKMISC_EXPORT CGenericListJournal : public QObject
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
     * \ingroup SharedState
     */
    template <typename T>
    class CListJournal : public CGenericListJournal
    {
    protected:
        //! Constructor.
        CListJournal(QObject *parent) : CGenericListJournal(parent) {}
    };
}

#endif
