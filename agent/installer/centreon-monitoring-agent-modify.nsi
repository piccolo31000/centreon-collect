#
# Copyright 2024 Centreon
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# For more information : contact@centreon.com
#

XPStyle on

Unicode false

!define APPNAME "CentreonMonitoringAgent"
!define COMPANYNAME "Centreon"
!define DESCRIPTION "The Centreon Monitoring Agent (CMA) collects metrics and computes statuses on the servers it monitors, and sends them to Centreon."

!define SERVICE_NAME ${APPNAME}

!define CMA_REG_KEY "SOFTWARE\${COMPANYNAME}\${APPNAME}"

!include "LogicLib.nsh"
!include "nsDialogs.nsh"
!include "mui.nsh"
!include "Sections.nsh"
!include "StrFunc.nsh"
!include "FileFunc.nsh"

!include "nsis_service_lib.nsi"

${Using:StrFunc} StrCase

!define NSISCONF_3 ";" ; NSIS 2 tries to parse some preprocessor instructions inside "!if 0" blocks!
!addincludedir "nsis_pcre"
!define /redef NSISCONF_3 ""
${NSISCONF_3}!addplugindir /x86-ansi "nsis_pcre"
!undef NSISCONF_3


!include "version.nsi"
!include "resources\setup_dlg.nsdinc"
!include "resources\encryption_dlg.nsdinc"
!include "resources\log_dlg.nsdinc"
#let it after dialog boxes
!include "dlg_helper.nsi"

Name "Centreon Monitoring Agent ${VERSIONMAJOR}.${VERSIONMINOR}.${VERSIONBUILD}"
Icon "resources/logo_centreon.ico"
RequestExecutionLevel admin
AllowRootDirInstall true



!macro VerifyUserIsAdmin
UserInfo::GetAccountType
pop $0
${If} $0 != "admin" ;Require admin rights
        messageBox mb_iconstop "Administrator rights required!"
        setErrorLevel 740 ;ERROR_ELEVATION_REQUIRED
        quit
${EndIf}
!macroend
 
function .onInit
	setShellVarContext all
	!insertmacro VerifyUserIsAdmin
functionEnd

/**
  * @brief at the end of the installer, we stop and start cma
*/
Function encryption_next_and_restart_centagent
    Call encryption_dlg_onNext

    !insertmacro SERVICE "stop" "${SERVICE_NAME}" ""

    ;wait for service stop
    StrCpy $0 ""
    ${Do}
        Push  "running"
        Push ${SERVICE_NAME}
        Push ""
        Call Service
        Pop $0
        ${If} $0 != "true"
            ${Break}
        ${EndIf}
        Sleep 500
    ${Loop}
    ; even if service is stopped, process can be stopping so we wait a little more
    Sleep 500

    !insertmacro SERVICE "start" "${SERVICE_NAME}" ""
    MessageBox MB_OK "The Centreon Monitoring Agent has now restarted"
    Quit
FunctionEnd


/**
  * @brief this fake page is used to have a close button after restart cma confirmation
*/
Function dummy_page
FunctionEnd

Page custom fnc_cma_Show setup_dlg_onNext
Page custom fnc_log_dlg_Show log_dlg_onNext ": logging"
Page custom fnc_encryption_Show  encryption_next_and_restart_centagent ": encryption"
Page custom dummy_page

/**
  * @brief this installer only update agent config, no installation
*/
Section "update config"
SectionEnd