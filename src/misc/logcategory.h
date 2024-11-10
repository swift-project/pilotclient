// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_LOGCATEGORY_H
#define SWIFT_MISC_LOGCATEGORY_H

#include "misc/metaclass.h"
#include "misc/valueobject.h"
#include "misc/swiftmiscexport.h"
#include "misc/logcategories.h"

#include <QList>
#include <QMetaType>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(swift::misc, CLogCategory)

namespace swift::misc
{
    /*!
     * A log category is an arbitrary string tag which can be attached to log messages.
     *
     * A log handler can filter messages based on their categories.
     * \see CLogCategories
     */
    class SWIFT_MISC_EXPORT CLogCategory : public CValueObject<CLogCategory>
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

        //! \copydoc swift::misc::mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

    private:
        QString m_string;

        BLACK_METACLASS(
            CLogCategory,
            BLACK_METAMEMBER(string)
        );
    };
} // ns

Q_DECLARE_METATYPE(swift::misc::CLogCategory)

#endif
