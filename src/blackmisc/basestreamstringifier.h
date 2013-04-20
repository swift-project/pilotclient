#ifndef BLACKMISC_BASESTREAMSTRINGIFIER_H
#define BLACKMISC_BASESTREAMSTRINGIFIER_H

#include "blackmisc/debug.h"
#include <QString>
#include <QtGlobal>
#include <QDebug>
#include <iostream>

namespace BlackMisc
{
/*!
 * \brief Provides "to QString" and stream operators
 */
template <class UsingClass> class CBaseStreamStringifier
{

    /*!
     * \brief Stream << overload to be used in debugging messages
     * \param debug
     * \param uc
     * \return
     */
    friend QDebug &operator<<(QDebug &debug, const UsingClass &uc)
    {
        const CBaseStreamStringifier &s = uc;
        debug << s.stringForStreaming();
        return debug;
    }

    /*!
     * \brief Stream operator << for QDataStream
     * \param stream
     * \param uc
     * \return
     */
    friend QDataStream &operator<<(QDataStream &stream, const UsingClass &uc)
    {
        const CBaseStreamStringifier &s = uc;
        stream << s.stringForStreaming();
        return stream;
    }

    /*!
     * \brief Stream operator << for log messages
     * \param log
     * \param uc
     * \return
     */
    friend CLogMessage &operator<<(CLogMessage &log, const UsingClass &uc)
    {
        const CBaseStreamStringifier &s = uc;
        log << s.stringForStreaming();
        return log;
    }

    /*!
     * \brief Stream operator << for std::cout
     * \param ostr
     * \param uc
     * \return
     */
    friend std::ostream &operator<<(std::ostream &ostr, const UsingClass &uc)
    {
        const CBaseStreamStringifier &s = uc;
        ostr << s.stringForStreaming().toStdString();
        return ostr;
    }

public:
    /*!
     * \brief Virtual destructor
     */
    virtual ~CBaseStreamStringifier() {}

    /*!
     * \brief Cast as QString
     */
    operator QString() const
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

};

} // namespace

#endif // guard
