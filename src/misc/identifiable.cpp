// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/identifiable.h"

namespace swift::misc
{
    CIdentifiable::CIdentifiable(QObject *object) : m_identifier(object->objectName())
    {
        // if the object name changes we update our originator
        m_connection = QObject::connect(object, &QObject::objectNameChanged, [this, object]() {
            m_identifier = CIdentifier(object->objectName());
        });
    }

    CIdentifiable::~CIdentifiable()
    {
        QObject::disconnect(m_connection);
    }
} // ns
