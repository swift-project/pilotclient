/* Copyright (C) 2020
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SHAREDSTATE_SCALARJOURNAL_H
#define BLACKMISC_SHAREDSTATE_SCALARJOURNAL_H

#include "blackmisc/sharedstate/activemutator.h"
#include "blackmisc/sharedstate/passiveobserver.h"
#include "blackmisc/variant.h"
#include "blackmisc/blackmiscexport.h"
#include <QObject>
#include <QMutex>

namespace BlackMisc::SharedState
{
    class IDataLink;

    /*!
     * Non-template base class for CScalarJournal.
     * \ingroup SharedState
     */
    class BLACKMISC_EXPORT CGenericScalarJournal : public QObject
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
     * \ingroup SharedState
     */
    template <typename T>
    class CScalarJournal : public CGenericScalarJournal
    {
    protected:
        //! Constructor.
        CScalarJournal(QObject *parent) : CGenericScalarJournal(parent) {}
    };
}

#endif
