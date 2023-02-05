/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SHAREDSTATE_DATALINK_H
#define BLACKMISC_SHAREDSTATE_DATALINK_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/promise.h"
#include "blackmisc/variant.h"
#include <QObject>
#include <QMetaObject>
#include <QMetaClassInfo>
#include <QStringBuilder>

/*!
 * \defgroup SharedState Shared state
 * Utilities for sharing state between multiple objects
 */

/*!
 * Put this macro in the private section of a class to set the channel of its child endpoints.
 * \ingroup SharedState
 */
#define BLACK_SHARED_STATE_CHANNEL(ID) Q_CLASSINFO("SharedStateChannel", ID)

namespace BlackMisc::SharedState
{
    class IDataLink;
    class CPassiveMutator;
    class CActiveMutator;
    class CPassiveObserver;
    class CActiveObserver;

    /*!
     * Observe the connection state of an IDataLink.
     * \ingroup SharedState
     */
    class CDataLinkConnectionWatcher : public QObject
    {
        Q_OBJECT

    public:
        //! True if connected to the transport layer.
        bool isConnected() const { return m_connected; }

    signals:
        //! Connection established.
        void connected();

        //! Connection dropped.
        void disconnected();

    private:
        friend class IDataLink;
        CDataLinkConnectionWatcher() = default;
        void setStatus(bool connected);
        bool m_connected = false;
    };

    /*!
     * Interface that provides a transport mechanism for sharing state.
     * \ingroup SharedState
     */
    class BLACKMISC_EXPORT IDataLink
    {
    public:
        //! Constructor.
        IDataLink();

        //! Destructor.
        virtual ~IDataLink() = 0;

        //! Get a connection status watcher.
        CDataLinkConnectionWatcher *watcher() { return &m_watcher; }

        //! @{
        //! Register a mutator with this transport mechanism.
        virtual void publish(const CPassiveMutator *mutator) = 0;
        virtual void publish(const CActiveMutator *mutator) = 0;
        //! @}

        //! @{
        //! Register an observer with this transport mechanism.
        virtual void subscribe(const CPassiveObserver *observer) = 0;
        virtual void subscribe(const CActiveObserver *observer) = 0;
        //! @}

    protected:
        //! Set the connection status visible through the watcher.
        void setConnectionStatus(bool connected) { m_watcher.setStatus(connected); }

        //! Get the channel name for child endpoints of the given object.
        static QString getChannelName(const QObject *object);

    private:
        CDataLinkConnectionWatcher m_watcher;
    };
}

Q_DECLARE_INTERFACE(BlackMisc::SharedState::IDataLink, "BlackMisc::SharedState::IDataLink")
Q_DECLARE_METATYPE(BlackMisc::CPromise<BlackMisc::CVariant>)

#endif
