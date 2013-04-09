#ifndef SAMPLESAVIATION_H
#define SAMPLESAVIATION_H

#include "blackmisc/avheading.h"
#include "blackmisc/avverticalpositions.h"
#include "blackmisc/pqconstants.h"
#include "blackmisc/aviocomsystem.h"
#include "blackmisc/avionavsystem.h"
#include "blackmisc/aviotransponder.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMiscTest {

/*!
 * \brief Samples for physical quantities
 */
class CSamplesAviation
{
public:
    /*!
     * \brief Run the samples
     */
    static int samples();
};
} // namespace

#endif
