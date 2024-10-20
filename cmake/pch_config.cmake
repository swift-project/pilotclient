# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

# This file contains definitions for all PCHs.
# The definitions are collected because targets might reuse headers from another target and also adds own headers.
# For example "core" uses the same PCH as "misc" but with additional headers included.

set(SWIFT_MISC_PCH
    [["blackmisc/collection.h"]]
    [["blackmisc/containerbase.h"]]
    [["blackmisc/dbus.h"]]
    [["blackmisc/dictionary.h"]]
    [["blackmisc/iterator.h"]]
    [["blackmisc/json.h"]]
    [["blackmisc/metaclass.h"]]
    [["blackmisc/range.h"]]
    [["blackmisc/sequence.h"]]
    [["blackmisc/valueobject.h"]]
    [["blackmisc/verify.h"]]
    [["blackmisc/worker.h"]]

    <QByteArray>
    <QChar>
    <QCoreApplication>
    <QDBusArgument>
    <QDBusMetaType>
    <QDate>
    <QDateTime>
    <QDebug>
    <QDir>
    <QFile>
    <QFlag>
    <QFlags>
    <QFutureWatcher>
    <QGlobalStatic>
    <QHash>
    <QIODevice>
    <QIcon>
    <QJsonArray>
    <QJsonObject>
    <QJsonValue>
    <QJsonValueRef>
    <QLatin1Char>
    <QLatin1String>
    <QList>
    <QLoggingCategory>
    <QMap>
    <QMessageLogContext>
    <QMetaMethod>
    <QMetaObject>
    <QMetaType>
    <QMutex>
    <QMutexLocker>
    <QObject>
    <QPair>
    <QPixmap>
    <QReadLocker>
    <QReadWriteLock>
    <QScopedPointer>
    <QSet>
    <QSharedPointer>
    <QString>
    <QStringBuilder>
    <QStringList>
    <QStringRef>
    <QThread>
    <QTime>
    <QTimer>
    <QVariant>
    <QVariantList>
    <QVector>
    <QWeakPointer>
    <QWriteLocker>
    <QtGlobal>

    <algorithm>
    <array>
    <cctype>
    <cmath>
    <cstddef>
    <cstdio>
    <cstdlib>
    <cstring>
    <exception>
    <functional>
    <future>
    <initializer_list>
    <iostream>
    <iterator>
    <map>
    <memory>
    <numeric>
    <random>
    <set>
    <stdexcept>
    <string>
    <tuple>
    <type_traits>
    <typeindex>
    <unordered_map>
    <utility>
    <vector>
)

set(SWIFT_CORE_PCH
    <QDBusAbstractInterface>
    <QDBusConnection>
    <QDBusConnectionInterface>
    <QDBusError>
    <QDBusMessage>
    <QDBusPendingCall>
    <QDBusPendingReply>
    <QDBusReply>
    <QDBusServer>
    <QDBusServiceWatcher>
    <QNetworkAccessManager>
    <QNetworkCookieJar>
    <QNetworkInterface>
    <QNetworkReply>
    <QNetworkRequest>
    <QUrl>
)

set(SWIFT_GUI_PCH
    [["blackcore/context/contextapplication.h"]]
    [["blackcore/context/contextaudio.h"]]
    [["blackcore/context/contextnetwork.h"]]
    [["blackcore/context/contextownaircraft.h"]]
    [["blackcore/context/contextsimulator.h"]]

    <QAbstractButton>
    <QAbstractItemModel>
    <QAbstractItemView>
    <QAbstractScrollArea>
    <QAbstractTableModel>
    <QComboBox>
    <QDialog>
    <QDockWidget>
    <QDrag>
    <QDragEnterEvent>
    <QDragLeaveEvent>
    <QDragMoveEvent>
    <QDropEvent>
    <QFrame>
    <QImage>
    <QLabel>
    <QLayout>
    <QLayoutItem>
    <QLineEdit>
    <QMenu>
    <QMenuBar>
    <QMessageBox>
    <QPoint>
    <QPushButton>
    <QRadioButton>
    <QRect>
    <QSize>
    <QSlider>
    <QSplitter>
    <QStackedWidget>
    <QStyle>
    <QTabBar>
    <QTabWidget>
    <QTableView>
    <QTextEdit>
    <QValidator>
    <QWidget>
    <QWindow>
)
