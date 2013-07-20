/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_PLUGINS_H
#define BLACKMISC_PLUGINS_H

#include <QtPlugin>
#include <QObject>

namespace BlackMisc
{

    class IContext;
    class IPluginFactory;

    /*!
        The interface implemented by the main worker class in a plugin.
        Constructed by a factory implementing IPluginFactory and
        accessible to the application through the CPluginManager.
    */
    class IPlugin
    {
    public:
        /*!
            Destructor.
        */
        virtual ~IPlugin() {}

        /*!
            Exceptions are not allowed to leave plugins, so we use this function
            to test whether the plugin was constructed successfully.
            \return False if an error occurred during plugin construction.
        */
        virtual bool isValid() const = 0;

        /*!
            Return the factory used to construct this object.
            This is usually passed to the plugin's constructor by the factory itself.
            \return
        */
        virtual IPluginFactory &getFactory() = 0;
    };

    /*!
        The root interface for classes exported from plugins.
        This is a factory pattern for creating and destroying instances of IPlugin.
        You usually don't need to worry about this, if you use the
        MAKE_BLACK_PLUGIN macro to define your factory.
    */
    class IPluginFactory
    {
    public:
        /*!
            Destructor.
        */
        virtual ~IPluginFactory() {}

        /*!
            Construct an instance of the plugin.
            \param context The application context.
            \return a pointer to the newly created plugin.
            \warning You must release this pointer with IPluginFactory::destroy().
        */
        virtual IPlugin *create(IContext &context) = 0;

        /*!
            Destroy a plugin which was created by this factory.
        */
        virtual void destroy(IPlugin *plugin) = 0;

        /*!
            Return the plugin's short name.
            \return
        */
        virtual const char *getName() const = 0;

        /*!
            Return the plugin's description.
            \return
        */
        virtual const char *getDescription() const = 0;
    };

    /*!
        Custom deleter for QScopedPointer.
    */
    struct TPluginDeleter
    {
        static void cleanup(IPlugin *plugin)
        {
            if (plugin) {
                plugin->getFactory().destroy(plugin);
            }
        }
    };

} //namespace BlackMisc

//! Qt interface ID for IPluginFactory.
#define BLACKMISC_IPLUGINFACTORY_IID "net.vatsim.client.BlackMisc.IPluginFactory"

Q_DECLARE_INTERFACE(BlackMisc::IPluginFactory, BLACKMISC_IPLUGINFACTORY_IID)

/*!
    Macro to put inside an IPluginFactory subclass to help with implementation.
    \param CLASS The plugin class which this factory constructs.
    \param NAME A string literal, the plugin's short name.
    \param DESCRIPTION A string literal, a brief description of the plugin.
*/
#define BLACKMISC_IMPLEMENT_IPLUGINFACTORY(CLASS, NAME, DESCRIPTION) \
    public: \
    BlackMisc::IPlugin *create(BlackMisc::IContext &context) { return new CLASS(*this, context); } \
    void destroy(BlackMisc::IPlugin *plugin) { if (plugin) delete plugin; } \
    const char *getName() const { return NAME; } \
    const char *getDescription() const { return DESCRIPTION; }

#endif //BLACKMISC_PLUGINS_H