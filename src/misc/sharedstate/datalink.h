// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SHAREDSTATE_DATALINK_H
#define SWIFT_MISC_SHAREDSTATE_DATALINK_H

#include "misc/swiftmiscexport.h"
#include "misc/promise.h"
#include "misc/variant.h"
#include <QObject>
#include <QMetaObject>
#include <QMetaClassInfo>
#include <QStringBuilder>

/*!
 * \defgroup shared_state Shared state
 * Utilities for sharing state between multiple objects
 */

/*!
 * Put this macro in the private section of a class to set the channel of its child endpoints.
 * \ingroup shared_state
 */
#define BLACK_SHARED_STATE_CHANNEL(ID) Q_CLASSINFO("SharedStateChannel", ID)

namespace swift::misc::shared_state
{
    class IDataLink;
    class CPassiveMutator;
    class CActiveMutator;
    class CPassiveObserver;
    class CActiveObserver;

    /*!
     * Observe the connection state of an IDataLink.
     * \ingroup shared_state
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
     * \ingroup shared_state
     */
    class SWIFT_MISC_EXPORT IDataLink
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

Q_DECLARE_INTERFACE(swift::misc::shared_state::IDataLink, "swift::misc::shared_state::IDataLink")
Q_DECLARE_METATYPE(swift::misc::CPromise<swift::misc::CVariant>)

#endif
