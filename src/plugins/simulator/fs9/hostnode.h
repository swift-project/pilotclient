/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKSIMPLUGIN_FS9_HOST_NODE_H
#define BLACKSIMPLUGIN_FS9_HOST_NODE_H

#include <QString>
#include <dplay8.h>

//! \file

namespace BlackSimPlugin::Fs9
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
        IDirectPlay8Address  *m_hostAddress = nullptr;
        DPN_APPLICATION_DESC  m_appDesc;
        QString               m_sessionName;
    };
}

#endif //guard
