import org.jenkinsci.plugins.workflow.steps.FlowInterruptedException

abortPreviousRunningBuilds()

def builders = [:]
def buildResults = [:]

// Create a map to pass in to the 'parallel' step so we can fire all the builds at once
builders['Build swift Linux'] = {
    node('linux') {
        try {
            stage('Linux Checkout') {
                checkout scm
            }

            stage('Linux Build') {
                withEnv(['BITROCK_BUILDER=/opt/installbuilder/bin/builder',
                         'BITROCK_CUSTOMIZE=/opt/installbuilder/autoupdate/bin/customize.run']) {
                    sh """
                        cp ~/vatsim.json .
                        python3 -u scripts/jenkins.py -w 64 -t gcc -d -j 2 -e ${getEolInMonth()} -q SWIFT_CONFIG_JSON+=vatsim.json
                    """
                }

                warnings consoleParsers: [[parserName: 'GNU C Compiler 4 (gcc)']], unstableTotalAll: '0'

                xunit testTimeMargin: '3000', thresholdMode: 1, thresholds: [failed(), skipped()],
                    tools: [QtTest(deleteOutputFiles: true, failIfNotNew: false, pattern: 'build/out/release/bin/*_testresults.xml',
                                   skipNoTestFiles: true, stopProcessingIfError: false)]
            }

            stage('Linux Checks') {
                sh 'cppcheck --xml --xml-version=2 --inline-suppr --std=c++14 -ibuild --enable=style,unusedFunction -i externals ' +
                        '--suppressions-list=cppcheck.supp --library=qt.cfg --library=posix.cfg . 2> cppcheck.xml'

                publishCppcheck allowNoReport: true, pattern: 'cppcheck.xml'
            }

            stage('Linux Archive') {
                stash name: 'swift-linux-64', includes: 'swiftinstaller-linux-64-*.run,swiftsymbols-linux-64-*.tar.gz'
                stash name: 'xswiftbus-linux-64', includes: 'xswiftbus-linux-64-*.7z'
            }
            buildResults['swift-linux'] = 'SUCCESS'
        } catch (error) {
            if (isUserAborted(error)) {
                echo 'User abort. No error!'
                buildResults['swift-linux'] = 'ABORTED'
            } else {
                echo error.getMessage()
                buildResults['swift-linux'] = 'FAILURE'
            }
            throw error
        } finally {
            notifySlack('Linux', buildResults['swift-linux'])
            cleanWs deleteDirs: true, notFailBuild: true
        }
    }
}

builders['Build swift MacOS'] = {
    node('macos') {
        try {
            stage('MacOS Checkout') {
                checkout scm
            }

            stage('MacOS Build') {
                withEnv(['PATH+LOCAL=/usr/local/bin',
                         'BITROCK_BUILDER=/Applications/BitRockInstallBuilderQt/bin/builder',
                         'BITROCK_CUSTOMIZE=/Applications/BitRockInstallBuilderQt/autoupdate/bin/customize.sh']) {
                    sh """
                        cp ~/vatsim.json .
                        python -u scripts/jenkins.py -w 64 -t clang -d -j2  -e ${getEolInMonth()} -q SWIFT_CONFIG_JSON+=vatsim.json
                    """
                }

                warnings consoleParsers: [[parserName: 'Clang (LLVM based)']], unstableTotalAll: '0'

                xunit testTimeMargin: '3000', thresholdMode: 1, thresholds: [failed(), skipped()],
                    tools: [QtTest(deleteOutputFiles: true, failIfNotNew: false, pattern: 'build/out/release/bin/*_testresults.xml',
                                   skipNoTestFiles: true, stopProcessingIfError: false)]
            }

            stage('MacOS Archive') {
                stash name: 'swift-macos-64', includes: 'swiftinstaller-macos-64-*.dmg,swiftsymbols-macos-64-*.tar.gz'
                stash name: 'xswiftbus-macos-64', includes: 'xswiftbus-macos-64-*.7z'
            }
            buildResults['swift-macos'] = 'SUCCESS'
        } catch (error) {
            if (isUserAborted(error)) {
                echo 'User abort. No error!'
                buildResults['swift-macos'] = 'ABORTED'
            } else {
                echo error.getMessage()
                buildResults['swift-macos'] = 'FAILURE'
            }
            throw error
        } finally {
            notifySlack('MacOS', buildResults['swift-macos'])
            cleanWs deleteDirs: true, notFailBuild: true
        }
    }
}

builders['Build swift Win32'] = {
    node('windows') {
        try {
            stage('Win32 Checkout') {
                checkout scm
            }

            stage('Win32 Build') {
                bat """
                    copy c:\\var\\vatsim.json .
                    python -u scripts/jenkins.py -w 32 -t msvc -d -e ${getEolInMonth()} -q SWIFT_CONFIG_JSON+=vatsim.json
                """

                warnings consoleParsers: [[parserName: 'MSBuild']], unstableTotalAll: '0'

                xunit testTimeMargin: '3000', thresholdMode: 1, thresholds: [failed(), skipped()],
                    tools: [QtTest(deleteOutputFiles: true, failIfNotNew: false, pattern: 'build/out/release/bin/*_testresults.xml',
                                   skipNoTestFiles: true, stopProcessingIfError: false)]
            }

            stage('Win32 Archive') {
                stash name: 'swift-windows-32', includes: 'swiftinstaller-windows-32-*.exe,swiftsymbols-windows-32-*.tar.gz'
                stash name: 'xswiftbus-windows-32', includes: 'xswiftbus-windows-32-*.7z'
            }
            buildResults['swift-win32'] = 'SUCCESS'
        } catch (error) {
            if (isUserAborted(error)) {
                echo 'User abort. No error!'
                buildResults['swift-win32'] = 'ABORTED'
            } else {
                echo error.getMessage()
                buildResults['swift-win32'] = 'FAILURE'
            }
            throw error
        } finally {
            notifySlack('Win32', buildResults['swift-win32'])
            killDBusDaemon()
            cleanWs deleteDirs: true, notFailBuild: true
        }
    }
}

builders['Build swift Win64'] = {
    node('windows') {
        try {
            stage('Win64 Checkout') {
                checkout scm
            }

            stage('Win64 Build') {
                bat """
                    copy c:\\var\\vatsim.json .
                    python -u scripts/jenkins.py -w 64 -t msvc -d -e ${getEolInMonth()} -q SWIFT_CONFIG_JSON+=vatsim.json
                """

                warnings consoleParsers: [[parserName: 'MSBuild']], unstableTotalAll: '0'

                xunit testTimeMargin: '3000', thresholdMode: 1, thresholds: [failed(), skipped()],
                    tools: [QtTest(deleteOutputFiles: true, failIfNotNew: false, pattern: 'build/out/release/bin/*_testresults.xml',
                                   skipNoTestFiles: true, stopProcessingIfError: false)]
            }

            stage('Win64 Archive') {
                stash name: 'swift-windows-64', includes: 'swiftinstaller-windows-64-*.exe,swiftsymbols-windows-64-*.tar.gz'
                stash name: 'xswiftbus-windows-64', includes: 'xswiftbus-windows-64-*.7z'
            }
            buildResults['swift-win64'] = 'SUCCESS'
        } catch (error) {
            if (isUserAborted(error)) {
                echo 'User abort. No error!'
                buildResults['swift-win64'] = 'ABORTED'
            } else {
                echo error.getMessage()
                buildResults['swift-win64'] = 'FAILURE'
            }
            throw error
        } finally {
            notifySlack('Win64', buildResults['swift-win64'])
            killDBusDaemon()
            cleanWs deleteDirs: true, notFailBuild: true
        }
    }
}

try {
    try {
        parallel builders
    } finally {
        def r = Result.fromString(buildResults.isEmpty() ? 'ABORTED' : currentBuild.currentResult)
        buildResults.each{ r = r.combine(Result.fromString(it.value)) }
        if (r.isWorseThan(Result.SUCCESS)) {
            currentBuild.result = r.toString()
        }
    }
} catch (error) {
    node('master') {
        notifyHarbormaster()
    }
    throw error
}

node('master') {
    try {
        stage('Package XSwiftBus') {
            unstash name: 'xswiftbus-linux-64'
            unstash name: 'xswiftbus-macos-64'
            unstash name: 'xswiftbus-windows-32'
            unstash name: 'xswiftbus-windows-64'
            sh '''
                7z x -y xswiftbus-windows-32-*.7z
                7z x -y xswiftbus-windows-64-*.7z
                7z x -y xswiftbus-linux-64-*.7z
                7z x -y xswiftbus-macos-64-*.7z
            '''
            def regex = /(?<=xswiftbus-windows-32-)\d.\d.\d.\d+(?=.7z)/
            def version = sh(returnStdout: true, script: "ls xswiftbus-windows-32-*.7z | grep -Po '${regex}'").trim()
            sh "7z a -y -mx=9 xswiftbus-allos-${version}.7z xswiftbus"

            stash name: 'xswiftbus-allos', includes: 'xswiftbus-allos-*.7z'
        }
    } catch (error) {
        if (isUserAborted(error)) {
            echo 'User abort. No error!'
            currentBuild.result = 'ABORTED'
        } else {
            currentBuild.result = 'FAILURE'
        }
        notifyHarbormaster()
        throw error
    } finally {
        cleanWs deleteDirs: true, notFailBuild: true
    }
}

node('master') {
    try {
        def regexDevBranch = /develop\/\d+\.\d+\.\d+/
        if (env.BRANCH_NAME && env.BRANCH_NAME ==~ regexDevBranch) {
            stage('Publish') {
                unstash name: 'swift-linux-64'
                unstash name: 'swift-macos-64'
                unstash name: 'swift-windows-32'
                unstash name: 'swift-windows-64'
                unstash name: 'xswiftbus-allos'

                sh 'chmod 664 *'
                sh '''#!/bin/bash
                    sftp datastore@datastore.swift-project.org:/datastore.swift-project.org/artifacts <<< "mput *.run"
                    sftp datastore@datastore.swift-project.org:/datastore.swift-project.org/artifacts <<< "mput *.dmg"
                    sftp datastore@datastore.swift-project.org:/datastore.swift-project.org/artifacts <<< "mput *.tar.gz"
                    sftp datastore@datastore.swift-project.org:/datastore.swift-project.org/artifacts <<< "mput *.exe"
                    sftp datastore@datastore.swift-project.org:/datastore.swift-project.org/artifacts <<< "mput *.7z"
                '''

                archiveArtifacts artifacts: 'swiftinstaller-*,swiftsymbols-*.tar.gz', onlyIfSuccessful: true
                archiveArtifacts artifacts: 'xswiftbus-allos-*.7z', onlyIfSuccessful: true
            }
        } else {
            stage('Archive') {
                unstash name: 'swift-linux-64'
                unstash name: 'swift-macos-64'
                unstash name: 'swift-windows-32'
                unstash name: 'swift-windows-64'
                unstash name: 'xswiftbus-allos'

                archiveArtifacts artifacts: 'swiftinstaller-*,swiftsymbols-*.tar.gz', onlyIfSuccessful: true
                archiveArtifacts artifacts: 'xswiftbus-allos-*.7z', onlyIfSuccessful: true
            }
        }
    } catch (error) {
        if (isUserAborted(error)) {
            echo 'User abort. No error!'
            currentBuild.result = 'ABORTED'
        } else {
            currentBuild.result = 'FAILURE'
        }
        notifyHarbormaster()
        throw error
    } finally {
        cleanWs deleteDirs: true, notFailBuild: true
    }
}

node('master') {
    notifyHarbormaster()
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////// Utility function definitions ////////////////////////
//////////////////////////////////////////////////////////////////////////////

def isUserAborted(error) {
    return manager.build.getAction(InterruptedBuildAction.class) ||
           (error instanceof FlowInterruptedException && error.causes.size() == 0)
}

def abortPreviousRunningBuilds() {
    Jenkins.instance.getItemByFullName(env.JOB_NAME).getBuilds().each{ build ->
        if (params.REVISION_ID == null || isAbortableDifferentialBuild(build)) {
            def exec = build.getExecutor()

            if (build.number < currentBuild.number && exec != null) {
                exec.interrupt(Result.ABORTED, new CauseOfInterruption.UserInterruption("Aborted by #${currentBuild.number}"))
                println("Aborted previous running build #${build.number}")
            }
        }
    }
}

def isAbortableDifferentialBuild(build) {
    def buildEnv = build.getEnvironment(TaskListener.NULL)
    return buildEnv.containsKey('ABORT_ON_REVISION_ID') && buildEnv.get('ABORT_ON_REVISION_ID') == params.REVISION_ID
}

def notifyHarbormaster() {
    if (params.PHID != null) {
        def arcCmd = '/opt/arc/arcanist/bin/arc call-conduit --conduit-uri https://dev.swift-project.org/'
        def type = currentBuild.resultIsBetterOrEqualTo('SUCCESS') ? 'pass' : 'fail'
        def json = """{ "buildTargetPHID": "${params.PHID}", "type": "${type}" }"""
        sh "echo '${json}' | ${arcCmd} harbormaster.sendmessage"

        def icon = '{icon question-circle color=orange}'
        if (currentBuild.result == 'SUCCESS') {
            icon = '{icon check color=green}'
        } else if (currentBuild.result == 'FAILURE') {
            icon = '{icon times color=red}'
        } else if (currentBuild.result == 'UNSTABLE') {
            icon = '{icon exclamation-triangle color=orange}'
        }
        def comment = "${icon} {${params.PHID}} ${currentBuild.currentResult}: ${env.BUILD_URL}"
        json = """{ "objectIdentifier": "D${params.REVISION_ID}", "transactions": [{ "type": "comment", "value": "${comment}" }] }"""
        sh "echo '${json}' | ${arcCmd} differential.revision.edit"
    }
}

def notifySlack(nodeName, buildStatus = 'STARTED') {
    // build status of null means successful
    buildStatus = buildStatus ?: 'SUCCESS'

    // Ignore ABORTED status
    if (buildStatus == 'ABORTED') {
        return
    }

    // Default values
    def colorCode = '#FF0000'
    def emoji = ':question:'
    def duration = currentBuild.durationString.replace(' and counting', '')
    def branch = params.STAGING_REF ?: scm.branches.first().name
    branch = branch.replaceFirst(/^refs\/tags\//, '')
    def title = "${java.net.URLDecoder.decode(JOB_NAME)}: `${branch}`"
    if (params.REVISION_ID != null) {
        title += " - D${params.REVISION_ID} (Build ${params.BUILD_ID})"
    }
    def subject = "${nodeName}: ${title} - #${BUILD_NUMBER} ${buildStatus} after ${duration}"
    def summary = "${subject} (<${env.BUILD_URL}|Open>)"

    // Override default values based on build status
    if (buildStatus == 'STARTED') {
        // yellow
        colorCode = '#FFFF00'
        emoji = ':heavy_plus_sign:'
    } else if (buildStatus == 'SUCCESS') {
        // green
        colorCode = '#008000'
        emoji = ':heavy_check_mark:'
    } else if (buildStatus == 'ABORTED') {
        // grey
        colorCode = '#808080'
        emoji = ':grey_question:'
    } else {
        // darkred
        colorCode = '#8B0000'
        emoji = ':x:'
    }

    // Send notifications - disable during testing
    slackSend (color: colorCode, message: "${emoji} ${summary}")
}

def getEolInMonth() {
    def regexDevBranch = /develop\/\d+\.\d+\.\d+/
    def regexReleaseBranch = /release\/\d+\.\d+/
    if (env.BRANCH_NAME && env.BRANCH_NAME ==~ regexDevBranch) {
        // 6 month for dev builds
        return 6
    } else if (env.BRANCH_NAME && env.BRANCH_NAME ==~ regexReleaseBranch) {
        // 12 month currently for release builds. That will be removed in future.
        return 12
    } else {
        // 3 month for everything else
        return 3
    }
}

def killDBusDaemon() {
    bat '''
        tasklist /FI "IMAGENAME eq dbus-daemon.exe" 2>NUL | find /I /N "dbus-daemon.exe">NUL
        if "%ERRORLEVEL%"=="0" taskkill /f /im dbus-daemon.exe
        EXIT 0
    '''
}
