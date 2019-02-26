/* Copyright (C) 2020
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "blackmisc/sharedstate/scalarjournal.h"
#include "blackmisc/sharedstate/datalink.h"

namespace BlackMisc
{
    namespace SharedState
    {
        void CGenericScalarJournal::initialize(IDataLink *dataLink)
        {
            dataLink->publish(m_mutator.data());
            dataLink->subscribe(m_observer.data());
            m_observer->setEventSubscription(CVariant::from(CAnyMatch()));
            m_mutator->postEvent(m_value);
        }

        CVariant CGenericScalarJournal::handleRequest(const CVariant &param)
        {
            Q_UNUSED(param)
            return m_value;
        }

        void CGenericScalarJournal::handleEvent(const CVariant &param)
        {
            m_value = param;
        }
    }
}
