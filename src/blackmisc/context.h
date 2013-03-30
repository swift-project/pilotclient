//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef CONTEXT_H
#define CONTEXT_H

#include <QMap>

namespace BlackMisc
{
	class CLog;
    class CDebug;

	//! IContext Interface.
	/*!
	  This interface abstracts any application and library context. Use it to
	  add singleton classes and for application and library wide logging. 
	*/
	class IContext
	{
	public:

		//! Reference to IConext.
		/*!
		  This static function returns a reference to the singleton object.
		  Use always this method to retrieve the context, since you cannot
		  instantiate your own one. Before it can be used you have to create
		  either a CApplicationContext or a CLibraryContext in the very
		  beginning of the executable/libary.
		*/
		static IContext &getInstance();
		
		//! Returns true if the context is initialized and ready.
		/*!
		  \return Returns true if context is initialized, otherwise false.
		*/
		static bool isInitialised();
		
		//! Destructor.
		/*!
		  Destroys the context.
		*/
		virtual ~IContext();
		
		//! Pure virtual method returns the pointer to singletone object by its name.
		/*!
		  \param singletonName a reference to the singletones name.
		  \return Returns a pointer to the singleton object.
		*/
        virtual void *singletonPointer(const QString &singletonName) = 0;
		
		//! Pure virtual method sets the singletone pointer, given by its name.
		/*!
		  \param singletonName a reference to the singletones name.
		  \param object a pointer to the singletone object.
		*/
        virtual void setSingletonPointer(const QString &singletonName, void *object) = 0;
		
		//! Deallocates the object and removes the singletone pointer from the context map.
		/*!
		  \param singletonName a reference to the singletones name.
		  \param object a pointer to the singletone object.
		*/
        virtual void releaseSingletonPointer(const QString &singletonName, void *object) = 0;
		
		//! Pure virtual method returns the pointer to debug object
		/*!
		  \return Pointer to CDebug object
		*/
        virtual CDebug *getDebug()						= 0;
		
		//! Pure virtual function to set the global error log object
		/*!
		  \param Pointer to CDebug object
		*/
        virtual void setDebug(CDebug *debug)		= 0;
		
	protected:
		//! Method to register the context. 
		/*! This method should be called by the derived class. It sets the 
			context variable to the calling object.*/
		/*!
		  \param Pointer to CLog object
		*/
		void	registerContext();
		 
		//! Pointer to the global context.
		/*!
		  This variable holds a pointer to the application context.
		*/
		static IContext *m_context;

		//! Pointer to the global Debug object.
		/*!
		  This member variable contains all logging types, not only debug.
		*/
        CDebug *m_debug;

	};

	//! CApplicationContext.
	/*!
	  This class implements the IContext interface for applications.  
	*/
	class CApplicationContext : public IContext
	{
	public:
		CApplicationContext();

		//! This method returns the pointer to singletone object by its name.
		/*!
		  \param singletonName a reference to the singletones name.
		  \return Returns a pointer to the singleton object.
		*/
        virtual void *singletonPointer(const QString &singletonName);
		
		//! Sets the singletone pointer, given by its name.
		/*!
		  \param singletonName a reference to the singletones name.
		  \param object a pointer to the singletone object.
		*/
        virtual void setSingletonPointer(const QString &singletonName, void *object);
		
		//! Deallocates the object and removes the singletone pointer from the context map.
		/*!
		  \param singletonName a reference to the singletones name.
		  \param object a pointer to the singletone object.
		*/
        virtual void releaseSingletonPointer(const QString &singletonName, void *ptr);

        //! Pure virtual method returns the pointer to debug object
        /*!
          \return Pointer to CDebug object
        */
        virtual CDebug *getDebug();

        //! Pure virtual function to set the global error log object
        /*!
          \param Pointer to CDebug object
        */
        virtual void setDebug(CDebug *debug);

	private:
		//! Typedef for the singleton map
        typedef QMap<QString, void*>	TSingletonMap;
		
		 //! Map of all registered objects inside the application.
		 /*!
		   This map holds associates all registered singleton objects to their names.
		 */
		TSingletonMap					m_singletonObjects;
	};

	//! CApplicationContext.
	/*!
	  This class implements the IContext interface for libraries.  
	*/
	class CLibraryContext : public IContext
	{
	public:
		CLibraryContext (IContext &application);

        //! This method returns the pointer to singletone object by its name.
		/*!
		  \param singletonName a reference to the singletones name.
		  \return Returns a pointer to the singleton object.
		*/
        virtual void *singletonPointer(const QString &singletonName);
		
		//! Sets the singletone pointer, given by its name.
		/*!
		  \param singletonName a reference to the singletones name.
		  \param object a pointer to the singletone object.
		*/
        virtual void setSingletonPointer(const QString &singletonName, void *ptr);
		
		//! Deallocates the object and removes the singletone pointer from the context map.
		/*!
		  \param singletonName a reference to the singletones name.
		  \param object a pointer to the singletone object.
		*/
        virtual void releaseSingletonPointer(const QString &singletonName, void *ptr);

        //! Pure virtual method returns the pointer to debug object
        /*!
          \return Pointer to CDebug object
        */
        virtual CDebug *getDebug();

        //! Pure virtual function to set the global error log object
        /*!
          \param Pointer to CDebug object
        */
        virtual void setDebug(CDebug *debug);
		
	private:
		 //! Pointer the application context.
		 /*!
           Libraries do not have their own context, because they are using
		   the one from the linked application.
		 */
		IContext		*m_applicationContext;
	};

    /*!
		This macros helps to create the body of a singletone class,
		which registers itself with the application context.
	*/
    #define SINGLETON_CLASS_DECLARATION(className) \
        private:\
             /* Singletone class constructor */ \
            className (const className &) {}\
            /* the local static pointer to the singleton instance */ \
            static className	*m_instance; \
        public:\
            static className &getInstance() \
            { \
                if (m_instance == NULL) \
                { \
                    /* We need a valid context.*/ \
                    bAssertstr(BlackMisc::IContext::isInitialised(), QString("You are trying to access a singleton without having initialized a context. The simplest correction is to add 'BlackMisc::CApplicationContext myApplicationContext;' at the very beginning of your application.")); \
					\
                    /* Get the singleton object from the context, if there is one */ \
					void *object = BlackMisc::IContext::getInstance().singletonPointer(#className); \
                    if (object == NULL) \
                    { \
                        /* We have no allocated object yet, so do it now. */ \
                        m_instance = new className; \
                        BlackMisc::IContext::getInstance().setSingletonPointer(#className, m_instance); \
                    } \
                    else \
                    { \
                        /* Always use the c++ methods instead of casting the C way. */ \
                        m_instance = reinterpret_cast<className*>(object); \
                    } \
                } \
                return *m_instance; \
            } \
        private:
            /* Put your class constructor directly after this makro and make sure it is private! */
			
	
	#define SINGLETON_CLASS_IMPLEMENTATION(className) className *className::m_instance = NULL;

} // namespace BlackMisc

#endif CONTEXT_H
