/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "originatoraware.h"

namespace BlackMisc
{
    COriginator COriginatorAware::getCurrentTimestampOriginator() const
    {
        COriginator o(m_originator);
        o.setCurrentUtcTime();
        return o;
    }

    COriginatorAware::COriginatorAware(QObject *object) : m_originator(object->objectName())
    {
        // if the object name changes we update our origiginator
        this->m_originatorConnection = QObject::connect(object, &QObject::objectNameChanged, [this, object]()
        {
            this->m_originator = COriginator(object->objectName());
        });
    }

    COriginatorAware::~COriginatorAware()
    {
        QObject::disconnect(m_originatorConnection);
    }

} // ns
