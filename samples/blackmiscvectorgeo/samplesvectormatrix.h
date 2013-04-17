#ifndef BLACKMISCTEST_SAMPLESVECTORMATRIX_H
#define BLACKMISCTEST_SAMPLESVECTORMATRIX_H

#include "blackmisc/mathvector3d.h"
#include "blackmisc/mathmatrix3x3.h"

namespace BlackMiscTest
{

/*!
 * \brief Samples for vector / matrix
 */
class CSamplesVectorMatrix
{
public:
    /*!
     * \brief Run the samples
     */
    static int samples();

private:
    /*!
     * \brief Avoid init
     */
    CSamplesVectorMatrix();
};
} // namespace

#endif
