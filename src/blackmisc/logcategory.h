/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_LOGCATEGORY_H
#define BLACKMISC_LOGCATEGORY_H

#include "blackmisc/metaclass.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/logcategories.h"

#include <QList>
#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    /*!
     * A log category is an arbitrary string tag which can be attached to log messages.
     *
     * A log handler can filter messages based on their categories.
     * \see CLogCategories
     */
    class BLACKMISC_EXPORT CLogCategory : public CValueObject<CLogCategory>
    {
    public:
        //! Constructor.
        CLogCategory() = default;

        //! Constructor.
        CLogCategory(const QString &categoryString) : m_string(categoryString) {}

        //! Constructor.
        CLogCategory(const char *categoryString) : m_string(categoryString) {}

        //! Returns true if the category string starts with the given prefix.
        bool startsWith(const QString &prefix) const { return m_string.startsWith(prefix); }

        //! Returns true if the category string ends with the given suffix.
        bool endsWith(const QString &suffix) const { return m_string.endsWith(suffix); }

        //! Returns true if the category string contains the given substring.
        bool contains(const QString &substring) const { return m_string.contains(substring); }

        //! \copydoc BlackMisc::Mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

    private:
        QString m_string;

        BLACK_METACLASS(
            CLogCategory,
            BLACK_METAMEMBER(string)
        );
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::CLogCategory)

#endif
