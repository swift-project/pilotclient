//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef BLACKMISC_CONTEXT_H
#define BLACKMISC_CONTEXT_H

#include <QObject>
#include <QMap>

namespace BlackMisc
{

    class CDebug;

    /*!
      Keeps track of all singleton and pseudo-singleton objects.
    */
    class IContext
    {
    public:
        /*!
          Returns a reference to the static global context singleton.
          \return
          \warning Do not use this from within a plugin.
        */
        static IContext &getInstance();

        /*!
          Destructor.
        */
        virtual ~IContext();

        /*!
          Returns the pointer to a singleton object by its name.
          You usually use the template overload of this method instead.
          \param singletonName
          \return
          \throw std::logic_error The requested singleton is not present.
        */
        virtual QObject *singleton(const QString &singletonName) = 0;

        /*!
          Returns true if a singleton object with the give name is present.
          You usually use the template overload of this method instead.
          \param singletonName
          \return
        */
        virtual bool hasSingleton(const QString &singletonName) const = 0;

        /*!
          Sets a singleton pointer, given by its name.
          You usually use the template overload of this method instead.
          \param singletonName
          \param object
        */
        virtual void setSingleton(const QString &singletonName, QObject *object) = 0;

        /*!
          Removes the singleton pointer, given by its name.
          You usually use the template overload of this method instead.
          \param singletonName
          \param object
        */
        virtual void releaseSingleton(const QString &singletonName) = 0;

        /*!
            Return the singleton pointer with the type T.
            \tparam T An interface defined with the BLACK_INTERFACE macro.
            \return
            \throw std::logic_error The requested singleton is not present.
        */
        template <class T>
        T *singleton()
        {
            QObject *qobj = singleton(T::blackInterfaceId());
            T *t = qobject_cast<T *>(qobj);
            Q_ASSERT_X(t, "IContext", "qobject_cast failed");
            return t;
        }

        /*!
            Return true if the requested singleton in present in the context.
            \tparam T An interface defined with the BLACK_INTERFACE macro.
            \return
        */
        template <class T>
        bool hasSingleton() const
        {
            return hasSingleton(T::blackInterfaceId());
        }

        /*!
            Set the singleton pointer with the type T.
            \tparam T An interface defined with the BLACK_INTERFACE macro.
            \param object
        */
        template <class T>
        void setSingleton(T *object)
        {
            setSingleton(T::blackInterfaceId(), object);
        }

        /*!
            Remove the singleton pointer with the type T.
            \tparam T An interface defined with the BLACK_INTERFACE macro.
        */
        template <class T>
        void releaseSingleton()
        {
            releaseSingleton(T::blackInterfaceId());
        }

        /*!
            Set the global name of the application.
            \param appName
        */
        virtual void setApplicationName(const QString &appName) = 0;

        /*!
            Set the application name to the default.
        */
        virtual void setDefaultApplicationName() = 0;

        /*!
            Return the global name of the application.
            \return
        */
        virtual const QString &getApplicationName() const = 0;

        /*!
            Return the CDebug singleton.
            \return
            \deprecated Use getSingletonPointer<CDebug>() instead.
            \throw std::logic_error The requested singleton is not present.
        */
        virtual CDebug *getDebug() = 0;

        /*!
            Set the CDebug singleton.
            \param debug
            \deprecated Use setSingletonPointer<CDebug>() instead.
        */
        virtual void setDebug(CDebug *debug) = 0;
    };

    /*!
        Enable an interface to be manipulated with the template methods of IContext.
        Put this macro in the public section of the interface definition.
        \param FQNAME The fully qualified name of the interface (e.g. BlackMisc::IWhatever).
    */
#define BLACK_INTERFACE(FQNAME) static const char *blackInterfaceId() { return #FQNAME ; }

    /*!
      Default implementation of the IContext interface.
    */
    class CApplicationContext : public IContext
    {
    public:
        /*!
            Constructor
        */
        CApplicationContext();

        virtual QObject *singleton(const QString &singletonName);
        virtual bool hasSingleton(const QString &singletonName) const;
        virtual void setSingleton(const QString &singletonName, QObject *object);
        virtual void releaseSingleton(const QString &singletonName);
        virtual CDebug *getDebug();
        virtual void setDebug(CDebug *debug);
        virtual void setApplicationName(const QString &applicationName) { m_appName = applicationName; }
        virtual void setDefaultApplicationName();
        virtual const QString &getApplicationName() const { return m_appName; }

    private:
        typedef QMap<QString, QObject *> TSingletonMap;

        TSingletonMap m_singletons;

        QString m_appName;
    };

    /*!
        This macros helps to create the body of a singletone class,
        which registers itself with the application context.
        \warning Singletons defined with this macro will not be accessible in plugins.
        \deprecated Preferred way is, during application initialization,
                    construct singletons and register them manually,
                    and when you want to access them, do it through the IContext.
    */
#define SINGLETON_CLASS_DECLARATION(className) \
private:\
    /* Forbid copying */ \
    className (const className &); \
    className &operator= (const className &); \
    /* the local static pointer to the singleton instance */ \
    static className    *m_instance; \
public:\
    static className &getInstance() \
    { \
        if (m_instance == NULL) \
        { \
            /* Get the singleton object from the context, if there is one */ \
            if (BlackMisc::IContext::getInstance().hasSingleton(#className)) \
            { \
                m_instance = reinterpret_cast<className*>(BlackMisc::IContext::getInstance().singleton(#className)); \
            } \
            else \
            { \
                /* We have no allocated object yet, so do it now. */ \
                m_instance = new className; \
                BlackMisc::IContext::getInstance().setSingleton(#className, m_instance); \
            } \
        } \
        return *m_instance; \
    } \
private:
    /* Put your class constructor directly after this makro and make sure it is private! */


#define SINGLETON_CLASS_IMPLEMENTATION(className) className *className::m_instance = NULL;

} // namespace BlackMisc

#endif //BLACKMISC_CONTEXT_H
