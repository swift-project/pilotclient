// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SIMPLUGIN_FS9_HOST_NODE_H
#define SWIFT_SIMPLUGIN_FS9_HOST_NODE_H

#include <QString>
#include <dplay8.h>

//! \file

namespace swift::simplugin::fs9
{
    //! Class representing a enumerated host node
    struct CHostNode
    {
        //! Constructor
        CHostNode();

        //! Copy constructor
        CHostNode(const CHostNode &other);

        //! Destructor
        ~CHostNode();

        //! Returns the hosts application description
        DPN_APPLICATION_DESC getApplicationDesc() const { return m_appDesc; }

        //! Set the hosts application description
        void setApplicationDesc(const DPN_APPLICATION_DESC &appDesc) { m_appDesc = appDesc; }

        //! Returns a pointer to the hosts address
        IDirectPlay8Address **getHostAddressPtr() { return &m_hostAddress; }

        //! Returns the hosts address
        IDirectPlay8Address *getHostAddress() { return m_hostAddress; }

        //! Sets hosts address
        void setHostAddress(IDirectPlay8Address *address) { m_hostAddress = address; }

        //! Returns the sessions name
        QString getSessionName() const { return m_sessionName; }

        //! Set the session name
        void setSessionName(const QString &name) { m_sessionName = name; }

        //! Copy assignment operator
        CHostNode &operator=(const CHostNode &other);

    private:
        IDirectPlay8Address *m_hostAddress = nullptr;
        DPN_APPLICATION_DESC m_appDesc;
        QString m_sessionName;
    };
}

#endif // guard
