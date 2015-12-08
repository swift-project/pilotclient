/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PLUGINSTORAGEPROVIDER_H
#define BLACKMISC_PLUGINSTORAGEPROVIDER_H

#include "blackmisc/variant.h"
#include <QObject>

namespace BlackMisc
{
    //! Interface for a plugin storage provider.
    //! It allows plugins to store any arbitrary data which can be packed into a \sa CVariant
    //! Every plugin shall have its own data area. This means mulitple plugins can store
    //! data under the same key without overwriting each other.
    class IPluginStorageProvider
    {
    public:
        //! Destructor
        virtual ~IPluginStorageProvider() {}

        //! Get plugin data stored for object and identified by key
        virtual CVariant getPluginData(const QObject *obj, const QString &key) const = 0;

        //! Store plugin data for object, identified by key and packed into value
        virtual void setPluginData(const QObject *obj, const QString &key, const CVariant &value) = 0;
    };

    //! Delegating class which can be directly used to access an \sa IPluginStorageProvider instance
    class CPluginStorageAware
    {
    protected:
        //! \copydoc IPluginStorageProvider::getPluginData
        virtual CVariant getPluginData(const QObject *obj, const QString &key) const
        {
            return m_pluginStorageProvider->getPluginData(obj, key);
        }

        //! \copydoc IOwnAircraftProvider::ownAircraft
        virtual void setPluginData(const QObject *obj, const QString &key, const CVariant &value)
        {
            m_pluginStorageProvider->setPluginData(obj, key, value);
        }

        //! Constructor
        CPluginStorageAware(IPluginStorageProvider *pluginStorageProvider) :
            m_pluginStorageProvider(pluginStorageProvider)
        {
            Q_ASSERT(pluginStorageProvider);
        }
        IPluginStorageProvider *m_pluginStorageProvider = nullptr; //!< access to object
    };
} // namespace

#endif
