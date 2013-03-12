#include <QDir>
#include <QStringList>


#include "blackmisc/config.h"
#include "blackmisc/debug.h"
#include "blackmisc/config_manager.h"

namespace BlackMisc
{
	SINGLETON_CLASS_IMPLEMENTATION(CConfigManager)
	
	CConfigManager::CConfigManager()
	{
	}
	
	void CConfigManager::setConfigPath(QString &path)
	{
		m_config_path = QDir(path).absolutePath();
	
	}

	int CConfigManager::readConfig(bool forceReload)
	{
		/*!
		  Foreach *.cfg file in the config path,
		  create a new CConfig object and parse it into.
		  Then append the pointer to this object into
		  the config map.
		*/
		
		if (forceReload)
			clear();
		
		QDir directory(m_config_path);
		QStringList filters;
		filters << "*.cfg";
		directory.setNameFilters(filters);
		 
		QStringList files = directory.entryList();
		 
		QStringList::const_iterator constIterator;
		for (constIterator = files.constBegin(); constIterator != files.constEnd(); ++constIterator)
		{
			int index = (*constIterator).indexOf(".");
			QString section = (*constIterator).left(index);

			if (!m_config_map.contains(section))
			{
				QString filePath = m_config_path + QDir::separator() + (*constIterator);
				CConfig *config = new CConfig(filePath);
				config->load();
				
				m_config_map.insert(section, config);
			}
		}

		return m_config_map.size();
	}
	
	int CConfigManager::writeConfig()
	{
		/*! 
		  Foreach config object in the map,
		  create a file with all values, from
		  the object. Filename is created
		  from the section name
		*/
		return m_config_map.size();
	}
	
	void CConfigManager::clear()
	{
		TConfigMap::iterator iterator;
		for (iterator = m_config_map.begin(); iterator != m_config_map.end(); ++iterator)
		{
			delete iterator.value();
		}
		
		m_config_map.clear();
	}
	
	CConfig *CConfigManager::getConfig(const QString &section)
	{
		if (m_config_map.contains(section))
			return m_config_map.value(section);
		else
		{
			bError << "Could not find config section: " << section;
			return NULL;
		}
	}
	
} // namespace BlackMisc