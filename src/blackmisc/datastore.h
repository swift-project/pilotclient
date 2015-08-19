/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DATASTORE_H
#define BLACKMISC_DATASTORE_H

#include "blackmiscexport.h"
#include "timestampbased.h"
#include "propertyindex.h"
#include "variant.h"

namespace BlackMisc
{
    /*!
     * Class from which a derived class can inherit datastore-related functions.
     */
    class BLACKMISC_EXPORT IDatastoreObjectWithIntegerKey : public ITimestampBased
    {
    public:
        //! Property index
        enum ColumnIndex
        {
            IndexDbIntergerKey = CPropertyIndex::GlobalIndexIDatastoreInteger
        };

        //! Get DB key.
        int getDbKey() const { return m_dbKey; }

        //! Set the DB key
        void setDbKey(int key) { m_dbKey = key; }

        //! Has valid DB key
        bool hasValidDbKey() const { return m_dbKey >= 0; }

    protected:
        //! \copydoc CValueObject::propertyByIndex
        CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

        //! \copydoc CValueObject::setPropertyByIndex
        void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

        //! Can given index be handled?
        static bool canHandleIndex(const BlackMisc::CPropertyIndex &index);

        int m_dbKey = -1; //!< key
    };

    /*!
     * Class from which a derived class can inherit datastore-related functions.
     */
    class BLACKMISC_EXPORT IDatastoreObjectWithStringKey : public ITimestampBased
    {
    public:
        //! Property index
        enum ColumnIndex
        {
            IndexDbStringKey = CPropertyIndex::GlobalIndexIDatastoreString
        };

        //! Get DB key.
        const QString &getDbKey() const { return m_dbKey; }

        //! Set the DB key
        void setDbKey(const QString &key) { m_dbKey = key.trimmed().toUpper(); }

        //! Has valid DB key
        bool hasValidDbKey() const { return !m_dbKey.isEmpty(); }

    protected:
        //! \copydoc CValueObject::propertyByIndex
        CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

        //! \copydoc CValueObject::setPropertyByIndex
        void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

        //! Can given index be handled
        static bool canHandleIndex(const BlackMisc::CPropertyIndex &index);

        QString m_dbKey; //!< key
    };

} // namespace

#endif // guard
