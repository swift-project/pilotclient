/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
        if(&other == this) { return *this; }

        m_appDesc = other.m_appDesc;
        m_sessionName = other.m_sessionName;
        other.m_hostAddress->Duplicate(&m_hostAddress);
        return *this;
    }
}
