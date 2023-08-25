// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_NETWORK_ROLE_H
#define BLACKMISC_NETWORK_ROLE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/db/datastore.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/valueobject.h"

#include <QJsonObject>
#include <QMetaType>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Network, CRole)

namespace BlackMisc::Network
{
    /*!
     * Role
     */
    class BLACKMISC_EXPORT CRole :
        public CValueObject<CRole>,
        public Db::IDatastoreObjectWithIntegerKey
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

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

        //! Role from DB JSON
        static CRole fromDatabaseJson(const QJsonObject &json);

    private:
        QString m_name;
        QString m_description;

        BLACK_METACLASS(
            CRole,
            BLACK_METAMEMBER(dbKey),
            BLACK_METAMEMBER(name),
            BLACK_METAMEMBER(description)
        );
    };

} // ns

Q_DECLARE_METATYPE(BlackMisc::Network::CRole)

#endif // guard
