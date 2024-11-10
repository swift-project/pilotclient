// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_ROLE_H
#define SWIFT_MISC_NETWORK_ROLE_H

#include "misc/swiftmiscexport.h"
#include "misc/db/datastore.h"
#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/valueobject.h"

#include <QJsonObject>
#include <QMetaType>
#include <QString>

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::network, CRole)

namespace swift::misc::network
{
    /*!
     * Role
     */
    class SWIFT_MISC_EXPORT CRole :
        public CValueObject<CRole>,
        public db::IDatastoreObjectWithIntegerKey
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexName = CPropertyIndexRef::GlobalIndexCRole,
            IndexDescription
        };

        //! Constructor
        CRole() = default;

        //! Constructor
        CRole(const QString &name, const QString &description);

        //! Name
        const QString &getName() const { return m_name; }

        //! Name
        void setName(const QString &name) { m_name = name.trimmed().toUpper(); }

        //! Description
        const QString &getDescription() const { return m_description; }

        //! Description
        void setDescription(const QString &description) { m_description = description.trimmed(); }

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! Role from DB JSON
        static CRole fromDatabaseJson(const QJsonObject &json);

    private:
        QString m_name;
        QString m_description;

        SWIFT_METACLASS(
            CRole,
            SWIFT_METAMEMBER(dbKey),
            SWIFT_METAMEMBER(name),
            SWIFT_METAMEMBER(description));
    };

} // ns

Q_DECLARE_METATYPE(swift::misc::network::CRole)

#endif // guard
