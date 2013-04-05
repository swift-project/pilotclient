#ifndef AVIOBASE_H
#define AVIOBASE_H

#include "blackmisc/pqconstants.h"

namespace BlackMisc {

/*!
 * \brief Base class for avionics
 */
class CAvionicsBase
{
    /*!
     * Stream operator for debugging
     * \brief operator <<
     * \param debug
     * \param avionic
     * \return
     * \remarks Has to be in the header files to avoid template link errors
     */
    friend QDebug operator<<(QDebug debug, const CAvionicsBase &avionic) {
        QString v = avionic.stringForStreamingOperator();
        debug << v;
        return debug;
    }
    /*!
     * Stream operator for log messages
     * \brief operator <<
     * \param log
     * \param avionic
     * \return
     * \remarks Has to be in the header files to avoid template link errors
     */
    friend CLogMessage operator<<(CLogMessage log, const CAvionicsBase &avionic) {
        QString v = avionic.stringForStreamingOperator();
        log << v;
        return log;
    }

protected:
    /*!
     * \brief Default constructor
     */
    CAvionicsBase() {}
    /*!
     * \brief Meaningful string representation
     * \return
     */
    virtual QString stringForStreamingOperator() const = 0;
    /*!
     * \brief Are the set values valid / in range
     * \return
     */
    virtual bool validValues() { return true; }
public:

    /**
     * @brief Virtual destructor
     */
    virtual ~CAvionicsBase() {}

};

} // namespace

#endif // AVIOBASE_H
