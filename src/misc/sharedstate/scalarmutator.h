// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SHAREDSTATE_SCALARMUTATOR_H
#define SWIFT_MISC_SHAREDSTATE_SCALARMUTATOR_H

#include <QMutex>
#include <QObject>

#include "misc/sharedstate/passivemutator.h"
#include "misc/swiftmiscexport.h"
#include "misc/variant.h"

namespace swift::misc::shared_state
{
    class IDataLink;

    /*!
     * Non-template base class for CScalarMutator.
     * \ingroup shared_state
     */
    class SWIFT_MISC_EXPORT CGenericScalarMutator : public QObject
    {
        Q_OBJECT

    public:
        //! Publish using the given transport mechanism.
        void initialize(IDataLink *);

    protected:
        //! Constructor.
        CGenericScalarMutator(QObject *parent) : QObject(parent) {}

        //! Set scalar value as variant.
        void setValue(const CVariant &value);

    private:
        QSharedPointer<CPassiveMutator> m_mutator = CPassiveMutator::create(this);
    };

    /*!
     * Base class for an object that shares state with a corresponding CScalarObserver subclass object.
     * \tparam T Datatype encapsulating the state to be shared.
     * \ingroup shared_state
     */
    template <typename T>
    class CScalarMutator : public CGenericScalarMutator
    {
    protected:
        //! Constructor.
        CScalarMutator(QObject *parent) : CGenericScalarMutator(parent) {}

    public:
        //! Set scalar value.
        void setValue(const T &value) { CGenericScalarMutator::setValue(CVariant::from(value)); }
    };
} // namespace swift::misc::shared_state

#endif // SWIFT_MISC_SHAREDSTATE_SCALARMUTATOR_H
