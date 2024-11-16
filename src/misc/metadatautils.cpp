// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/metadatautils.h"

#include <QMetaObject>
#include <QMetaType>
#include <QObject>
#include <QTextStream>

namespace swift::misc
{
    void displayAllUserMetatypesTypes(QTextStream &out)
    {
        out << getAllUserMetatypesTypes();
    }

    QString getAllUserMetatypesTypes(const QString &separator)
    {
        int fails = 0;
        QString meta;
        for (int mt = QMetaType::User; mt < QMetaType::User + 1000; mt++)
        {
            if (!QMetaType::isRegistered(mt))
            {
                fails++;
                // normally a consecutive order of metatypes, we allow a space before we break
                if (fails > 3) { break; }
                continue;
            }
            QMetaType metaType(mt);
            meta = meta.append("type: ").append(QString::number(mt)).append(" name: ").append(QMetaType::typeName(mt)).append(" | ").append(QString::number(QMetaType::sizeOf(mt))).append(" / ").append(QString::number(swift::misc::heapSizeOf(metaType))).append(separator);
        }
        return meta;
    }

    QString className(const QObject *object)
    {
        if (!object) { return "nullptr"; }
        return object->metaObject()->className();
    }

    QString classNameShort(const QObject *object)
    {
        if (!object) { return "nullptr"; }
        const QString fn = object->metaObject()->className();
        if (fn.contains("::"))
        {
            const int index = fn.lastIndexOf("::");
            if (fn.length() > index + 3)
            {
                return fn.mid(index + 2);
            }
        }
        return fn;
    }

#ifdef Q_CC_MSVC
#    include <crtdbg.h>

    size_t heapSizeOf(const QMetaType &metaType)
    {
        metaType.destroy(metaType.create()); // ignore one-off allocations of a class being instantiated for the first time
        _CrtMemState oldState, newState, diff;
        oldState.lTotalCount = newState.lTotalCount = diff.lTotalCount = 0; // avoid compiler warning
        diff.lSizes[_NORMAL_BLOCK] = 0;
        _CrtMemCheckpoint(&oldState);
        void *p = metaType.create();
        _CrtMemCheckpoint(&newState);
        metaType.destroy(p);
        _CrtMemDifference(&diff, &oldState, &newState);
        return diff.lSizes[_NORMAL_BLOCK];
    }

    size_t heapSizeOf(const QMetaObject &metaObject)
    {
        delete metaObject.newInstance(); // ignore one-off allocations of a class being instantiated for the first time
        _CrtMemState oldState, newState, diff;
        oldState.lTotalCount = newState.lTotalCount = diff.lTotalCount = 0; // avoid compiler warning
        diff.lSizes[_NORMAL_BLOCK] = 0;
        _CrtMemCheckpoint(&oldState);
        QObject *obj = metaObject.newInstance();
        _CrtMemCheckpoint(&newState);
        delete obj;
        _CrtMemDifference(&diff, &oldState, &newState);
        return diff.lSizes[_NORMAL_BLOCK];
    }

#else //! Q_CC_MSVC

    size_t heapSizeOf(const QMetaType &)
    {
        // qDebug() << "heapSizeOf not supported by your compiler toolchain";
        return 0;
    }

    size_t heapSizeOf(const QMetaObject &)
    {
        // qDebug() << "heapSizeOf not supported by your compiler toolchain";
        return 0;
    }

#endif //! Q_CC_MSVC

} // namespace swift::misc
