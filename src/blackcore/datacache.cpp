/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "datacache.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/identifier.h"
#include <QStandardPaths>
#include <QLockFile>

using namespace BlackMisc;

namespace BlackCore
{

    CDataCache::CDataCache() :
        CValueCache(CValueCache::Distributed)
    {
        if (! QDir::root().mkpath(persistentStore()))
        {
            CLogMessage(this).error("Failed to create directory %1") << persistentStore();
        }

        connect(this, &CValueCache::valuesChangedByLocal, this, &CDataCache::saveToStore);
        connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, [this] { loadFromStore(); });

        if (! QFile::exists(m_revisionFileName)) { QFile(m_revisionFileName).open(QFile::WriteOnly); }
        m_watcher.addPath(m_revisionFileName);
        loadFromStore();
    }

    CDataCache *CDataCache::instance()
    {
        static CDataCache cache;
        return &cache;
    }

    const QString &CDataCache::persistentStore()
    {
        static const QString dir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/org.swift-project/data/cache/core";
        return dir;
    }

    QString lockFileError(const QLockFile &lock)
    {
        switch (lock.error())
        {
        case QLockFile::NoError: return "No error";
        case QLockFile::PermissionError: return "Insufficient permission";
        case QLockFile::UnknownError: return "Unknown filesystem error";
        case QLockFile::LockFailedError:
        {
            QString hostname, appname;
            qint64 pid = 0;
            lock.getLockInfo(&pid, &hostname, &appname);
            return QString("Lock open in another process (%1 %2 on %3)").arg(hostname, QString::number(pid), appname);
        }
        default: return "Bad error number";
        }
    }

    void CDataCache::saveToStore(const BlackMisc::CVariantMap &values)
    {
        QMutexLocker lock(&m_mutex);

        QLockFile revisionFileLock(m_revisionFileName + ".lock");
        if (! revisionFileLock.lock())
        {
            CLogMessage(this).error("Failed to lock %1: %2") << m_revisionFileName << lockFileError(revisionFileLock);
            return;
        }

        loadFromStore(false, true); // last-minute check for remote changes before clobbering the revision file
        for (const auto &key : values.keys()) { m_deferredChanges.remove(key); } // ignore changes that we are about to overwrite

        QFile revisionFile(m_revisionFileName);
        if (! revisionFile.open(QFile::WriteOnly))
        {
            CLogMessage(this).error("Failed to open %1: %2") << m_revisionFileName << revisionFile.errorString();
            return;
        }
        m_revision = CIdentifier().toUuid();
        revisionFile.write(m_revision.toByteArray());

        saveToFiles(persistentStore(), values);
    }

    void CDataCache::loadFromStore(bool revLock, bool defer)
    {
        QMutexLocker lock(&m_mutex);

        QLockFile revisionFileLock(m_revisionFileName + ".lock");
        if (revLock && ! revisionFileLock.lock())
        {
            CLogMessage(this).error("Failed to lock %1: %2") << m_revisionFileName << lockFileError(revisionFileLock);
            return;
        }

        QFile revisionFile(m_revisionFileName);
        if (! revisionFile.exists())
        {
            return;
        }
        if (! revisionFile.open(QFile::ReadOnly))
        {
            CLogMessage(this).error("Failed to open %1: %2") << m_revisionFileName << revisionFile.errorString();
            return;
        }

        QUuid newRevision(revisionFile.readAll());
        if (m_revision != newRevision)
        {
            m_revision = newRevision;
            CVariantMap newValues;
            loadFromFiles(persistentStore(), newValues);
            m_deferredChanges.insert(newValues);
        }

        if (! (m_deferredChanges.isEmpty() || defer))
        {
            changeValuesFromRemote(m_deferredChanges, CIdentifier::anonymous());
            m_deferredChanges.clear();
        }
    }

}
