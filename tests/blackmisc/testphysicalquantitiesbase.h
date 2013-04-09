#ifndef BLACKMISCTEST_TESTPHYSICALQUANTITIESBASE_H
#define BLACKMISCTEST_TESTPHYSICALQUANTITIESBASE_H

#include <QtTest/QtTest>
#include "blackmisc/pqconstants.h"

namespace BlackMiscTest
{

/*!
 * \brief Physical quantities,basic tests
 */
class CTestPhysicalQuantitiesBase : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Standard test case constructor
     * \param parent
     */
    explicit CTestPhysicalQuantitiesBase(QObject *parent = 0);
private slots:
    /*!
     * \brief Basic unit tests for physical units
     */
    void unitsBasics();
    /*!
     * \brief Basic tests around length
     */
    void lengthBasics();
    /*!
     * \brief Basic tests about speed
     */
    void speedBasics();
    /*!
     * \brief Frequency tests
     */
    void frequencyTests();
    /*!
     * \brief Testing angles (degrees / radians)
     */
    void angleTests();
    /*!
     * \brief Testing angles
     */
    void massTests();
    /*!
     * \brief Testing pressure
     */
    void pressureTests();
    /*!
     * \brief Testing temperature
     */
    void temperatureTests();
    /*!
     * \brief Testing time
     */
    void timeTests();
    /*!
     * \brief Testing construction / destruction in memory
     */
    void memoryTests();
    /*!
     * \brief Basic arithmetic such as +/-
     */
    void basicArithmetic();

};

} // namespace

#endif // BLACKMISCTEST_TESTPHYSICALQUANTITIESBASE_H
