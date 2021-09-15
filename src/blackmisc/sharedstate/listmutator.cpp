/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "blackmisc/sharedstate/listmutator.h"
#include "blackmisc/sharedstate/datalink.h"

namespace BlackMisc::SharedState
{
    void CGenericListMutator::initialize(IDataLink *dataLink)
    {
        dataLink->publish(m_mutator.data());
    }

    void CGenericListMutator::addElement(const CVariant &value)
    {
        m_mutator->postEvent(value);
    }
}
