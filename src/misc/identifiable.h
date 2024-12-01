// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_IDENTIFIABLE_H
#define SWIFT_MISC_IDENTIFIABLE_H

#include <QMetaObject>
#include <QObject>
#include <QString>

#include "misc/identifier.h"
#include "misc/mixin/mixincompare.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    /*!
     * Base class with a member CIdentifier to be inherited by a class which has an identity in the environment of
     * modular distributed swift processes
     */
    class SWIFT_MISC_EXPORT CIdentifiable
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
} // namespace swift::misc

#endif // SWIFT_MISC_IDENTIFIABLE_H
