// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SHAREDSTATE_LISTMUTATOR_H
#define SWIFT_MISC_SHAREDSTATE_LISTMUTATOR_H

#include "misc/sharedstate/passivemutator.h"
#include "misc/variantlist.h"
#include "misc/swiftmiscexport.h"
#include <QObject>
#include <QMutex>

namespace swift::misc::shared_state
{
    class IDataLink;

    /*!
     * Non-template base class for CListMutator.
     * \ingroup shared_state
     */
    class SWIFT_MISC_EXPORT CGenericListMutator : public QObject
    {
        Q_OBJECT

    public:
        //! Publish using the given transport mechanism.
        void initialize(IDataLink *);

    protected:
        //! Constructor.
        CGenericListMutator(QObject *parent) : QObject(parent) {}

        //! Add list element as variant.
        void addElement(const CVariant &value);

    private:
        QSharedPointer<CPassiveMutator> m_mutator = CPassiveMutator::create(this);
    };

    /*!
     * Base class for an object that shares state with a corresponding CListObserver subclass object.
     * \tparam T Datatype encapsulating the state to be shared.
     * \ingroup shared_state
     */
    template <typename T>
    class CListMutator : public CGenericListMutator
    {
    protected:
        //! Constructor.
        CListMutator(QObject *parent) : CGenericListMutator(parent) {}

    public:
        //! Add list element.
        void addElement(const typename T::value_type &value) { CGenericListMutator::addElement(CVariant::from(value)); }
    };
}

#endif
