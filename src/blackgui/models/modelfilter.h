/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_LISTMODELFILTER_H
#define BLACKGUI_MODELS_LISTMODELFILTER_H

#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/countrylist.h"
#include "blackmisc/identifierlist.h"
#include "blackmisc/namevariantpairlist.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/variant.h"

#include <QString>
#include <Qt>
#include <memory>

namespace BlackGui::Models
{
    //! Model filter interface
    template<class ContainerType> class IModelFilter
    {
    public:
        //! Constructor
        IModelFilter(bool enabled = true) : m_enabled(enabled) {}

        //! Destructor
        virtual ~IModelFilter() {}

        //! Used container data
        virtual ContainerType filter(const ContainerType &container) const = 0;

        //! Anything to do?
        bool isValid() const { return m_valid; }

        //! Enabled?
        virtual bool isEnabled() const { return m_enabled && isValid(); }

        //! Enabled?
        void setEnabled(bool enable);

        //! Return an implementation-specific value object representing the filter
        virtual BlackMisc::CVariant getAsValueObject() const { return {}; }

    protected:
        //! Standard string search supporting wildcard at begin and end: "*xyz", "abc*"
        bool stringMatchesFilterExpression(const QString &value, const QString &filter, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;

        //! Remove the * wildcards
        QString stripWildcard(const QString &value) const;

        bool m_valid = false;  //!< is filter valid?

    private:
        bool m_enabled = true; //!< is filter enabled?
    };

    //! Model filter interface for those who can generate such a filter (e.g. a widget or dialog)
    template<class ContainerType> class IModelFilterProvider
    {
    public:
        //! Get the filter, this is the filter itself, not its widget or dialog
        virtual std::unique_ptr<IModelFilter<ContainerType>> createModelFilter() const = 0;
    };
} // namespace
#endif // guard
