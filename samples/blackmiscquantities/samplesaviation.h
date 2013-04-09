#ifndef BLACKMISC_SAMPLESAVIATION_H
#define BLACKMISC_SAMPLESAVIATION_H

#include "blackmisc/avheading.h"
#include "blackmisc/avverticalpositions.h"
#include "blackmisc/pqconstants.h"
#include "blackmisc/aviocomsystem.h"
#include "blackmisc/avionavsystem.h"
#include "blackmisc/aviotransponder.h"

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
