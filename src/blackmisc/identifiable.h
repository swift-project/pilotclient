// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_IDENTIFIABLE_H
#define BLACKMISC_IDENTIFIABLE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/identifier.h"

#include <QMetaObject>
#include <QObject>
#include <QString>

namespace BlackMisc
{
    /*!
     * Base class with a member CIdentifier to be inherited by a class which has an identity in the environment of modular distributed swift processes
     */
    class BLACKMISC_EXPORT CIdentifiable
    {
    public:
        //! Get identifier
        const CIdentifier &identifier() const { return m_identifier; }

        //! Set identifier, allows to set an external identifier
        void setIdentifier(const CIdentifier &identifier) { m_identifier = identifier; }

        //! My identifier?
        bool isMyIdentifier(const CIdentifier &otherIdentifier) const { return m_identifier == otherIdentifier; }

    protected:
        //! Use literal based object name
        CIdentifiable(const QString &objectName) : m_identifier(objectName) {}

        //! Connect with QObject providing the name
        CIdentifiable(QObject *nameProvider);

        //! Construct without a name
        CIdentifiable() {}

        //! Destructor
        ~CIdentifiable();

        //! Copy constructor
        CIdentifiable(const CIdentifiable &) = default;

        //! Copy assignment operator
        CIdentifiable &operator=(const CIdentifiable &) = default;

    private:
        CIdentifier m_identifier;
        QMetaObject::Connection m_connection;
    };
} // namespace

#endif // guard
