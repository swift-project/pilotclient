#ifndef TESTAVIATIONBASE_H
#define TESTAVIATIONBASE_H

#include <QtTest/QtTest>
#include "blackmisc/pqconstants.h"
#include "blackmisc/avheading.h"
#include "blackmisc/avverticalpositions.h"
#include "blackmisc/aviocomsystem.h"
#include "blackmisc/avionavsystem.h"
#include "blackmisc/aviotransponder.h"

using namespace BlackMisc::Aviation;

namespace BlackMiscTest {

/*!
 * \brief Aviation classes basic tests
 */
class CTestAviationBase : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Standard test case constructor
     * \param parent
     */
    explicit CTestAviationBase(QObject *parent = 0);

private slots:
    /*!
     * \brief Basic unit tests for physical units
     */
    void headingBasics();
    /*!
     * \brief Vertical positions
     */
    void verticalPosition();
    /*!
     * \brief COM and NAV units
     */
    void comAndNav();

    /*!
     * \brief Transponder
     */
    void transponder();
};

} // namespace

#endif // TESTAVIATIONBASE_H
