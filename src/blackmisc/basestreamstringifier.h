#ifndef BLACKMISC_BASESTREAMSTRINGIFIER_H
#define BLACKMISC_BASESTREAMSTRINGIFIER_H

#include "blackmisc/debug.h"
#include <QDBusMetaType>
#include <QString>
#include <QtGlobal>
#include <QDataStream>
#include <QDebug>
#include <iostream>

namespace BlackMisc {

/*!
 * \brief Provides "to QString" and stream operators
 */
// Virtual operators: http://stackoverflow.com/a/4571634/356726
class CBaseStreamStringifier
{

    /*!
     * \brief Stream << overload to be used in debugging messages
     * \param debug
     * \param uc
     * \return
     */
    friend QDebug operator<<(QDebug debug, const CBaseStreamStringifier &uc)
    {
        debug << uc.stringForStreaming();
        return debug;
    }

    // msvc2010: friend QDebug &operator<<(QDebug &debug, const CBaseStreamStringifier &uc)
    // MinGW: No reference

    /*!
     * \brief Operator << based on text stream
     * \param textStream
     * \param uc
     * \return
     */
    friend QTextStream &operator<<(QTextStream &textStream, const CBaseStreamStringifier &uc)
    {
        textStream << uc.stringForStreaming();
        return textStream;
    }

    /*!
     * \brief Operator << when there is no debug stream
     * \param nodebug
     * \param uc
     * \return
     */
    friend QNoDebug operator<<(QNoDebug nodebug, const CBaseStreamStringifier & /* uc */)
    {
        return nodebug;
    }

    /*!
     * \brief Stream operator << for QDataStream
     * \param stream
     * \param uc
     * \return
     */
    friend QDataStream &operator<<(QDataStream &stream, const CBaseStreamStringifier &uc)
    {
        stream << uc.stringForStreaming();
        return stream;
    }

    /*!
     * \brief Stream operator << for log messages
     * \param log
     * \param uc
     * \return
     */
    friend CLogMessage operator<<(CLogMessage log, const CBaseStreamStringifier &uc)
    {
        log << uc.stringForStreaming();
        return log;
    }

    /*!
     * \brief Stream operator << for std::cout
     * \param ostr
     * \param uc
     * \return
     */
    friend std::ostream &operator<<(std::ostream &ostr, const CBaseStreamStringifier &uc)
    {
        ostr << uc.stringForStreaming().toStdString();
        return ostr;
    }

    /*!
     * \brief Unmarshalling operator >>, DBus to object
     * \param argument
     * \param uc
     * \return
     */
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, CBaseStreamStringifier &uc) {
        argument.beginStructure();
        uc.unmarshallFromDbus(argument);
        argument.endStructure();
        return argument;
    }

    /*!
     * \brief Marshalling operator <<, object to DBus
     * \param argument
     * \param pq
     * \return
     */
    friend QDBusArgument &operator<<(QDBusArgument &argument, const CBaseStreamStringifier &uc)
    {
        argument.beginStructure();
        uc.marshallToDbus(argument);
        argument.endStructure();
        return argument;
    }

public:

    /*!
     * \brief Virtual destructor
     */
    virtual ~CBaseStreamStringifier() {}

    /*!
     * \brief Cast as QString
     * \bool i18n
     * \remarks operator cast caused too many ambiguity trouble
     */
    QString toQString(bool i18n = false) const
    {
        return this->stringForConverter(i18n);
    }

protected:
    /*!
     * \brief Default constructor
     */
    CBaseStreamStringifier() {}

    /*!
     * \brief String for streaming operators
     * \return
     */
    virtual QString stringForStreaming() const
    {
        // simplest default implementation requires only one method
        return this->stringForConverter();
    }

    /*!
     * \brief String for QString conversion
     * \param i18n
     * \return
     */
    virtual QString stringForConverter(bool i18n = false) const = 0;

    /*!
     * \brief Stream to DBus
     * \param argument
     */
    virtual void marshallToDbus(QDBusArgument &) const {}

    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &) {}

    /*!
     * \brief Copy assignment operator.
     * This is protected in order to forbid slicing an instance of one derived
     * class into an instance of a completely unrelated derived class.
     * \return
     */
    CBaseStreamStringifier& operator=(const CBaseStreamStringifier&) { return *this; }

};

} // namespace

#endif // guard
