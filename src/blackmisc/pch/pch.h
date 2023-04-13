/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

/*                                         ---------
 *                                        ! WARNING !
 *      ---------------------------------------------------------------------------------
 * >>>> CHANGES IN ANY HEADERS INCLUDED HEREIN WILL TRIGGER A FULL REBUILD OF EVERYTHING! <<<<
 *      ---------------------------------------------------------------------------------
 */

#ifdef BLACKMISC_PCH_H
#    error "Don't #include this file"
#else
#    define BLACKMISC_PCH_H

#    ifdef __cplusplus

#        if defined(_MSC_FULL_VER) && _MSC_FULL_VER < 191000000
#            pragma warning(disable : 4503)
#        endif

#        include "blackmisc/collection.h"
#        include "blackmisc/containerbase.h"
#        include "blackmisc/dbus.h"
#        include "blackmisc/dictionary.h"
#        include "blackmisc/iterator.h"
#        include "blackmisc/json.h"
#        include "blackmisc/metaclass.h"
#        include "blackmisc/range.h"
#        include "blackmisc/sequence.h"
#        include "blackmisc/valueobject.h"
#        include "blackmisc/verify.h"
#        include "blackmisc/worker.h"

#        include <QByteArray>
#        include <QChar>
#        include <QCoreApplication>
#        include <QDBusArgument>
#        include <QDBusMetaType>
#        include <QDate>
#        include <QDateTime>
#        include <QDebug>
#        include <QDir>
#        include <QFile>
#        include <QFlag>
#        include <QFlags>
#        include <QFutureWatcher>
#        include <QGlobalStatic>
#        include <QHash>
#        include <QIODevice>
#        include <QIcon>
#        include <QJsonArray>
#        include <QJsonObject>
#        include <QJsonValue>
#        include <QJsonValueRef>
#        include <QLatin1Char>
#        include <QLatin1Literal>
#        include <QLatin1String>
#        include <QList>
#        include <QLoggingCategory>
#        include <QMap>
#        include <QMessageLogContext>
#        include <QMetaMethod>
#        include <QMetaObject>
#        include <QMetaType>
#        include <QMutex>
#        include <QMutexLocker>
#        include <QObject>
#        include <QPair>
#        include <QPixmap>
#        include <QReadLocker>
#        include <QReadWriteLock>
#        include <QScopedPointer>
#        include <QSet>
#        include <QSharedPointer>
#        include <QString>
#        include <QStringBuilder>
#        include <QStringList>
#        include <QStringRef>
#        include <QThread>
#        include <QTime>
#        include <QTimer>
#        include <QVariant>
#        include <QVariantList>
#        include <QVector>
#        include <QWeakPointer>
#        include <QWriteLocker>
#        include <QtGlobal>

#        include <algorithm>
#        include <array>
#        include <cctype>
#        include <cmath>
#        include <cstddef>
#        include <cstdio>
#        include <cstdlib>
#        include <cstring>
#        include <exception>
#        include <functional>
#        include <future>
#        include <initializer_list>
#        include <iostream>
#        include <iterator>
#        include <map>
#        include <memory>
#        include <numeric>
#        include <random>
#        include <set>
#        include <stdexcept>
#        include <string>
#        include <tuple>
#        include <type_traits>
#        include <typeindex>
#        include <unordered_map>
#        include <utility>
#        include <vector>

#    endif // __cplusplus
#endif // guard
