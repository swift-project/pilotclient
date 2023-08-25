// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "hostnode.h"
#include "directplayutils.h"

namespace BlackSimPlugin::Fs9
{
    CHostNode::CHostNode()
    {
    }

    CHostNode::CHostNode(const CHostNode &other)
        : m_appDesc(other.m_appDesc), m_sessionName(other.m_sessionName)
    {
        other.m_hostAddress->Duplicate(&m_hostAddress);
    }

    CHostNode::~CHostNode()
    {
        SafeRelease(m_hostAddress);
    }

    CHostNode &CHostNode::operator=(const CHostNode &other)
    {
        // check for self-assignment
        if (&other == this) { return *this; }

        m_appDesc = other.m_appDesc;
        m_sessionName = other.m_sessionName;
        other.m_hostAddress->Duplicate(&m_hostAddress);
        return *this;
    }
}
