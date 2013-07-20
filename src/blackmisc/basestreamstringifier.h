#ifndef BLACKMISC_BASESTREAMSTRINGIFIER_H
#define BLACKMISC_BASESTREAMSTRINGIFIER_H

#include "blackmisc/debug.h"
#include <QString>
#include <QtGlobal>
#include <QDataStream>
#include <QDebug>
#include <iostream>

namespace BlackMisc
{
/*!
 * \brief Provides "to QString" and stream operators
 */
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
        const CBaseStreamStringifier &sf = uc; // allows to acces protected method
        debug << sf.stringForStreaming();
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
        const CBaseStreamStringifier &sf = uc; // allows to acces protected method
        textStream << sf.stringForStreaming();
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
        const CBaseStreamStringifier &sf = uc; // allows to acces protected method
        stream << sf.stringForStreaming();
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
        const CBaseStreamStringifier &sf = uc; // allows to acces protected method
        log << sf.stringForStreaming();
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
        const CBaseStreamStringifier &sf = uc; // allows to acces protected method
        ostr << sf.stringForStreaming().toStdString();
        return ostr;
    }

public:
    /*!
     * \brief Virtual destructor
     */
    virtual ~CBaseStreamStringifier() {}

    /*!
     * \brief Cast as QString
     * \remarks operator cast caused too many ambiguity trouble
     */
    QString toQString() const
    {
        return this->stringForConverter();
    }

protected:

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
     * \brief String for converter
     * \return
     */
    virtual QString stringForConverter() const = 0;

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
