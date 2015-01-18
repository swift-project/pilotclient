/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_LISTMODELFILTER_H
#define BLACKGUI_LISTMODELFILTER_H

#include <QString>

namespace BlackGui
{
    namespace Models
    {
        //! Model filter interface
        template<class ContainerType> class IModelFilter
        {

        public:
            //! Destructor
            virtual ~IModelFilter() {}

            //! Used container data
            virtual ContainerType filter(const ContainerType &container) const = 0;

            //! Anything to do?
            virtual bool isValid() const = 0;

        protected:

            //! Standard string search supporting wildcard at begin and end: "*xyz", "abc*"
            bool stringMatchesFilterExpression(const QString &value, const QString &filter, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;

            //! Remove the * wildcards
            QString stripWildcard(const QString &value) const;

        };

    } // namespace
} // namespace
#endif // guard
