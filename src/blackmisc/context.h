//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef BLACKMISC_CONTEXT_H
#define BLACKMISC_CONTEXT_H

#include <QMetaType>
#include <QObject>
#include <QMap>
#include <stdexcept>

namespace BlackMisc
{

    /*!
     * Provides centralized modular access to long-lived interface objects.
     */
    class IContext
    {
    public:
        /*!
         * Returns a reference to a static global context singleton.
         * \return
         * \warning Do not use this in code which could be called from within a plugin.
         * \deprecated Provided as a crutch to ease transition from singleton-based design.
         */
        static IContext &getInstance();

        /*!
         * Allows for apps to set the static global context singleton.
         * \deprecated Provided as a crutch to ease transition from singleton-based design.
         */
        static void setInstance(IContext &);

        /*!
         * Destructor.
         */
        virtual ~IContext();

        /*!
         * Returns true if an object with the given name is present.
         * You usually use the type-safe hasObject method instead.
         * \param name
         * \return
         */
        bool hasQObject(const QString &name)
        {
            return getQObjectNothrow(name);
        }

        /*!
         * Set the object pointer with the type T.
         * \tparam T A class derived from QObject using the Q_OBJECT macro.
         * \param object
         * \param name The name of the object; default is the name of its class, T.
         */
        template <class T> void setObject(T &object, QString name = T::staticMetaObject.className())
        {
            setQObject(name, object);
        }

        /*!
         * Remove the object pointer with the type T.
         * \tparam T A class derived from QObject using the Q_OBJECT macro.
         * \param name The name of the object; default is the name of its class T.
         */
        template <class T> void removeObject(const QString &name = T::staticMetaObject.className())
        {
            removeQObject(name);
        }

        /*!
         * Return an object pointer of the class T.
         * \tparam T A class derived from QObject using the Q_OBJECT macro.
         * \param name The name of the object; default is the name of its class, T.
         * \return
         * \throw std::logic_error The requested object is not present.
         */
        template <class T> T &getObject(const QString &name = T::staticMetaObject.className())
        {
            T *obj = qobject_cast<T *>(getQObjectNothrow(name));
            if (!obj) { throw std::logic_error("IContext: qobject_cast failed"); }
            return *obj;
        }

        /*!
         * Return true if the requested object in present in the context.
         * \tparam T A class derived from QObject using the Q_OBJECT macro.
         * \param name The name of the object; default is the name of its class, T.
         * \return
         */
        template <class T> bool hasObject(const QString &name = T::staticMetaObject.className()) const
        {
            const QObject *qobj = getQObjectNothrow(name);
            return qobj && qobject_cast<const T *>(qobj);
        }

        /*!
         * Sets an object pointer, given by its name.
         * You usually use the type-safe setObject method instead.
         * \param object
         * \param name
         */
        virtual void setQObject(QString name, QObject &object) = 0;

        /*!
         * Removes an object pointer, given by its name.
         * You usually use the type-safe removeObject method instead.
         * \param name
         */
        virtual void removeQObject(const QString &name) = 0;

        /*!
         * Like getQObject but returns nullptr instead of throwing an exception.
         * \param name
         * \return
         */
        virtual const QObject *getQObjectNothrow(const QString &name) const = 0;

        /*!
         * Like getQObject but returns nullptr instead of throwing an exception.
         * \param name
         * \return
         */
        QObject *getQObjectNothrow(const QString &name)
        {
            return const_cast<QObject *>(static_cast<const IContext *>(this)->getQObjectNothrow(name));
        }

        /*!
         * Returns the pointer to an object by its name.
         * You usually use the type-safe getObject method instead.
         * \param name
         * \return
         * \throw std::logic_error The requested object is not present.
         */
        QObject &getQObject(const QString &name)
        {
            QObject *qobj = getQObjectNothrow(name);
            if (!qobj) { throw std::logic_error("IContext: named object not found"); }
            return *qobj;
        }

        /*!
         * Set the global name of the application.
         * \param appName
         */
        virtual void setApplicationName(QString appName) = 0;

        /*!
         * Set the application name to the default.
         */
        virtual void setDefaultApplicationName() = 0;

        /*!
         * Return the global name of the application.
         * \return
         */
        virtual const QString &getApplicationName() const = 0;
    };

    /*!
     * Default implementation of the IContext interface.
     */
    class CApplicationContext : public IContext
    {
    public:
        /*!
         * Constructor
         */
        CApplicationContext();

        virtual const QObject *getQObjectNothrow(const QString &) const;
        virtual void setQObject(QString, QObject &);
        virtual void removeQObject(const QString &);
        virtual void setApplicationName(QString appName) { m_appName = appName; }
        virtual void setDefaultApplicationName();
        virtual const QString &getApplicationName() const { return m_appName; }

    private:
        QMap<QString, QObject *> m_map;
        QString m_appName;
    };

    /*!
        This macros helps to create the body of a singletone class,
        which registers itself with the application context.
        \warning Singletons defined with this macro will not be accessible in plugins.
        \deprecated Preferred way is, during application initialization,
                    construct would-be singletons and register them manually,
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
            if (BlackMisc::IContext::getInstance().hasQObject(#className)) \
            { \
                m_instance = static_cast<className*>(&BlackMisc::IContext::getInstance().getQObject(#className)); \
            } \
            else \
            { \
                /* We have no allocated object yet, so do it now. */ \
                m_instance = new className; \
                BlackMisc::IContext::getInstance().setQObject(#className, *m_instance); \
            } \
        } \
        return *m_instance; \
    } \
private:
    /* Put your class constructor directly after this makro and make sure it is private! */


#define SINGLETON_CLASS_IMPLEMENTATION(className) className *className::m_instance = NULL;

} // namespace BlackMisc

#endif //BLACKMISC_CONTEXT_H
