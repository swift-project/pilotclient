// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/logcategorylist.h"

#include <QDBusMetaType>
#include <QList>
#include <QMetaObject>

#include "misc/containerbase.h"
#include "misc/iterator.h"
#include "misc/range.h"

struct QMetaObject;

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc, CLogCategory, CLogCategoryList)

namespace swift::misc
{
    void CLogCategoryList::appendCategoriesFromMetaObject(const QMetaObject &metaObject, const QMetaObject &super)
    {
        for (auto *meta = &metaObject; meta != &super; meta = meta->superClass()) { push_back(meta->className()); }
    }

    QStringList CLogCategoryList::toQStringList() const
    {
        return transform([](const CLogCategory &cat) { return cat.toQString(); });
    }

    QString CLogCategoryList::toQString(bool i18n) const { return convertToQString(i18n); }

    QString CLogCategoryList::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n); // log categories are always Latin-1
        return toQStringList().join("|");
    }

    CLogCategoryList CLogCategoryList::fromQStringList(const QStringList &stringList)
    {
        return makeRange(Iterators::makeTransformIterator(stringList.begin(),
                                                          [](const QString &str) { return CLogCategory { str }; }),
                         stringList.end());
    }

    CLogCategoryList CLogCategoryList::fromQString(const QString &string)
    {
        return fromQStringList(string.split("|", Qt::SkipEmptyParts));
    }

    bool CLogCategoryList::anyStartWith(const QString &prefix) const
    {
        return containsBy([=](const CLogCategory &cat) { return cat.startsWith(prefix); });
    }

    bool CLogCategoryList::anyEndWith(const QString &suffix) const
    {
        return containsBy([=](const CLogCategory &cat) { return cat.endsWith(suffix); });
    }

    void CLogCategoryList::registerMetadata()
    {
        qRegisterMetaType<CLogCategoryList>();
        qDBusRegisterMetaType<CLogCategoryList>();
        registerMetaValueType<CLogCategoryList>();
    }
} // namespace swift::misc
