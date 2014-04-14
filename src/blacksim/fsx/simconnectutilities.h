#ifndef BLACKSIM_FSX_SIMCONNECTUTILITIES_H
#define BLACKSIM_FSX_SIMCONNECTUTILITIES_H

#include <QString>
#include <QMetaEnum>

// apart from the below definitions, the following code is OS independent,
// though it does not make sense to be used on non WIN machines.
// But it allows parts as GUI to comile
#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#else
typedef unsigned long   DWORD;
#endif

namespace BlackSim
{
    namespace Fsx
    {
        //! \brief Utilities for SimConnect
        class CSimConnectUtilities : public QObject
        {
            Q_OBJECT
            Q_ENUMS(SIMCONNECT_EXCEPTION)
            Q_ENUMS(SIMCONNECT_SURFACE)

        private:
            //! \brief CSimConnect
            CSimConnectUtilities();

        public:
            //! Path to local config file
            static QString getLocalSimConnectCfgFilename();

            /*!
             * \brief Create a FSX simconnect.cfg file
             * \param fileName and path
             * \param ip IP address of FSX
             * \param port Port of FSX (e.g. 500)
             * \return success
             */
            static bool writeSimConnectCfg(const QString &fileName, const QString &ip, quint16 port);

            /*!
             * \brief Resolve SimConnect exception (based on Qt metadata).
             * \param id enum element
             * \return enum element's name
             */
            static const QString simConnectExceptionToString(const DWORD id);

            /*!
             * \brief Resolve SimConnect surface (based on Qt metadata).
             * \param type enum element
             * \param beautify remove _
             * \return
             */
            static const QString simConnectSurfaceTypeToString(const DWORD type, bool beautify = true);

            //! SimConnect surfaces.
            // http://msdn.microsoft.com/en-us/library/cc526981.aspx#AircraftFlightInstrumentationData
            enum SIMCONNECT_SURFACE
            {
                Concrete,
                Grass,
                Water,
                Grass_bumpy,
                Asphalt,
                Short_grass,
                Long_grass,
                Hard_turf,
                Snow,
                Ice,
                Urban,
                Forest,
                Dirt,
                Coral,
                Gravel,
                Oil_treated,
                Steel_mats,
                Bituminus,
                Brick,
                Macadam,
                Planks,
                Sand,
                Shale,
                Tarmac,
                Wright_flyer_track
            };

            //! SimConnect exceptions.
            enum SIMCONNECT_EXCEPTION
            {
                SIMCONNECT_EXCEPTION_NONE,
                SIMCONNECT_EXCEPTION_ERROR,
                SIMCONNECT_EXCEPTION_SIZE_MISMATCH,
                SIMCONNECT_EXCEPTION_UNRECOGNIZED_ID,
                SIMCONNECT_EXCEPTION_UNOPENED,
                SIMCONNECT_EXCEPTION_VERSION_MISMATCH,
                SIMCONNECT_EXCEPTION_TOO_MANY_GROUPS,
                SIMCONNECT_EXCEPTION_NAME_UNRECOGNIZED,
                SIMCONNECT_EXCEPTION_TOO_MANY_EVENT_NAMES,
                SIMCONNECT_EXCEPTION_EVENT_ID_DUPLICATE,
                SIMCONNECT_EXCEPTION_TOO_MANY_MAPS,
                SIMCONNECT_EXCEPTION_TOO_MANY_OBJECTS,
                SIMCONNECT_EXCEPTION_TOO_MANY_REQUESTS,
                SIMCONNECT_EXCEPTION_WEATHER_INVALID_PORT,
                SIMCONNECT_EXCEPTION_WEATHER_INVALID_METAR,
                SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_GET_OBSERVATION,
                SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_CREATE_STATION,
                SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_REMOVE_STATION,
                SIMCONNECT_EXCEPTION_INVALID_DATA_TYPE,
                SIMCONNECT_EXCEPTION_INVALID_DATA_SIZE,
                SIMCONNECT_EXCEPTION_DATA_ERROR,
                SIMCONNECT_EXCEPTION_INVALID_ARRAY,
                SIMCONNECT_EXCEPTION_CREATE_OBJECT_FAILED,
                SIMCONNECT_EXCEPTION_LOAD_FLIGHTPLAN_FAILED,
                SIMCONNECT_EXCEPTION_OPERATION_INVALID_FOR_OBJECT_TYPE,
                SIMCONNECT_EXCEPTION_ILLEGAL_OPERATION,
                SIMCONNECT_EXCEPTION_ALREADY_SUBSCRIBED,
                SIMCONNECT_EXCEPTION_INVALID_ENUM,
                SIMCONNECT_EXCEPTION_DEFINITION_ERROR,
                SIMCONNECT_EXCEPTION_DUPLICATE_ID,
                SIMCONNECT_EXCEPTION_DATUM_ID,
                SIMCONNECT_EXCEPTION_OUT_OF_BOUNDS,
                SIMCONNECT_EXCEPTION_ALREADY_CREATED,
                SIMCONNECT_EXCEPTION_OBJECT_OUTSIDE_REALITY_BUBBLE,
                SIMCONNECT_EXCEPTION_OBJECT_CONTAINER,
                SIMCONNECT_EXCEPTION_OBJECT_AI,
                SIMCONNECT_EXCEPTION_OBJECT_ATC,
                SIMCONNECT_EXCEPTION_OBJECT_SCHEDULE
            };

            //! \brief Register metadata
            static void registerMetadata();

        private:
            /*!
             * \brief Resolve enum value to its cleartext (based on Qt metadata).
             * \param id enum element
             * \param enumName name of the resolved enum
             * \return enum element's name
             */
            static const QString resolveEnumToString(const DWORD id, const char *enumName);
        };
    }
}

Q_DECLARE_METATYPE(BlackSim::Fsx::CSimConnectUtilities::SIMCONNECT_EXCEPTION)
Q_DECLARE_METATYPE(BlackSim::Fsx::CSimConnectUtilities::SIMCONNECT_SURFACE)

#endif // guard
