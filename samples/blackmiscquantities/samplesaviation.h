#ifndef SAMPLESAVIATION_H
#define SAMPLESAVIATION_H

#include "blackmisc/avheading.h"
#include "blackmisc/avverticalpositions.h"
#include "blackmisc/pqconstants.h"
#include "blackmisc/aviocomsystem.h"

using namespace BlackMisc;

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
}

#endif
