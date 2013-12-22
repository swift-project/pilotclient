/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKCORE_ATCLISTMGR_H
#define BLACKCORE_ATCLISTMGR_H

#include "blackmisc/atclist.h"
#include <QObject>

namespace BlackCore
{

    /*!
     * Abstract base class that manages and provides access to a list of available ATC stations.
     */
    class IAtcListManager : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(BlackMisc::CAtcList list READ getList NOTIFY listChanged)

    public:
        /*!
         * Virtual destructor.
         */
        virtual ~IAtcListManager() {}

        /*!
         * Immutable getter.
         * \return
         */
        virtual const BlackMisc::CAtcList &getList() const = 0;

    signals:
        /*!
         * Emitted whenever there is a change in the ATC list.
         * \param list The new list
         */
        void listChanged(const BlackMisc::CAtcList &list);
    };

    /*!
     * Concrete ATC list manager. Implementation of IAtcListManager.
     *
     * Has a dependency on INetwork. An INetwork must be available through the IContext singleton.
     */
    class CAtcListManager : public IAtcListManager
    {
        Q_OBJECT

    public:
        /*!
         * Constructor.
         * \param context
         */
        CAtcListManager(BlackMisc::IContext &context);

        virtual const BlackMisc::CAtcList& getList() const { return m_list; }

    public slots:
        /*!
         * CAtcListManager is responsible for connecting these slots.
         * \{
         */
        void update(const QString& callsign, const BlackMisc::PhysicalQuantities::CFrequency& freq,
            const BlackMisc::Geo::CCoordinateGeodetic& pos, const BlackMisc::PhysicalQuantities::CLength& range);
        void remove(const QString& callsign);
        void clear();
        /*! \} */

    private:
        BlackMisc::CAtcList m_list;
    };

}//namespace BlackCore

#endif //BLACKCORE_ATCLISTMGR_H