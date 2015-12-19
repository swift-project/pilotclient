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

#include "blackgui/blackguiexport.h"
#include <QString>
#include <memory>

namespace BlackGui
{
    namespace Models
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
            virtual bool isValid() const = 0;

            //! Enabled?
            virtual bool isEnabled() const { return m_enabled && isValid(); }

            //! Enabled?
            void setEnabled(bool enable);

        protected:
            //! Standard string search supporting wildcard at begin and end: "*xyz", "abc*"
            bool stringMatchesFilterExpression(const QString &value, const QString &filter, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;

            //! Remove the * wildcards
            QString stripWildcard(const QString &value) const;

        private:
            bool m_enabled = true;
        };

        //! Model filter interface for those who can generate such a filter (e.g. a widget or dialog)
        template<class ContainerType> class IModelFilterProvider
        {
        public:
            //! Get the filter, this is the filter itself, not its widget or dialog
            virtual std::unique_ptr<IModelFilter<ContainerType>> createModelFilter() const = 0;
        };

    } // namespace
} // namespace
#endif // guard
