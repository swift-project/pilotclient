// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SHAREDSTATE_ACTIVEMUTATOR_H
#define SWIFT_MISC_SHAREDSTATE_ACTIVEMUTATOR_H

#include <functional>

#include <QFuture>
#include <QObject>

#include "misc/sharedstate/passivemutator.h"
#include "misc/swiftmiscexport.h"
#include "misc/variant.h"

namespace swift::misc::shared_state
{
    /*!
     * Extends CPassiveMutator with the ability to respond to requests.
     * \ingroup shared_state
     */
    class SWIFT_MISC_EXPORT CActiveMutator final : public CPassiveMutator
    {
        Q_OBJECT
        friend QSharedPointer<CActiveMutator>;

        template <typename T, typename F>
        CActiveMutator(T *parent, F requestHandler) : CPassiveMutator(parent),
                                                      m_requestHandler([=](const CVariant &param) { return private_ns::invokeMethod(parent, requestHandler, param); })
        {}

    public:
        //! Factory method.
        template <typename T, typename F>
        static auto create(T *parent, F requestHandler)
        {
            return QSharedPointer<CActiveMutator>::create(parent, requestHandler);
        }

        //! Respond to a request and return a reply.
        QFuture<CVariant> handleRequest(const CVariant &param) const;

        //! Get a QWeakPointer pointing to this object.
        QWeakPointer<const CActiveMutator> weakRef() const { return qSharedPointerCast<const CActiveMutator>(CPassiveMutator::weakRef()); }

    private:
        const std::function<QFuture<CVariant>(const CVariant &)> m_requestHandler;
    };
} // namespace swift::misc::shared_state

#endif
