# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_subdirectory(share/dbus-1)

add_library(misc SHARED
        # Audio
        audio/audiodeviceinfo.cpp
        audio/audiodeviceinfo.h
        audio/audiodeviceinfolist.cpp
        audio/audiodeviceinfolist.h
        audio/audiosettings.cpp
        audio/audiosettings.h
        audio/audioutils.cpp
        audio/audioutils.h
        audio/notificationsounds.cpp
        audio/notificationsounds.h
        audio/registermetadataaudio.cpp
        audio/registermetadataaudio.h

        # Aviation
        aviation/aircraftcategory.cpp
        aviation/aircraftcategory.h
        aviation/aircraftcategorylist.cpp
        aviation/aircraftcategorylist.h
        aviation/aircraftengine.cpp
        aviation/aircraftengine.h
        aviation/aircraftenginelist.cpp
        aviation/aircraftenginelist.h
        aviation/aircrafticaocode.cpp
        aviation/aircrafticaocode.h
        aviation/aircrafticaocodelist.cpp
        aviation/aircrafticaocodelist.h
        aviation/aircraftlights.cpp
        aviation/aircraftlights.h
        aviation/aircraftparts.cpp
        aviation/aircraftparts.h
        aviation/aircraftpartslist.cpp
        aviation/aircraftpartslist.h
        aviation/aircraftsituation.cpp
        aviation/aircraftsituation.h
        aviation/aircraftsituationchange.cpp
        aviation/aircraftsituationchange.h
        aviation/aircraftsituationchangelist.cpp
        aviation/aircraftsituationchangelist.h
        aviation/aircraftsituationlist.cpp
        aviation/aircraftsituationlist.h
        aviation/aircraftvelocity.cpp
        aviation/aircraftvelocity.h
        aviation/airlineicaocode.cpp
        aviation/airlineicaocode.h
        aviation/airlineicaocodelist.cpp
        aviation/airlineicaocodelist.h
        aviation/airport.cpp
        aviation/airport.h
        aviation/airporticaocode.cpp
        aviation/airporticaocode.h
        aviation/airportlist.cpp
        aviation/airportlist.h
        aviation/altitude.cpp
        aviation/altitude.h
        aviation/atcstation.cpp
        aviation/atcstation.h
        aviation/atcstationlist.cpp
        aviation/atcstationlist.h
        aviation/callsign.cpp
        aviation/callsign.h
        aviation/callsignobjectlist.h
        aviation/callsignset.cpp
        aviation/callsignset.h
        aviation/comnavequipment.cpp
        aviation/comnavequipment.h
        aviation/comsystem.cpp
        aviation/comsystem.h
        aviation/flightplan.cpp
        aviation/flightplan.h
        aviation/flightplanaircraftinfo.cpp
        aviation/flightplanaircraftinfo.h
        aviation/flightplanlist.cpp
        aviation/flightplanlist.h
        aviation/heading.cpp
        aviation/heading.h
        aviation/informationmessage.cpp
        aviation/informationmessage.h
        aviation/livery.cpp
        aviation/livery.h
        aviation/liverylist.cpp
        aviation/liverylist.h
        aviation/modulator.cpp
        aviation/modulator.h
        aviation/navsystem.h
        aviation/percallsign.h
        aviation/registermetadataaviation.cpp
        aviation/registermetadataaviation.h
        aviation/selcal.cpp
        aviation/selcal.h
        aviation/simbriefdata.cpp
        aviation/simbriefdata.h
        aviation/ssrequipment.cpp
        aviation/ssrequipment.h
        aviation/track.cpp
        aviation/track.h
        aviation/transponder.cpp
        aviation/transponder.h
        aviation/waketurbulencecategory.cpp
        aviation/waketurbulencecategory.h

        # DB
        db/artifact.cpp
        db/artifact.h
        db/artifactlist.cpp
        db/artifactlist.h
        db/datastore.cpp
        db/datastore.h
        db/datastoreobjectlist.h
        db/datastoreutility.cpp
        db/datastoreutility.h
        db/dbflags.cpp
        db/dbflags.h
        db/dbinfo.cpp
        db/dbinfo.h
        db/dbinfolist.cpp
        db/dbinfolist.h
        db/distribution.cpp
        db/distribution.h
        db/distributionlist.cpp
        db/distributionlist.h
        db/registermetadatadb.cpp
        db/registermetadatadb.h
        db/updateinfo.cpp
        db/updateinfo.h

        # Geo
        geo/coordinategeodetic.cpp
        geo/coordinategeodetic.h
        geo/coordinategeodeticlist.cpp
        geo/coordinategeodeticlist.h
        geo/earthangle.cpp
        geo/earthangle.h
        geo/elevationplane.cpp
        geo/elevationplane.h
        geo/geoobjectlist.h
        geo/kmlutils.cpp
        geo/kmlutils.h
        geo/latitude.h
        geo/longitude.h
        geo/registermetadatageo.cpp
        geo/registermetadatageo.h

        # Input
        input/actionhotkey.cpp
        input/actionhotkey.h
        input/actionhotkeydefs.cpp
        input/actionhotkeydefs.h
        input/actionhotkeylist.cpp
        input/actionhotkeylist.h
        input/hotkeycombination.cpp
        input/hotkeycombination.h
        input/joystickbutton.cpp
        input/joystickbutton.h
        input/joystickbuttonlist.cpp
        input/joystickbuttonlist.h
        input/keyboardkey.cpp
        input/keyboardkey.h
        input/keyboardkeylist.cpp
        input/keyboardkeylist.h
        input/keycodes.h
        input/registermetadatainput.cpp
        input/registermetadatainput.h

        # Main
        algorithm.h
        applicationinfo.h
        applicationinfo.cpp
        applicationinfolist.cpp
        applicationinfolist.h
        atomicfile.cpp
        atomicfile.h
        blackmiscexport.h
        cachesettingsutils.cpp
        cachesettingsutils.h
        collection.h
        comparefunctions.h
        compressutils.cpp
        compressutils.h
        connectionguard.cpp
        connectionguard.h
        containerbase.h
        country.cpp
        country.h
        countrylist.cpp
        countrylist.h
        cputime.cpp
        cputime.h
        crashhandler.cpp
        crashhandler.h
        crashinfo.cpp
        crashinfo.h
        crashsettings.cpp
        crashsettings.h
        datacache.cpp
        datacache.h
        datastream.h
        dbus.cpp
        dbus.h
        dbusserver.cpp
        dbusserver.h
        dbusutils.cpp
        dbusutils.h
        dictionary.h
        digestsignal.cpp
        digestsignal.h
        directories.cpp
        directories.h
        directoryutils.cpp
        directoryutils.h
        eventloop.h
        filelogger.cpp
        filelogger.h
        fileutils.cpp
        fileutils.h
        genericdbusinterface.h
        htmlutils.cpp
        htmlutils.h
        icon.cpp
        icon.h
        iconlist.cpp
        iconlist.h
        icons.cpp
        icons.h
        identifiable.cpp
        identifiable.h
        identifier.cpp
        identifier.h
        identifierlist.cpp
        identifierlist.h
        identifierset.cpp
        identifierset.h
        imageutils.cpp
        imageutils.h
        inheritancetraits.h
        integersequence.h
        invoke.h
        iterator.h
        json.cpp
        json.h
        jsonexception.cpp
        jsonexception.h
        lockfree.h
        logcategories.h
        logcategory.cpp
        logcategory.h
        logcategorylist.cpp
        logcategorylist.h
        loghandler.cpp
        loghandler.h
        loghistory.cpp
        loghistory.h
        logmessage.cpp
        logmessage.h
        logpattern.cpp
        logpattern.h
        mapbuilder.h
        memotable.h
        metaclass.h
        metadatautils.cpp
        metadatautils.h
        namevariantpair.cpp
        namevariantpair.h
        namevariantpairlist.cpp
        namevariantpairlist.h
        obfuscation.cpp
        obfuscation.h
        optional.h
        orderable.cpp
        orderable.h
        orderablelist.h
        pixmap.cpp
        pixmap.h
        platform.cpp
        platform.h
        platformset.cpp
        platformset.h
        predicates.h
        processctrl.cpp
        processctrl.h
        processinfo.cpp
        processinfo.h
        promise.h
        propertyindex.cpp
        propertyindex.h
        propertyindexallclasses.h
        propertyindexlist.cpp
        propertyindexlist.h
        propertyindexref.cpp
        propertyindexref.h
        propertyindexvariantmap.cpp
        propertyindexvariantmap.h
        provider.cpp
        provider.h
        range.h
        registermetadata.cpp
        registermetadata.h
        restricted.h
        rgbcolor.cpp
        rgbcolor.h
        sequence.cpp
        sequence.h
        setbuilder.h
        settingscache.cpp
        settingscache.h
        simplecommandparser.cpp
        simplecommandparser.h
        slot.h
        stacktrace.cpp
        stacktrace.h
        statusexception.cpp
        statusexception.h
        statusmessage.cpp
        statusmessage.h
        statusmessagelist.cpp
        statusmessagelist.h
        stringutils.cpp
        stringutils.h
        swiftdirectories.cpp
        swiftdirectories.h
        threadutils.cpp
        threadutils.h
        timestampbased.cpp
        timestampbased.h
        timestampobjectlist.h
        tokenbucket.cpp
        tokenbucket.h
        tuple.h
        typetraits.h
        valuecache.cpp
        valuecache.h
        valuecacheprivate.h
        valueobject.h
        variant.cpp
        variant.h
        variantlist.cpp
        variantlist.h
        variantmap.cpp
        variantmap.h
        variantprivate.h
        verify.cpp
        verify.h
        windllutils.cpp
        windllutils.h
        worker.cpp
        worker.h
        blackmisc.qrc

        # Math
        math/constants.h
        math/mathutils.cpp
        math/mathutils.h

        # Mixin
        mixin/mixincompare.h
        mixin/mixindatastream.h
        mixin/mixindbus.h
        mixin/mixinhash.h
        mixin/mixinicon.h
        mixin/mixinindex.h
        mixin/mixinjson.h
        mixin/mixinmetatype.h
        mixin/mixinstring.h

        # Network
        network/data/lastserver.h
        network/settings/serversettings.h
        network/authenticateduser.cpp
        network/authenticateduser.h
        network/client.cpp
        network/client.h
        network/clientlist.cpp
        network/clientlist.h
        network/clientprovider.cpp
        network/clientprovider.h
        network/connectionstatus.cpp
        network/connectionstatus.h
        network/ecosystem.cpp
        network/ecosystem.h
        network/ecosystemlist.cpp
        network/ecosystemlist.h
        network/ecosystemprovider.cpp
        network/ecosystemprovider.h
        network/entityflags.cpp
        network/entityflags.h
        network/facilitytype.cpp
        network/facilitytype.h
        network/fsdsetup.cpp
        network/fsdsetup.h
        network/loginmode.cpp
        network/loginmode.h
        network/networkutils.cpp
        network/networkutils.h
        network/ping.cpp
        network/ping.h
        network/rawfsdmessage.cpp
        network/rawfsdmessage.h
        network/rawfsdmessagelist.cpp
        network/rawfsdmessagelist.h
        network/registermetadatanetwork.cpp
        network/registermetadatanetwork.h
        network/remotefile.cpp
        network/remotefile.h
        network/remotefilelist.cpp
        network/remotefilelist.h
        network/role.cpp
        network/role.h
        network/rolelist.cpp
        network/rolelist.h
        network/server.cpp
        network/server.h
        network/serverlist.cpp
        network/serverlist.h
        network/textmessage.cpp
        network/textmessage.h
        network/textmessagelist.cpp
        network/textmessagelist.h
        network/url.cpp
        network/url.h
        network/urllog.cpp
        network/urllog.h
        network/urlloglist.cpp
        network/urlloglist.h
        network/user.cpp
        network/user.h
        network/userlist.cpp
        network/userlist.h
        network/voicecapabilities.cpp
        network/voicecapabilities.h

        # PQ
        pq/acceleration.h
        pq/angle.cpp
        pq/angle.h
        pq/constants.h
        pq/frequency.h
        pq/length.h
        pq/literals.cpp
        pq/literals.h
        pq/mass.h
        pq/measurementunit.cpp
        pq/measurementunit.h
        pq/physicalquantity.cpp
        pq/physicalquantity.h
        pq/pqstring.cpp
        pq/pqstring.h
        pq/pressure.h
        pq/registermetadatapq.cpp
        pq/registermetadatapq.h
        pq/speed.h
        pq/temperature.h
        pq/time.cpp
        pq/time.h
        pq/units.cpp
        pq/units.h

        # Shared state
        sharedstate/dbus/duplex.cpp
        sharedstate/dbus/duplex.h
        sharedstate/dbus/dupleximpl.cpp
        sharedstate/dbus/dupleximpl.h
        sharedstate/dbus/duplexproxy.cpp
        sharedstate/dbus/duplexproxy.h
        sharedstate/dbus/hub.cpp
        sharedstate/dbus/hub.h
        sharedstate/dbus/hubimpl.cpp
        sharedstate/dbus/hubimpl.h
        sharedstate/dbus/hubproxy.cpp
        sharedstate/dbus/hubproxy.h
        sharedstate/activemutator.cpp
        sharedstate/activemutator.h
        sharedstate/activeobserver.cpp
        sharedstate/activeobserver.h
        sharedstate/datalink.cpp
        sharedstate/datalink.h
        sharedstate/datalinkdbus.cpp
        sharedstate/datalinkdbus.h
        sharedstate/datalinklocal.cpp
        sharedstate/datalinklocal.h
        sharedstate/listjournal.cpp
        sharedstate/listjournal.h
        sharedstate/listmutator.cpp
        sharedstate/listmutator.h
        sharedstate/listobserver.cpp
        sharedstate/listobserver.h
        sharedstate/passivemutator.cpp
        sharedstate/passivemutator.h
        sharedstate/passiveobserver.cpp
        sharedstate/passiveobserver.h
        sharedstate/scalarjournal.cpp
        sharedstate/scalarjournal.h
        sharedstate/scalarmutator.cpp
        sharedstate/scalarmutator.h
        sharedstate/scalarobserver.cpp
        sharedstate/scalarobserver.h

        # Simulation
        simulation/distributorlistpreferences.h
        simulation/aircraftmatchersetup.h
        simulation/simulatorplugininfo.cpp
        simulation/matchingutils.cpp
        simulation/categorymatcher.cpp
        simulation/matchingstatisticsentry.cpp
        simulation/matchinglog.cpp
        simulation/simulatorinfo.h
        simulation/simulatedaircraftlist.h
        simulation/ownaircraftproviderdummy.h
        simulation/matchingscript.cpp
        simulation/aircraftmodelloaderprovider.h
        simulation/aircraftmodelsetprovider.cpp
        simulation/simulatorinternals.cpp
        simulation/fscommon/vpilotrulesreader.h
        simulation/fscommon/vpilotmodelruleset.cpp
        simulation/fscommon/aircraftcfgparser.cpp
        simulation/fscommon/vpilotrulesreader.cpp
        simulation/fscommon/aircraftcfgentries.h
        simulation/fscommon/fscommonutil.h
        simulation/fscommon/bcdconversions.h
        simulation/fscommon/fscommonutil.cpp
        simulation/fscommon/fsdirectories.cpp
        simulation/fscommon/aircraftcfgentrieslist.cpp
        simulation/fscommon/fsdirectories.h
        simulation/fscommon/bcdconversions.cpp
        simulation/fscommon/aircraftcfgentrieslist.h
        simulation/fscommon/aircraftcfgentries.cpp
        simulation/fscommon/vpilotmodelrule.cpp
        simulation/fscommon/vpilotmodelrule.h
        simulation/fscommon/fscommon.h
        simulation/fscommon/aircraftcfgparser.h
        simulation/fscommon/vpilotmodelruleset.h
        simulation/aircraftmodel.cpp
        simulation/distributor.cpp
        simulation/interpolatorspline.h
        simulation/distributorlist.cpp
        simulation/registermetadatasimulation.cpp
        simulation/interpolatormulti.h
        simulation/interpolationsetupprovider.h
        simulation/aircraftmodellist.h
        simulation/interpolationsetupprovider.cpp
        simulation/interpolatorlinear.h
        simulation/backgroundvalidation.cpp
        simulation/airspaceaircraftsnapshot.cpp
        simulation/autopublishdata.h
        simulation/interpolationrenderingsetup.h
        simulation/matchingscriptmisc.h
        simulation/interpolationsetuplist.cpp
        simulation/remoteaircraftprovider.h
        simulation/interpolant.h
        simulation/ownaircraftprovider.cpp
        simulation/backgroundvalidation.h
        simulation/xplane/xswiftbusconfigwriter.cpp
        simulation/xplane/navdatareference.h
        simulation/xplane/navdatareference.cpp
        simulation/xplane/xplaneutil.h
        simulation/xplane/aircraftmodelloaderxplane.h
        simulation/xplane/xplaneutil.cpp
        simulation/xplane/qtfreeutils.h
        simulation/xplane/aircraftmodelloaderxplane.cpp
        simulation/xplane/xswiftbusconfigwriter.h
        simulation/matchingutils.h
        simulation/registermetadatasimulation.h
        simulation/interpolatorpbh.h
        simulation/fsx/simconnectutilities.h
        simulation/fsx/simconnectutilities.cpp
        simulation/fsx/fsx.h
        simulation/aircraftmodelinterfaces.h
        simulation/interpolationlogger.h
        simulation/interpolatormulti.cpp
        simulation/remoteaircraftprovider.cpp
        simulation/modelconverterx.cpp
        simulation/simulatorinfolist.h
        simulation/simulationenvironmentprovider.cpp
        simulation/interpolator.h
        simulation/interpolatorspline.cpp
        simulation/aircraftmodelloaderprovider.cpp
        simulation/simulatorplugininfolist.h
        simulation/remoteaircraftproviderdummy.cpp
        simulation/interpolationlogger.cpp
        simulation/matchingscript.h
        simulation/categorymatcher.h
        simulation/flightgear/flightgearutil.h
        simulation/flightgear/aircraftmodelloaderflightgear.h
        simulation/flightgear/aircraftmodelloaderflightgear.cpp
        simulation/flightgear/flightgearutil.cpp
        simulation/aircraftmodelloader.h
        simulation/simulatedaircraft.cpp
        simulation/aircraftmodellist.cpp
        simulation/aircraftmodel.h
        simulation/interpolatorlinear.cpp
        simulation/data/lastmodel.h
        simulation/data/modelcaches.h
        simulation/data/modelcaches.cpp
        simulation/airspaceaircraftsnapshot.h
        simulation/interpolatorfunctions.h
        simulation/ownaircraftprovider.h
        simulation/distributorlistpreferences.cpp
        simulation/interpolatorpbh.cpp
        simulation/reverselookup.h
        simulation/autopublishdata.cpp
        simulation/simulationenvironmentprovider.h
        simulation/simulatorplugininfo.h
        simulation/interpolationrenderingsetup.cpp
        simulation/simulatorinfo.cpp
        simulation/matchingstatistics.cpp
        simulation/ownaircraftproviderdummy.cpp
        simulation/simulatorinfolist.cpp
        simulation/aircraftmodelloader.cpp
        simulation/distributor.h
        simulation/matchinglog.h
        simulation/remoteaircraftproviderdummy.h
        simulation/simulatorplugininfolist.cpp
        simulation/aircraftmodelsetprovider.h
        simulation/simulatorinternals.h
        simulation/distributorlist.h
        simulation/aircraftmodelutils.h
        simulation/interpolator.cpp
        simulation/matchingstatistics.h
        simulation/modelconverterx.h
        simulation/settings/modelsettings.cpp
        simulation/settings/simulatorsettings.cpp
        simulation/settings/xswiftbussettings.cpp
        simulation/settings/xswiftbussettings.h
        simulation/settings/fgswiftbussettings.h
        simulation/settings/xswiftbussettingsqtfree.h
        simulation/settings/modelsettings.h
        simulation/settings/xswiftbussettingsqtfree.inc
        simulation/settings/fgswiftbussettings.h
        simulation/settings/fgswiftbussettings.cpp
        simulation/settings/modelmatchersettings.h
        simulation/settings/swiftpluginsettings.cpp
        simulation/settings/interpolationrenderingsetupsettings.h
        simulation/settings/simulatorsettings.h
        simulation/settings/swiftpluginsettings.h
        simulation/aircraftmatchersetup.cpp
        simulation/matchingscriptmisc.cpp
        simulation/simulatedaircraft.h
        simulation/aircraftmodelutils.cpp
        simulation/simulatedaircraftlist.cpp
        simulation/matchingstatisticsentry.h
        simulation/interpolationsetuplist.h

        # Test
        test/testdata.cpp
        test/testdata.h
        test/testing.cpp
        test/testing.h
        test/testservice.cpp
        test/testservice.h
        test/testserviceinterface.cpp
        test/testserviceinterface.h

        # Weather
        weather/cloudlayer.cpp
        weather/cloudlayer.h
        weather/cloudlayerlist.cpp
        weather/cloudlayerlist.h
        weather/gridpoint.cpp
        weather/gridpoint.h
        weather/metar.cpp
        weather/metar.h
        weather/metardecoder.cpp
        weather/metardecoder.h
        weather/metarlist.cpp
        weather/metarlist.h
        weather/presentweather.cpp
        weather/presentweather.h
        weather/presentweatherlist.cpp
        weather/presentweatherlist.h
        weather/registermetadataweather.cpp
        weather/registermetadataweather.h
        weather/temperaturelayer.cpp
        weather/temperaturelayer.h
        weather/temperaturelayerlist.cpp
        weather/temperaturelayerlist.h
        weather/visibilitylayer.cpp
        weather/visibilitylayer.h
        weather/visibilitylayerlist.cpp
        weather/visibilitylayerlist.h
        weather/weatherdataplugininfo.cpp
        weather/weatherdataplugininfo.h
        weather/weatherdataplugininfolist.cpp
        weather/weatherdataplugininfolist.h
        weather/weathergrid.cpp
        weather/weathergrid.h
        weather/weathergridprovider.cpp
        weather/weathergridprovider.h
        weather/weatherscenario.cpp
        weather/weatherscenario.h
        weather/windlayer.cpp
        weather/windlayer.h
        weather/windlayerlist.cpp
        weather/windlayerlist.h
)

if(APPLE)
    target_sources(misc PRIVATE
        macos/microphoneaccess.h
        macos/microphoneaccess.mm
    )
endif()

target_include_directories(misc PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_compile_definitions(misc PRIVATE BUILD_BLACKMISC_LIB)

target_link_libraries(misc
        PUBLIC
            blackconfig
            Qt::Core
            Qt::Gui
            Qt::DBus
            Qt::Network
            Qt::Multimedia
            externals_rapidjson # used by xswiftbussettingsqtfree.inc
        PRIVATE
            Qt::Xml
            SimpleCrypt
)

if(SWIFT_USE_CRASHPAD)
    target_link_libraries(misc PUBLIC externals_crashpad)
endif()

if(WIN32)
    # Remove Advapi32 once the Reg functions are removed again from CIdentifier
    target_link_libraries(misc PUBLIC Shell32 Dbghelp version Advapi32)
endif()

if(APPLE)
    target_link_libraries(misc PRIVATE "-lbsm -framework AVFoundation -framework Security -framework CoreFoundation -framework ApplicationServices -framework Foundation -framework IOKit")
endif()

target_precompile_headers(misc
    PRIVATE
        ${SWIFT_MISC_PCH}
)

install(TARGETS misc
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
