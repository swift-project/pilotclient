//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QtGlobal>
#include <QDebug>
#include <QMap>


namespace BlackMisc
{

class IContext;

class CValue
{
public:

    //! Configuration type enum.
    /*! This enum lists the three different values, used in the config file. */
    typedef enum { CONFIG_STRING = 0,   /*!< Type String. */
                   CONFIG_INT,          /*!< Type Integer. */
                   CONFIG_DOUBLE,       /*!< Type Double. */
                   CONFIG_BOOL,         /*!< Type Bool. */
                   CONFIG_UNKOWN        /*!< Type Unknown. */
                 } TConfigType;

    CValue();

    CValue(const QString &value);

    void                    init();

    QString                &asString() { return m_value; }

    qint32                  asInt(bool *ok = NULL);

    bool                    asBool(bool *ok = NULL);

    double                  asDouble(bool *result);

    inline bool             isValid() {return m_type != CONFIG_UNKOWN;}

    inline bool             isInt() { return m_type == CONFIG_INT; }
    inline bool             isDouble() { return m_type == CONFIG_DOUBLE; }
    inline bool             isBool() { return m_type == CONFIG_BOOL; }

protected:

    bool                    m_bool_value;
    double                  m_double_value;
    qint32                  m_int_value;
    TConfigType             m_type;
    QString                 m_value;

};

//!  Configuration class.
/*!
  This class implements the configuration part of the library.
  \warning it is not safe to use this from within
*/
class CConfig
{
public:

    CConfig(IContext &context, const QString &filename, const QString &separator = "=", bool isRelative = false);

    //! Sets the value of the specified key.
    /*!
      \param key Key, which value should be set.
      \param value The actual value as T.
    */
    void setValue(const QString &key, const CValue &value);

    //! Returns the value from key.
    /*!
      \param key Specified key.
      \return The value to key 'key' as T&
    */
    CValue value(const QString &key) const;

    //! Function to check if the key is in the config.
    /*!
      \param key Specified key.
      \return Returns true, if key is in the config.
    */
    void add(const QString &key, const CValue &value);

    //! Function to check if the key is in the config.
    /*!
      \param key Specified key.
      \return Returns true, if key is in the config.
    */
    void update(const QString &key, const CValue &value);

    //! Function to check if the key is in the config.
    /*!
      \param key Specified key.
      \return Returns true, if key is in the config.
    */
    bool contains(const QString &key) const;

    //! Removes the key and its value from the config.
    /*!
      \param key Key, which has to be removed.
    */
    void remove(const QString &key);

    //! Displays the configuration to the debug output.
    /*!
      More info.
    */
    void display();

    /*!
     * Config File section
     */

    //! Returns the config filename of this config object.
    /*!
      \return filename of this config.
    */
    const QString &configFile() const {return m_configfile; }

    //! Loads the config file.
    /*!
      \return Returns true if loading went well.
    */
    bool load();

    //! Loads the config file.
    /*!
      \param filename Specify a different filename.
      \return Returns true if loading went well.
    */
    bool load(const QString &filename);

    //! Saves the config to file.
    /*!
      \return Returns true if loading went well.
    */
    bool save() { return false; } // TODO

    //! Saves the config to file.
    /*!
      \param filename Specify a different filename.
      \return Returns true if loading went well.
    */
    bool save(const QString &filename) { qDebug() << filename; return false; } // TODO

protected:

    IContext                           &m_context;
    QString                             m_configfile;
    QString                             m_separator;
    typedef QMap<QString, CValue>       TValueMap;
    TValueMap                           m_value_map;

};
} //! namespace BlackMisc

#endif // CONFIG_H
