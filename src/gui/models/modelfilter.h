// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_LISTMODELFILTER_H
#define SWIFT_GUI_MODELS_LISTMODELFILTER_H

#include <memory>

#include <QString>
#include <Qt>

#include "misc/aviation/aircrafticaocodelist.h"
#include "misc/aviation/airlineicaocodelist.h"
#include "misc/aviation/airportlist.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/liverylist.h"
#include "misc/countrylist.h"
#include "misc/identifierlist.h"
#include "misc/namevariantpairlist.h"
#include "misc/network/clientlist.h"
#include "misc/network/serverlist.h"
#include "misc/network/textmessagelist.h"
#include "misc/network/userlist.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/distributorlist.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/statusmessagelist.h"
#include "misc/variant.h"

namespace swift::gui::models
{
    //! Model filter interface
    template <class ContainerType>
    class IModelFilter
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
        virtual swift::misc::CVariant getAsValueObject() const { return {}; }

    protected:
        //! Standard string search supporting wildcard at begin and end: "*xyz", "abc*"
        bool stringMatchesFilterExpression(const QString &value, const QString &filter,
                                           Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;

        //! Remove the * wildcards
        QString stripWildcard(const QString &value) const;

        bool m_valid = false; //!< is filter valid?

    private:
        bool m_enabled = true; //!< is filter enabled?
    };

    //! Model filter interface for those who can generate such a filter (e.g. a widget or dialog)
    template <class ContainerType>
    class IModelFilterProvider
    {
    public:
        //! Get the filter, this is the filter itself, not its widget or dialog
        virtual std::unique_ptr<IModelFilter<ContainerType>> createModelFilter() const = 0;
    };
} // namespace swift::gui::models
#endif // guard
