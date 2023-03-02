add_library(core SHARED
        # AFV
        afv/audio/output.h
        afv/audio/output.cpp
        afv/audio/callsigndelaycache.cpp
        afv/audio/soundcardsampleprovider.h
        afv/audio/callsignsampleprovider.h
        afv/audio/callsigndelaycache.h
        afv/audio/soundcardsampleprovider.cpp
        afv/audio/receiversampleprovider.cpp
        afv/audio/callsignsampleprovider.cpp
        afv/audio/input.h
        afv/audio/input.cpp
        afv/audio/receiversampleprovider.h
        afv/crypto/cryptodtochannel.cpp
        afv/crypto/cryptodtoserializer.cpp
        afv/crypto/cryptodtomode.h
        afv/crypto/cryptodtochannel.h
        afv/crypto/cryptodtoserializer.h
        afv/crypto/cryptodtoheaderdto.h
        afv/model/atcstationmodel.h
        afv/model/atcstationmodel.cpp
        afv/model/afvmapreader.h
        afv/model/afvmapreader.cpp
        afv/dto.h
        afv/clients/afvclient.h
        afv/clients/afvclient.cpp
        afv/connection/clientconnection.h
        afv/connection/apiserverconnection.cpp
        afv/connection/apiserverconnection.h
        afv/connection/clientconnection.cpp
        afv/connection/clientconnectiondata.cpp
        afv/connection/clientconnectiondata.h
        afv/constants.h

        # Application
        application/applicationsettings.h
        application/updatesettings.h

        # Audio
        audio/audiosettings.h

        # Context
        context/contextapplicationproxy.cpp
        context/contextownaircraftproxy.h
        context/context.h
        context/contextsimulator.h
        context/contextaudio.cpp
        context/contextaudio.h
        context/contextapplicationimpl.cpp
        context/contextsimulatorproxy.cpp
        context/contextownaircraft.h
        context/contextownaircraftimpl.cpp
        context/contextnetwork.cpp
        context/contextapplication.h
        context/contextsimulatorempty.h
        context/contextapplication.cpp
        context/contextaudioimpl.cpp
        context/contextapplicationempty.h
        context/contextownaircraft.cpp
        context/contextnetworkproxy.h
        context/contextsimulatorimpl.cpp
        context/contextapplicationproxy.h
        context/contextapplicationimpl.h
        context/contextsimulatorproxy.h
        context/contextaudioproxy.h
        context/contextaudioimpl.h
        context/contextownaircraftproxy.cpp
        context/contextsimulator.cpp
        context/contextaudioproxy.cpp
        context/contextnetwork.h
        context/context.cpp
        context/contextnetworkempty.h
        context/contextnetworkproxy.cpp
        context/contextownaircraftimpl.h
        context/contextsimulatorimpl.h
        context/contextnetworkimpl.cpp
        context/contextnetworkimpl.h
        context/contextownaircraftempty.h

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
        db/backgrounddataupdater.cpp
        db/airportdatareader.cpp
        db/databaseutils.h
        db/modeldatareader.h
        db/infodatareader.h
        db/networkwatchdog.h
        db/databasereader.h
        db/databaseauthentication.cpp
        db/databasewriter.cpp
        db/databasereaderconfig.h
        db/networkwatchdog.cpp
        db/backgrounddataupdater.h
        db/databaseutils.cpp
        db/modeldatareader.cpp
        db/databasereader.cpp
        db/databasereaderconfig.cpp
        db/infodatareader.cpp
        db/icaodatareader.cpp
        db/databaseauthentication.h
        db/icaodatareader.h
        db/databasewriter.h
        db/airportdatareader.h

        # FSD
        fsd/fsdidentification.cpp
        fsd/clientresponse.cpp
        fsd/planeinforequestfsinn.cpp
        fsd/textmessage.cpp
        fsd/killrequest.cpp
        fsd/revbclientparts.cpp
        fsd/clientidentification.cpp
        fsd/visualpilotdatatoggle.h
        fsd/rehost.h
        fsd/visualpilotdataupdate.cpp
        fsd/planeinformationfsinn.h
        fsd/clientquery.h
        fsd/flightplan.cpp
        fsd/pong.cpp
        fsd/planeinformationfsinn.cpp
        fsd/addatc.h
        fsd/servererror.h
        fsd/servererror.cpp
        fsd/fsdclient.cpp
        fsd/atcdataupdate.h
        fsd/authchallenge.h
        fsd/atcdataupdate.cpp
        fsd/ping.h
        fsd/messagebase.h
        fsd/visualpilotdataupdate.h
        fsd/rehost.cpp
        fsd/interimpilotdataupdate.h
        fsd/authresponse.h
        fsd/addatc.cpp
        fsd/textmessage.h
        fsd/visualpilotdataperiodic.cpp
        fsd/visualpilotdatastopped.h
        fsd/authresponse.cpp
        fsd/pilotdataupdate.cpp
        fsd/ping.cpp
        fsd/messagebase.cpp
        fsd/blackfsd.pro
        fsd/enums.h
        fsd/deleteatc.h
        fsd/clientidentification.h
        fsd/revbclientparts.h
        fsd/deletepilot.h
        fsd/fsdclient.h
        fsd/flightplan.h
        fsd/pbh.h
        fsd/killrequest.h
        fsd/planeinforequest.h
        fsd/addpilot.h
        fsd/planeinformation.cpp
        fsd/clientquery.cpp
        fsd/addpilot.cpp
        fsd/deleteatc.cpp
        fsd/authchallenge.cpp
        fsd/planeinformation.h
        fsd/serializer.cpp
        fsd/euroscopesimdata.cpp
        fsd/pong.h
        fsd/euroscopesimdata.h
        fsd/interimpilotdataupdate.cpp
        fsd/clientresponse.h
        fsd/visualpilotdatatoggle.cpp
        fsd/planeinforequestfsinn.h
        fsd/planeinforequest.cpp
        fsd/deletepilot.cpp
        fsd/pilotdataupdate.h
        fsd/serializer.h
        fsd/visualpilotdatastopped.cpp
        fsd/fsdidentification.h
        fsd/visualpilotdataperiodic.h

        # VATSIM
        vatsim/vatsimbookingreader.cpp
        vatsim/vatsimbookingreader.h
        vatsim/vatsimdatafilereader.cpp
        vatsim/vatsimdatafilereader.h
        vatsim/vatsimmetarreader.cpp
        vatsim/vatsimmetarreader.h
        vatsim/vatsimsettings.cpp
        vatsim/vatsimsettings.h
        vatsim/vatsimstatusfilereader.cpp
        vatsim/vatsimstatusfilereader.h
        vatsim/vatsimserverfilereader.cpp
        vatsim/vatsimserverfilereader.h

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
        blackcoreexport.h
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
        pluginmanagerweatherdata.cpp
        pluginmanagerweatherdata.h
        progress.h
        registermetadata.cpp
        registermetadata.h
        setupreader.cpp
        setupreader.h
        simulator.cpp
        simulator.h
        threadedreader.cpp
        threadedreader.h
        weatherdata.h
        weathermanager.cpp
        weathermanager.h
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
            externals_sodium
            externals_msgpack
            blackconfig
            externals_vatsim_auth
        PRIVATE
            Qt::Qml
            Qt::Xml
)

target_compile_definitions(core PRIVATE BUILD_BLACKCORE_LIB)

install(TARGETS core
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
