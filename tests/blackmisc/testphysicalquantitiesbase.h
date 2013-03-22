#ifndef TESTPHYSICALQUANTITIESBASE_H
#define TESTPHYSICALQUANTITIESBASE_H

#include <QtTest/QtTest>
#include "blackmisc/pqconstants.h"
using namespace BlackMisc;

namespace BlackMiscTest {

/*!
 * \brief Physical quantities,basic tests
 */
class TestPhysicalQuantitiesBase : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Standard test case constructor
     * \param parent
     */
    explicit TestPhysicalQuantitiesBase(QObject *parent = 0);
private slots:
    /*!
     * \brief Basic unit tests for physical units
     */
    void unitsBasics();
    /*!
     * \brief Basic tests around distance
     */
    void distanceBasics();
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
     * \brief testing construction / destruction in memory
     */
    void memoryTests();
};

}

#endif // TESTPHYSICALQUANTITIESBASE_H
