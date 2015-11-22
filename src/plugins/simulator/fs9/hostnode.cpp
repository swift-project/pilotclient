/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "hostnode.h"

namespace BlackSimPlugin
{
    namespace Fs9
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
            if (m_hostAddress)
                m_hostAddress->Release();

            m_hostAddress = nullptr;
        }
    }
}
