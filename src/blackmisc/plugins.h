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

} //namespace BlackMisc

// must be placed outside namespace and before CPluginFactoryBase
Q_DECLARE_INTERFACE(BlackMisc::IPluginFactory, "net.vatsim.client.BlackMisc.IPluginFactory")

namespace BlackMisc
{

    /*!
        Base class for CPluginFactory template used by MAKE_BLACK_PLUGIN.
    */
    class CPluginFactoryBase : public QObject, public IPluginFactory
    {
        Q_OBJECT
        Q_INTERFACES(BlackMisc::IPluginFactory)
    };

    /*!
        Template used by MAKE_BLACK_PLUGIN.
    */
    template <class P>
    class CPluginFactory : public CPluginFactoryBase
    {
    public:
        IPlugin *create(IContext &context) { return new P(*this, context); }

        void destroy(IPlugin *plugin) { if (plugin) delete plugin; }
    };

    /*!
        Simplifies the process of building a plugin.
        Put this macro somewhere in one of your plugin's .cpp files (but not in a namespace)
        to export the necessary factory class for your plugin.
        FQCLASS must have a constructor with the signature (IPluginFactory&, IContext&).
        \param NAME A short name for your plugin with no spaces (a bareword, not a string).
        \param FQCLASS The fully qualified name of the IPlugin subclass that the factory will construct.
    */
    #define MAKE_BLACK_PLUGIN(NAME, FQCLASS, DESCR) \
        class CPluginFactory_##NAME : public BlackMisc::CPluginFactory<FQCLASS> \
        { \
            const char *getName() const { return #NAME ; } \
            const char *getDescription() const { return DESCR; } \
        }; \
        Q_EXPORT_PLUGIN2(NAME, CPluginFactory_##NAME )

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

#endif //BLACKMISC_PLUGINS_H