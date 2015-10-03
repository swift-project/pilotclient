/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DATABASEOBJECTLIST_H
#define BLACKMISC_DATABASEOBJECTLIST_H

#include "blackmisc/timestampobjectlist.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include <QList>
#include <QMap>

namespace BlackMisc
{
    //! List of objects read from database.
    //! Such objects should implement \sa ITimestampBased and \sa IDatastoreObjectWithIntegerKey
    template<class OBJ, class CONTAINER> class IDatastoreObjectListWithIntegerKey : public ITimestampObjectList<OBJ, CONTAINER>
    {
    public:
        //! Object with key, notFound otherwise
        OBJ findByKey(int key, const OBJ &notFound = OBJ()) const;

        //! Sort by timestamp
        void sortByKey();

        //! From DB JSON
        static CONTAINER fromDatabaseJson(const QJsonArray &array);

    protected:
        //! Constructor
        IDatastoreObjectListWithIntegerKey();
    };

    //! List of objects read from database.
    //! Such objects should implement \sa ITimestampBased and \sa IDatastoreObjectWithString
    template<class OBJ, class CONTAINER> class IDatastoreObjectListWithStringKey : public ITimestampObjectList<OBJ, CONTAINER>
    {
    public:
        //! Object with key, not found otherwise
        OBJ findByKey(const QString &key, const OBJ &notFound = OBJ()) const;

        //! Sort by timestamp
        void sortByKey();

        //! All keys as string list
        QStringList toDbKeyList() const;

        //! From DB JSON
        static CONTAINER fromDatabaseJson(const QJsonArray &array);

    protected:
        //! Constructor
        IDatastoreObjectListWithStringKey();
    };

    //! \cond PRIVATE
    namespace Aviation
    {
        class CLivery;
        class CLiveryList;
    }

    namespace Simulation
    {
        class CDistributor;
        class CDistributorList;
        class CAircraftModel;
        class CAircraftModelList;
    }

    extern template class BLACKMISC_EXPORT_TEMPLATE IDatastoreObjectListWithIntegerKey<BlackMisc::Aviation::CLivery, BlackMisc::Aviation::CLiveryList>;
    extern template class BLACKMISC_EXPORT_TEMPLATE IDatastoreObjectListWithIntegerKey<BlackMisc::Aviation::CAircraftIcaoCode, BlackMisc::Aviation::CAircraftIcaoCodeList>;
    extern template class BLACKMISC_EXPORT_TEMPLATE IDatastoreObjectListWithIntegerKey<BlackMisc::Aviation::CAirlineIcaoCode, BlackMisc::Aviation::CAirlineIcaoCodeList>;
    extern template class BLACKMISC_EXPORT_TEMPLATE IDatastoreObjectListWithIntegerKey<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList>;
    extern template class BLACKMISC_EXPORT_TEMPLATE IDatastoreObjectListWithStringKey<BlackMisc::Simulation::CDistributor, BlackMisc::Simulation::CDistributorList>;
    extern template class BLACKMISC_EXPORT_TEMPLATE IDatastoreObjectListWithStringKey<BlackMisc::CCountry, BlackMisc::CCountryList>;
    //! \endcond

} //namespace

#endif //guard
