# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(core SHARED
        # AFV
        afv/audio/callsigndelaycache.cpp
        afv/audio/callsigndelaycache.h
        afv/audio/callsignsampleprovider.cpp
        afv/audio/callsignsampleprovider.h
        afv/audio/input.cpp
        afv/audio/input.h
        afv/audio/output.cpp
        afv/audio/output.h
        afv/audio/receiversampleprovider.cpp
        afv/audio/receiversampleprovider.h
        afv/audio/soundcardsampleprovider.cpp
        afv/audio/soundcardsampleprovider.h
        afv/clients/afvclient.cpp
        afv/clients/afvclient.h
        afv/connection/apiserverconnection.cpp
        afv/connection/apiserverconnection.h
        afv/connection/clientconnection.cpp
        afv/connection/clientconnection.h
        afv/connection/clientconnectiondata.cpp
        afv/connection/clientconnectiondata.h
        afv/constants.h
        afv/crypto/cryptodtochannel.cpp
        afv/crypto/cryptodtochannel.h
        afv/crypto/cryptodtoheaderdto.h
        afv/crypto/cryptodtomode.h
        afv/crypto/cryptodtoserializer.cpp
        afv/crypto/cryptodtoserializer.h
        afv/dto.h
        afv/model/afvmapreader.cpp
        afv/model/afvmapreader.h
        afv/model/atcstationmodel.cpp
        afv/model/atcstationmodel.h

        # Application
        application/applicationsettings.h
        application/updatesettings.h

        # Audio
        audio/audiosettings.h

        # Context
        context/context.cpp
        context/context.h
        context/contextapplication.cpp
        context/contextapplication.h
        context/contextapplicationempty.h
        context/contextapplicationimpl.cpp
        context/contextapplicationimpl.h
        context/contextapplicationproxy.cpp
        context/contextapplicationproxy.h
        context/contextaudio.cpp
        context/contextaudio.h
        context/contextaudioimpl.cpp
        context/contextaudioimpl.h
        context/contextaudioproxy.cpp
        context/contextaudioproxy.h
        context/contextnetwork.cpp
        context/contextnetwork.h
        context/contextnetworkempty.h
        context/contextnetworkimpl.cpp
        context/contextnetworkimpl.h
        context/contextnetworkproxy.cpp
        context/contextnetworkproxy.h
        context/contextownaircraft.cpp
        context/contextownaircraft.h
        context/contextownaircraftempty.h
        context/contextownaircraftimpl.cpp
        context/contextownaircraftimpl.h
        context/contextownaircraftproxy.cpp
        context/contextownaircraftproxy.h
        context/contextsimulator.cpp
        context/contextsimulator.h
        context/contextsimulatorempty.h
        context/contextsimulatorimpl.cpp
        context/contextsimulatorimpl.h
        context/contextsimulatorproxy.cpp
        context/contextsimulatorproxy.h

        # Data
        data/authenticateduser.h
        data/dbcaches.h
        data/globalsetup.cpp
        data/globalsetup.h
        data/launchersetup.cpp
        data/launchersetup.h
        data/networksetup.cpp
        data/networksetup.h
        data/vatsimsetup.cpp
        data/vatsimsetup.h

        # DB
        db/airportdatareader.cpp
        db/airportdatareader.h
        db/backgrounddataupdater.cpp
        db/backgrounddataupdater.h
        db/databaseauthentication.cpp
        db/databaseauthentication.h
        db/databasereader.cpp
        db/databasereader.h
        db/databasereaderconfig.cpp
        db/databasereaderconfig.h
        db/databaseutils.cpp
        db/databaseutils.h
        db/databasewriter.cpp
        db/databasewriter.h
        db/icaodatareader.cpp
        db/icaodatareader.h
        db/infodatareader.cpp
        db/infodatareader.h
        db/modeldatareader.cpp
        db/modeldatareader.h

        # FSD
        fsd/addatc.cpp
        fsd/addatc.h
        fsd/addpilot.cpp
        fsd/addpilot.h
        fsd/atcdataupdate.cpp
        fsd/atcdataupdate.h
        fsd/authchallenge.cpp
        fsd/authchallenge.h
        fsd/authresponse.cpp
        fsd/authresponse.h
        fsd/clientidentification.cpp
        fsd/clientidentification.h
        fsd/clientquery.cpp
        fsd/clientquery.h
        fsd/clientresponse.cpp
        fsd/clientresponse.h
        fsd/deleteatc.cpp
        fsd/deleteatc.h
        fsd/deletepilot.cpp
        fsd/deletepilot.h
        fsd/enums.h
        fsd/euroscopesimdata.cpp
        fsd/euroscopesimdata.h
        fsd/flightplan.cpp
        fsd/flightplan.h
        fsd/fsdclient.cpp
        fsd/fsdclient.h
        fsd/fsdidentification.cpp
        fsd/fsdidentification.h
        fsd/interimpilotdataupdate.cpp
        fsd/interimpilotdataupdate.h
        fsd/killrequest.cpp
        fsd/killrequest.h
        fsd/messagebase.cpp
        fsd/messagebase.h
        fsd/mute.cpp
        fsd/mute.h
        fsd/pbh.h
        fsd/pilotdataupdate.cpp
        fsd/pilotdataupdate.h
        fsd/ping.cpp
        fsd/ping.h
        fsd/planeinforequest.cpp
        fsd/planeinforequest.h
        fsd/planeinforequestfsinn.cpp
        fsd/planeinforequestfsinn.h
        fsd/planeinformation.cpp
        fsd/planeinformation.h
        fsd/planeinformationfsinn.cpp
        fsd/planeinformationfsinn.h
        fsd/pong.cpp
        fsd/pong.h
        fsd/rehost.cpp
        fsd/rehost.h
        fsd/serializer.cpp
        fsd/serializer.h
        fsd/servererror.cpp
        fsd/servererror.h
        fsd/textmessage.cpp
        fsd/textmessage.h
        fsd/visualpilotdataperiodic.cpp
        fsd/visualpilotdataperiodic.h
        fsd/visualpilotdatastopped.cpp
        fsd/visualpilotdatastopped.h
        fsd/visualpilotdatatoggle.cpp
        fsd/visualpilotdatatoggle.h
        fsd/visualpilotdataupdate.cpp
        fsd/visualpilotdataupdate.h

        # VATSIM
        vatsim/vatsimdatafilereader.cpp
        vatsim/vatsimdatafilereader.h
        vatsim/vatsimmetarreader.cpp
        vatsim/vatsimmetarreader.h
        vatsim/vatsimserverfilereader.cpp
        vatsim/vatsimserverfilereader.h
        vatsim/vatsimsettings.cpp
        vatsim/vatsimsettings.h
        vatsim/vatsimstatusfilereader.cpp
        vatsim/vatsimstatusfilereader.h

        # Main
        actionbind.cpp
        actionbind.h
        aircraftmatcher.cpp
        aircraftmatcher.h
        airspaceanalyzer.cpp
        airspaceanalyzer.h
        airspacemonitor.cpp
        airspacemonitor.h
        application.cpp
        application.h
        cookiemanager.cpp
        cookiemanager.h
        corefacade.cpp
        corefacade.h
        corefacadeconfig.cpp
        corefacadeconfig.h
        coremodeenums.h
        githubpackagesreader.cpp
        githubpackagesreader.h
        inputmanager.cpp
        inputmanager.h
        modelsetbuilder.cpp
        modelsetbuilder.h
        pluginmanager.cpp
        pluginmanager.h
        pluginmanagersimulator.cpp
        pluginmanagersimulator.h
        progress.h
        registermetadata.cpp
        registermetadata.h
        setupreader.cpp
        setupreader.h
        simulator.cpp
        simulator.h
        swiftcoreexport.h
        threadedreader.cpp
        threadedreader.h
        threadedreaderperiodic.cpp
        threadedreaderperiodic.h
        webdataservices.cpp
        webdataservices.h
        webdataservicesms.cpp
        webdataservicesms.h
        webreaderflags.cpp
        webreaderflags.h
)


target_include_directories(core PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(core
        PUBLIC
            Qt::Core
            misc
            input
            sound
            Opus::opus
            libsodium::libsodium
            msgpack-cxx
            config
        PRIVATE
            Qt::Qml
            Qt::Xml
            QJsonWebToken
)

if(SWIFT_VATSIM_SUPPORT)
    target_link_libraries(core PUBLIC VATSIMAuth::VATSIMAuth)
endif()

target_compile_definitions(core PRIVATE BUILD_SWIFT_CORE_LIB)


if(SWIFT_USE_PCH)
    target_precompile_headers(core
            PRIVATE
                ${SWIFT_MISC_PCH}
                ${SWIFT_CORE_PCH}
    )
endif()

install(TARGETS core
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
