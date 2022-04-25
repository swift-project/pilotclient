/* Copyright (C) 2014
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/logcategorylist.h"
#include "blackmisc/containerbase.h"
#include "blackmisc/iterator.h"
#include "blackmisc/range.h"

#include <QDBusMetaType>
#include <QList>
#include <QMetaObject>

struct QMetaObject;

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc, CLogCategory, CLogCategoryList)

namespace BlackMisc
{
    void CLogCategoryList::appendCategoriesFromMetaObject(const QMetaObject &metaObject, const QMetaObject &super)
    {
        for (auto *meta = &metaObject; meta != &super; meta = meta->superClass())
        {
            push_back(meta->className());
        }
    }

    QStringList CLogCategoryList::toQStringList() const
    {
        return transform([](const CLogCategory &cat) { return cat.toQString(); });
    }

    QString CLogCategoryList::toQString(bool i18n) const
    {
        return convertToQString(i18n);
    }

    QString CLogCategoryList::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n); // log categories are always Latin-1
        return toQStringList().join("|");
    }

    CLogCategoryList CLogCategoryList::fromQStringList(const QStringList &stringList)
    {
        return makeRange(Iterators::makeTransformIterator(stringList.begin(), [](const QString &str) { return CLogCategory{str}; }), stringList.end());
    }

    CLogCategoryList CLogCategoryList::fromQString(const QString &string)
    {
        return fromQStringList(string.split("|", Qt::SkipEmptyParts));
    }

    bool CLogCategoryList::anyStartWith(const QString &prefix) const
    {
        return containsBy([ = ](const CLogCategory &cat) { return cat.startsWith(prefix); });
    }

    bool CLogCategoryList::anyEndWith(const QString &suffix) const
    {
        return containsBy([ = ](const CLogCategory &cat) { return cat.endsWith(suffix); });
    }

    void CLogCategoryList::registerMetadata()
    {
        qRegisterMetaType<CLogCategoryList>();
        qDBusRegisterMetaType<CLogCategoryList>();
        qRegisterMetaTypeStreamOperators<CLogCategoryList>();
        registerMetaValueType<CLogCategoryList>();
    }
}
