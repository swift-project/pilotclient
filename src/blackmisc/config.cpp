//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "blackmisc/config.h"
#include "blackmisc/debug.h"
#include <QFile>
#include <QStringList>

namespace BlackMisc
{
    CValue::CValue() : _type(CONFIG_UNKOWN), _value(""), _int_value(0),
        _bool_value(false), _double_value(0.0)
    {

    }

    CValue::CValue (const QString& value) : _type(CONFIG_UNKOWN), _value(value), _int_value(0),
        _bool_value(false), _double_value(0.0)
    {
        init();
    }

    void CValue::init()
    {
        bool result = false;
        qint32 int_value = 0;
        double double_value = 0.0;

        int_value = _value.toInt( &result );
        if (result)
        {
            _type            = CONFIG_INT;
            _int_value       = int_value;
            _double_value    = int_value;
            _bool_value      = false;
            return;
        }

        double_value = _value.toDouble( &result );
        if (result)
        {
            _type            = CONFIG_DOUBLE;
            _int_value       = 0;
            _double_value    = double_value;
            _bool_value      = false;
            return;
        }

        if ( _value.compare("false", Qt::CaseInsensitive) == 0)
        {
            _type            = CONFIG_BOOL;
            _int_value       = 0;
            _double_value    = 0.0;
            _bool_value      = false;
            return;
        }

        if ( _value.compare("true", Qt::CaseInsensitive) == 0)
        {
            _type            = CONFIG_BOOL;
            _int_value       = 0;
            _double_value    = 0.0;
            _bool_value      = true;
            return;
        }

        _type            = CONFIG_STRING;
        _int_value       = 0;
        _double_value    = 0.0;
        _bool_value      = false;
    }

    qint32 CValue::asInt( bool* ok )
    {
        bool result = true;
        if ( _type != CONFIG_INT )
        {
           result = false;
        }

        if (ok != NULL)
            *ok = result;
        return _int_value;
    }

    double CValue::asDouble( bool* ok )
    {
        bool result = true;
        if ( _type != CONFIG_DOUBLE )
        {
            result = false;
        }

        if (ok != NULL)
            *ok = result;
        return _double_value;
    }

    bool CValue::asBool( bool* ok )
    {
        bool result = true;
        if ( _type != CONFIG_BOOL )
        {
            result = false;
        }

        if (ok != NULL)
            *ok = result;
        return _bool_value;
    }

    CConfig::CConfig(IContext &context, const QString& filename, const QString& separator, bool /*isRelative*/)
        : m_context(context), m_configfile(filename), m_separator(separator)
    {
    }

    bool CConfig::load()
    {
        return load(m_configfile);
    }

    bool CConfig::load(const QString& filename)
    {
        m_configfile = filename;

        m_value_map.clear();

        if (m_configfile.isEmpty())
        {
            bError(m_context) << "Can't open emtpy config file!";
            return false;
        }
        QFile input (m_configfile);
        if ( !input.open(QIODevice::ReadOnly))
        {
            bError(m_context) << "Failed to open config file !" << m_configfile;
            input.close();
            return false;
        }

        bool error = false;
        quint32 no_line = 0;

        QTextStream instream(&input);

        while ( !instream.atEnd() )
        {
            ++no_line;

            QString line = instream.readLine();

            // Remove any whitespace from the start and end
            line = line.trimmed();

            // Search for the comment operator and discard it
            int pos = line.indexOf( QChar('#') );
            if ( pos != -1 )
                line = line.left(pos).trimmed();

            // Check if we have a empty line
            if ( line.isEmpty() )
                continue;

            // Separate between key and value
            QStringList tags = line.split(m_separator);
            if ( tags.count() != 2)
            {
                bWarning(m_context) << "Could not parse line " << no_line << " in file " << m_configfile << "!";
                error = true;
                continue;
            }
            QString key = tags[0].trimmed();
            CValue value = CValue( tags[1].trimmed() );

            setValue (key, value);
        }

        input.close();
        return !error;
    }

    void CConfig::setValue(const QString &key, const CValue &value)
    {
        if ( contains(key) )
            update(key, value);
        else
            add(key, value);
    }

    CValue CConfig::value(const QString &key) const
    {
        if (m_value_map.contains(key))
            return m_value_map.value(key);
        else
            return CValue();
    }

    bool CConfig::contains(const QString &key) const
    {
        return m_value_map.contains(key);
    }

    void CConfig::remove(const QString &key)
    {
        m_value_map.remove(key);
    }

    void CConfig::add(const QString &key, const CValue &value)
    {
        // Paranoid...
        if ( contains(key) )
        {
            update(key, value);
        }
        else
        {
            m_value_map.insert(key, value);
        }
    }

    void CConfig::update(const QString &key, const CValue &value)
    {
        m_value_map[key] = value;
    }

    void CConfig::display()
    {
        TValueMap::const_iterator it;
        for (it = m_value_map.begin(); it != m_value_map.end(); ++it)
        {
            CValue value = it.value();
            bDebug(m_context) << "Key: " << it.key() << " - Value: " << value.asString();
        }
    }

} //! namespace BlackMisc
