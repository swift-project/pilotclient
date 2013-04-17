/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKCORE_PLUGINMGR_H
#define BLACKCORE_PLUGINMGR_H

#include <QPluginLoader>
#include <QVector>
#include <QSharedPointer>

namespace BlackMisc
{
    class IPlugin;
    class IPluginFactory;
}

namespace BlackCore
{

    /*!
        Plugin manager.
        Loads plugins and allows them to be queried and instantiated.
    */
    class CPluginManager
    {
    public:
        /*!
            Destructor.
        */
        virtual ~CPluginManager();

        /*!
            Attempt to load all plugins found at the given path.
            \param pluginsPath
        */
        void loadAllPlugins(const QString &pluginsPath);

        /*!
            Return the total number of plugins loaded so far.
            \return
        */
        size_t getPluginCount() const
        {
            return m_plugins.size();
        }

        /*!
            Return the name of a plugin.
            \param index the plugin's index in the vector of plugins.
            \return
        */
        const QString getName(size_t index) const;

        /*!
            Return the description of a plugin.
            \param index the plugin's index in the vector of plugins.
            \return
        */
        const QString getDescription(size_t index) const;

        /*!
            Construct a plugin.
            \param index the plugin's index in the vector of plugins.
            \return a pointer to the newly created plugin.
            \warning You must release this pointer with IPluginFactory::destroy().
        */
        BlackMisc::IPlugin *constructPlugin(size_t index);

        /*!
            Direct access to the factory. You don't usually need this.
            \param index
            \return
        */
        BlackMisc::IPluginFactory *getFactory(size_t index)
        {
            return const_cast<BlackMisc::IPluginFactory*>(static_cast<const CPluginManager*>(this)->getFactory(index));
        }

        /*!
            Direct access to the factory. You don't usually need this.
            \param index
            \return
        */
        const BlackMisc::IPluginFactory *getFactory(size_t index) const
        {
            return getEntry(index).factory;
        }

        /*!
            Direct access to the QPluginLoader. You don't usually need this.
            \param index
            \return
        */
        QPluginLoader *getLoader(size_t index)
        {
            return const_cast<QPluginLoader*>(static_cast<const CPluginManager*>(this)->getLoader(index));
        }

        /*!
            Direct access to the QPluginLoader. You don't usually need this.
            \param index
            \return
        */
        const QPluginLoader *getLoader(size_t index) const
        {
            return getEntry(index).loader.data();
        }

    private:
        class PluginEntry
        {
        public:
            PluginEntry() : factory(0) {}
            BlackMisc::IPluginFactory *factory;
            QSharedPointer<QPluginLoader> loader;
        };

        QVector<PluginEntry> m_plugins;

        PluginEntry &getEntry(size_t index)
        {
            return const_cast<PluginEntry&>(static_cast<const CPluginManager*>(this)->getEntry(index));
        }

        const PluginEntry &getEntry(size_t index) const;
    };

} // namespace BlackCore

#endif //BLACKCORE_PLUGINMGR_H