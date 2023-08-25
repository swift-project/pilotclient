// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SHAREDSTATE_DATALINKLOCAL_H
#define BLACKMISC_SHAREDSTATE_DATALINKLOCAL_H

#include "blackmisc/sharedstate/datalink.h"
#include "blackmisc/variant.h"
#include "blackmisc/blackmiscexport.h"
#include <QObject>
#include <QFuture>
#include <QMutex>

namespace BlackMisc::SharedState
{
    /*!
     * A transport mechanism using signals and slots in the local process.
     * \ingroup SharedState
     */
    class BLACKMISC_EXPORT CDataLinkLocal : public QObject, public IDataLink
    {
        Q_OBJECT
        Q_INTERFACES(BlackMisc::SharedState::IDataLink)

    public:
        //! Constructor.
        CDataLinkLocal(QObject *parent = nullptr);

        //! Destructor.
        virtual ~CDataLinkLocal() override;

        virtual void publish(const CPassiveMutator *mutator) override;
        virtual void publish(const CActiveMutator *mutator) override;
        virtual void subscribe(const CPassiveObserver *observer) override;
        virtual void subscribe(const CActiveObserver *observer) override;

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
}

#endif
