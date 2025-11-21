// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SHAREDSTATE_DATALINKLOCAL_H
#define SWIFT_MISC_SHAREDSTATE_DATALINKLOCAL_H

#include <QFuture>
#include <QMutex>
#include <QObject>

#include "misc/sharedstate/datalink.h"
#include "misc/swiftmiscexport.h"
#include "misc/variant.h"

namespace swift::misc::shared_state
{
    /*!
     * A transport mechanism using signals and slots in the local process.
     * \ingroup shared_state
     */
    class SWIFT_MISC_EXPORT CDataLinkLocal : public QObject, public IDataLink
    {
        Q_OBJECT
        Q_INTERFACES(swift::misc::shared_state::IDataLink)

    public:
        //! Constructor.
        CDataLinkLocal(QObject *parent = nullptr);

        //! Destructor.
        ~CDataLinkLocal() override;

        void publish(const CPassiveMutator *mutator) override;
        void publish(const CActiveMutator *mutator) override;
        void subscribe(const CPassiveObserver *observer) override;
        void subscribe(const CActiveObserver *observer) override;

    private:
        struct Channel
        {
            QWeakPointer<const CActiveMutator> activeMutator;
            QVector<QWeakPointer<const CPassiveObserver>> passiveObservers;
        };

        void dispatchEvent(const CVariant &param, const QString &channel);
        QFuture<CVariant> handleRequest(const CVariant &param, const QString &channel);
        Channel &getChannel(const QString &name);
        Channel &getChannel(const QObject *object);

        QMap<QString, Channel> m_channels;
        mutable QRecursiveMutex m_channelsMutex;
    };
} // namespace swift::misc::shared_state

#endif // SWIFT_MISC_SHAREDSTATE_DATALINKLOCAL_H
