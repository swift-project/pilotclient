/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "identifiable.h"

namespace BlackMisc
{
    CIdentifier CIdentifiable::getCurrentTimestampIdentifier() const
    {
        CIdentifier o(m_identifier);
        o.setCurrentUtcTime();
        return o;
    }

    CIdentifiable::CIdentifiable(QObject *object) : m_identifier(object->objectName())
    {
        // if the object name changes we update our origiginator
        this->m_connection = QObject::connect(object, &QObject::objectNameChanged, [this, object]()
        {
            this->m_identifier = CIdentifier(object->objectName());
        });
    }

    CIdentifiable::~CIdentifiable()
    {
        QObject::disconnect(m_connection);
    }

} // ns
