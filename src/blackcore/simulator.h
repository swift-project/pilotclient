//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "blackcore/sim_callbacks.h"
#include "blackmisc/coordinategeodetic.h"
#include "blackmisc/mathvector3d.h"
#include <QString>


#if defined(_MSC_VER) && _MSC_VER >= 1600
# include <unordered_map>
# include <functional>
#else
# include <tr1/unordered_map>
# include <tr1/functional>
#endif


#define SHARED_LIBRARY_NAME_FS9 "bb_driver_fs9"
#define SHARED_LIBRARY_NAME_FSX "bb_driver_fsx"
#define SHARED_LIBRARY_NAME_XPLANE "bb_driver_xplane"

/*!
 * \file Simulator driver interface.
 */

namespace BlackMisc
{

class IContext;

}

namespace BlackCore
{

/*!
 * A struct to hold data about an aircraft model and repaint.
 */
class CPlaneModel
{
public:
    QString name;           //!< full name of the aircraft model, arbitrary string (hopefully unique)
    QString typeCode;       //!< ICAO aircraft type code
    QString airlineCode;    //!< ICAO air operator code
};

/*!
 * A struct for passing around the physical state of an aircraft.
 */
class CPhysicalState
{
public:
    //! Constructor, initialize to zero.
    CPhysicalState() : headingDegrees(0), pitchDegrees(0), bankDegrees(0), groundSpeedKnots(0) {}
    BlackMisc::Geo::CCoordinateGeodetic position; //!< geographical position
    float headingDegrees;   //!< heading in degrees
    float pitchDegrees;     //!< pitch in degrees
    float bankDegrees;      //!< bank in degrees
    float groundSpeedKnots; //!< ground speed in knots
    BlackMisc::Math::CVector3D trueSpeedMetersPerSec;    //!< needed by FSX
};

/*!
 * A struct for passing around the avionics state of an aircraft.
 */
class CAvionicsState
{
    //! Constructor, initialize to zero.
    CAvionicsState() : squawkCode(0), squawkModeC(false), squawkIdent(false),
        com1FreqHz(0), com2FreqHz(0)
    {}
    qint16 squawkCode;  //!< decimal squawk code
    bool squawkModeC;   //!< true if squawking mode C
    bool squawkIdent;   //!< true if squawking ident
    qint32 com1FreqHz;  //!< COM1 radio frequency in Hz
    qint32 com2FreqHz;  //!< COM2 radio frequency in Hz
};

/*!
 * A struct for passing around the animation state of an aircraft.
 */
class CAnimationState
{
public:
    //! Constructor, initialize to zero.
    CAnimationState() : gearPercent(0), flapsPercent(0), landingLights(false),
        taxiLights(false), navLights(false), strobeLights(false), beaconLights(false)
    {}
    qint8 gearPercent;  //!< 0 = retracted, 100 = extended
    qint8 flapsPercent; //!< 0 = ratracted, 100 = extended
    bool landingLights; //!< true if landing lights on
    bool taxiLights;    //!< true if taxi lights on
    bool navLights;     //!< true if nav lights on
    bool strobeLights;  //!< true if strobe lights on
    bool beaconLights;  //!< true if beacon lights on
};

#ifdef Q_OS_WIN

//! A callback that is called when the simulator is started.
typedef std::tr1::function<void(const bool status)> cbSimStarted;

//! A callback that is called when the user's plane changes its avionics state.
typedef std::tr1::function<void(const CAvionicsState &state)> cbChangedAvionicsState;

//! A callback that is called when the user's plane changes its animation state.
typedef std::tr1::function<void(const CAnimationState &state)> cbChangedAnimationState;

//! A callback that is called when the user's plane changes its model.
typedef std::tr1::function<void(const CPlaneModel &model)> cbChangedModel;

#else
//! A callback that is called when the simulator is started.
typedef std::function<void(const bool status)> cbSimStarted;

//! A callback that is called when the user's plane changes its avionics state.
typedef std::function<void(const CAvionicsState &state)> cbChangedAvionicsState;

//! A callback that is called when the user's plane changes its animation state.
typedef std::function<void(const CAnimationState &state)> cbChangedAnimationState;

//! A callback that is called when the user's plane changes its model.
typedef std::function<void(const CPlaneModel &model)> cbChangedModel;
#endif

/*!
 * The interface that is implemented by each simulator driver.
 *
 * Simulator drivers are responsible for communicating with the simulator on the user's
 * computer and keeping it in sync with the client.
 */
class ISimulator
{
public:
    // Version of the driver interface. To increment when the interface change.
    static const quint32         InterfaceVersionMajor;
    static const quint32         InterfaceVersionMinor;

    //! Enumeration to describe which simulator is desired.
    enum ESimulator
    {
        FS9 = 0,    //!< Microsoft Flight Simulator 9
        FSX,        //!< Microsoft Flight Simulator 10
        XPLANE      //!< X-Plane
    };

    //! Constructor.
    ISimulator() {}

    //! Destructor.
    virtual ~ISimulator() {}

    //! Provide the driver with a pointer to the global context.
    virtual void setLibraryContext(BlackMisc::IContext *context);

    //! Factory method.
    static ISimulator *createDriver(ESimulator sim);

    //! Initialize the driver.
    virtual int init() = 0;

    //! Connect to the simulator.
    virtual int connect() = 0;

    //! Provide a callback to be called when the simulation starts.
    virtual void setcbSimStarted(const cbSimStarted &func);

    //! Query whether the driver is connected to the simulator.
    virtual bool isConnected() = 0;

    //! If there has been an error, return the associated message.
    virtual QString getLastErrorMessage() = 0;

    //! Provide a callback to be called when the user plane changes avionics state.
    virtual void setcbChangedAvionicsState(const cbChangedAvionicsState &func);

    //! Provide a callback to be called when the user plane changes animation state.
    virtual void setcbChangedAnimationState(const cbChangedAnimationState &func);

    //! Provide a callback to be called when the user plane changes model.
    virtual void setcbChangedModel(const cbChangedModel &func);

    //! Returns true if the user plane is in contact with the ground.
    virtual bool isOnGround() = 0;

    /*!
     * Returns the physical state of the user plane.
     * \warning This might block - use QtConcurrent::run if that is a problem.
     */
    virtual CPhysicalState getPhysicalState() = 0;

    /*!
     * Adds a plane to the simulator traffic and returns its handle.
     * \warning This might block - use QtConcurrent::run if that is a problem
     */
    virtual qint32 addPlane(const QString &callsign) = 0;

    /*!
     * Remove a plane from the simulator traffic.
     * \param planeID a handle that was returned by addPlane().
     */
    virtual bool removePlane(const qint32 planeID) = 0;

    /*!
     * Set the model of an aircraft in the simulator traffic.
     * \param planeID a handle that was returned by addPlane().
     */
    virtual void setModel(const qint32 planeID, const CPlaneModel &model) = 0;

    /*!
     * Set the physical state of an aircraft in the simulator traffic.
     * \param planeID a handle that was returned by addPlane().
     */
    virtual bool setPhysicalState(const qint32 planeID, const CPhysicalState &state) = 0;

    /*!
     * Set the animation state of an aircraft in the simulator traffic.
     * \param planeID a handle that was returned by addPlane().
     */
    virtual bool setAnimationState(const qint32 planeID, const CAnimationState &state) = 0;

    //! Calls the supplied visitor function once for every model available in the simulator.

#ifdef Q_OS_WIN
    virtual void visitAllModels(const std::tr1::function<void(const CPlaneModel &)> &visitor) = 0;
#else
    virtual void visitAllModels(const std::function<void(const CPlaneModel &)> &visitor) = 0;
#endif

    /*!
     * Fills container with all models.
     * Class T must be a container of CPlaneModel objects and must support push_back.
     */
#ifdef Q_OS_WIN
    template <class T>
    void getAllModels(T &container) { visitAllModels(std::tr1::bind(T::push_back, container)); }
#else
    template <class T>
    void getAllModels(T &container) { visitAllModels(std::bind(T::push_back, container)); }
#endif


protected:
    BlackMisc::IContext *m_libraryContext;              //!< The global context.

    cbSimStarted m_cbSimStarted;                        //!< Callback to call when the sim starts.
    cbChangedAvionicsState m_cbChangedAvionicsState;    //!< Callback to call when the user plane changes avionics state.
    cbChangedAnimationState m_cbChangedAnimationState;  //!< Callback to call when the user plane changes animation state.
    cbChangedModel m_cbChangedModel;                    //!< Callback to call when the user plane changes model.
};

} //! namespace BlackCore

#endif // SIMULATOR_H
