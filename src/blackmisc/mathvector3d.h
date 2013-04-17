#ifndef BLACKMISC_MATHVECTOR3D_H
#define BLACKMISC_MATHVECTOR3D_H

#include "blackmisc/mathvector3dbase.h"

namespace BlackMisc
{
namespace Math
{

/*!
 * \brief Concrete vector implementation
 */
class CVector3D : public CVector3DBase<CVector3D>
{
public:
    /*!
     * \brief Default constructor
     */
    CVector3D() : CVector3DBase() {}

    /*!
     * \brief Constructor by value
     * \param i
     * \param j
     * \param k
     */
    CVector3D(qreal i, qreal j, qreal k) : CVector3DBase(i, j, k) {}

    /*!
     * \brief Constructor by value
     * \param value
     */
    CVector3D(qreal value) : CVector3DBase(value) {}

    /*!
     * \brief i
     * \return
     */
    qreal i() const
    {
        return this->m_vector.x();
    }

    /*!
     * \brief j
     * \return
     */
    qreal j() const
    {
        return this->m_vector.y();
    }

    /*!
     * \brief k
     * \return
     */
    qreal k() const
    {
        return this->m_vector.z();
    }

    /*!
     * \brief Set i
     * \param i
     */
    void setI(qreal i)
    {
        this->m_vector.setX(i);
    }

    /*!
     * \brief Set j
     * \param j
     */
    void setJ(qreal j)
    {
        this->m_vector.setY(j);
    }

    /*!
     * \brief Set k
     * \param k
     */
    void setK(qreal k)
    {
        this->m_vector.setZ(k);
    }

};

} // namespace
} // namespace
#endif // guard
