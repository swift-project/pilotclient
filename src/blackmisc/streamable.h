#ifndef BLACKMISC_STREAMABLE_H
#define BLACKMISC_STREAMABLE_H

#include "blackmisc/debug.h"
#include <QDBusMetaType>
#include <QString>
#include <QtGlobal>
#include <QDataStream>
#include <QDebug>
#include <iostream>

namespace BlackMisc {

/*!
 * \brief Base class for streamable value objects.
 * Public non-virtual interface with protected virtual implementation.
 */
// Virtual operators: http://stackoverflow.com/a/4571634/356726
class CStreamable
{
    /*!
     * \brief Stream << overload to be used in debugging messages
     * \param debug
     * \param uc
     * \return
     */
    friend QDebug operator<<(QDebug debug, const CStreamable &uc)
    {
        debug << uc.stringForStreaming();
        return debug;
    }

    /*!
     * \brief Operator << based on text stream
     * \param textStream
     * \param uc
     * \return
     */
    friend QTextStream &operator<<(QTextStream &textStream, const CStreamable &uc)
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
    friend QNoDebug operator<<(QNoDebug nodebug, const CStreamable & /* uc */)
    {
        return nodebug;
    }

    /*!
     * \brief Stream operator << for QDataStream
     * \param stream
     * \param uc
     * \return
     */
    friend QDataStream &operator<<(QDataStream &stream, const CStreamable &uc)
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
    friend CLogMessage operator<<(CLogMessage log, const CStreamable &uc)
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
    friend std::ostream &operator<<(std::ostream &ostr, const CStreamable &uc)
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
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, CStreamable &uc)
    {
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
    friend QDBusArgument &operator<<(QDBusArgument &argument, const CStreamable &uc)
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
    virtual ~CStreamable() {}

    /*!
     * \brief Cast as QString
     * \bool i18n
     */
    QString toQString(bool i18n = false) const
    {
        return this->convertToQString(i18n);
    }

protected:
    /*!
     * \brief Default constructor
     */
    CStreamable() {}

    /*!
     * \brief String for streaming operators
     * \return
     */
    virtual QString stringForStreaming() const
    {
        // simplest default implementation requires only one method
        return this->convertToQString();
    }

    /*!
     * \brief String for QString conversion
     * \param i18n
     * \return
     */
    virtual QString convertToQString(bool i18n = false) const = 0;

    /*!
     * \brief Stream to DBus
     * \param argument
     */
    virtual void marshallToDbus(QDBusArgument &) const = 0;

    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &) = 0;
};

/*!
 * Non-member non-friend operator for streaming T objects to QDBusArgument.
 * Needed because we can't rely on the friend operator in some cases due to
 * an unrelated template for streaming Container<T> in QtDBus/qdbusargument.h
 * which matches more types than it can actually handle.
 * \param argument
 * \param uc
 */
template <class T> typename std::enable_if<std::is_base_of<CStreamable, T>::value, QDBusArgument>::type const&
operator>>(const QDBusArgument &argument, T &uc)
{
    return argument >> static_cast<CStreamable&>(uc);
}

/*!
 * Non-member non-friend operator for streaming T objects from QDBusArgument.
 * Needed because we can't rely on the friend operator in some cases due to
 * an unrelated template for streaming Container<T> in QtDBus/qdbusargument.h
 * which matches more types than it can actually handle.
 * \param argument
 * \param uc
 */
template <class T> typename std::enable_if<std::is_base_of<CStreamable, T>::value, QDBusArgument>::type&
operator<<(QDBusArgument &argument, T &uc)
{
    return argument << static_cast<CStreamable const&>(uc);
}

} // namespace

#endif // guard
