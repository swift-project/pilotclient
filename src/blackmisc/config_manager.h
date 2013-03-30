//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QMap>
#include <QString>

#include "blackmisc/context.h"

namespace BlackMisc
{

	class CConfig;

	class CConfigManager
    {
		// safe singleton declaration
        SINGLETON_CLASS_DECLARATION(CConfigManager)
		
		CConfigManager();
    public:
	
		//! Configuration Manager error codes.
        /*! This enum lists all errors, which can appear. If you need
		 *  a readable output then consider using /sa getErrorString() 
		 */
        typedef enum { UNKNOWN_PATH = 0,   /*!< Type String. */

                     } TErrorCodes;

		//! Sets the global path, where all the config files are located
        /*!
          \param path absolute pathname
        */
        void setConfigPath(QString path);
		
		int readConfig(bool forceReload = false);
		
		int writeConfig();
		
		void clear();
		
		CConfig *getConfig(const QString &section);
		
	private:
		
		typedef QMap<QString, CConfig*>       TConfigMap;
		TConfigMap							  m_config_map;
		
		QString								  m_config_path;
	
	};
} // namespace BlackLib

#endif // CONFIGMANAGER_H
