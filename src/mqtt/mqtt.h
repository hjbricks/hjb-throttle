/*
MIT License

Copyright (c) 2023 Hilbert Barelds

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

#define MQTT_INFO     "rocrail/service/info"
#define MQTT_COMMAND "rocrail/service/command"

/*
  -t "rocrail/service/client" -m '<model cmd="lclist"/>'
  -t "rocrail/service/client" -m '<model cmd="swlist"/>'
  -t "rocrail/service/client" -m '<sys cmd="go"/>'
  -t "rocrail/service/client" -m '<sys cmd="stop"/>'
  -t "rocrail/service/client" -m '<lc id="FUX" dir="true"  addr="6337" secaddr="0" V="50"/>'
  -t "rocrail/service/client" -m '<lc id="FUX" dir="true"  addr="6337" secaddr="0" V="0"/>'
  -t "rocrail/service/client" -m '<lc id="FUX" dir="false"  addr="6337" secaddr="0" V="20"/>'
  -t "rocrail/service/client" -m '<lc id="FUX" dir="true"  addr="6337" secaddr="0" V="20"/>'
  -t "rocrail/service/client" -m '<lc id="FUX" dir="true"  addr="6337" secaddr="0" V="0"/>'
  -t "rocrail/service/client" -m '<sw id="sw21" cmd="flip"  manualcmd="true" />

<clock divider="1" hour="21" minute="29" wday="2" mday="18" month="4" year="2023" time="1681846146" temp="20" bri="255" lux="0" pressure="0" humidity="0" cmd="sync" wio="true"/>

<lc id="FUX" uid="0" sid="0" dir="true" addr="6337" secaddr="0" V="0" placing="true" blockenterside="false" blockenterid="" modeevent="false" mode="stop" modereason="checkroute" resumeauto="false" manual="true" shunting="false" standalone="false" blockid="bk01" destblockid="" fn="false" runtime="22440" mtime="0" rdate="1681845781" mint="0" throttleid="virtual" active="false" waittime="0" scidx="-1" scheduleid="" tourid="" scheduleinithour="0" len="110" weight="0" train="" trainlen="110" trainweight="0" V_realkmh="0" fifotop="false" image="fuxing.png" imagenr="0" energypercentage="0" lookupschedule="false" pause="false" consist=""/>

<swlist>
  <sw type="right" id="sw11" x="1" y="12" z="0" testing="false" switched="348" ori="north" pre_move_x="1" pre_move_y="11" pre_move_z="0" porttype="0" frogporttype="0" prev_id="sw11" useshortid="false" shortid="" nr="0" desc="" decid="" blockid="" routeids="[bk21+]-[bk02+],[bk02+]-[bk11+]" manual="false" outoforder="false" operable="true" staticuse="false" exclude="false" accnr="0" subtype="default" savepos="none" dir="true" swtype="default" road="false" show="true" showid="true" sod="true" swapstraight="false" rectcrossing="false" iid="" uidname="" bus="0" addr1="0" port1="11" gate1="0" inv="false" singlegate="false" accessory="true" addr2="0" port2="0" gate2="0" inv2="false" actdelay="true" syncdelay="false" delay="500" param1="0" value1="1" param2="0" value2="1" prot="D" tdiid="" tdaddr="0" tdport="0" td="false" fbR="" fbG="" fb2R="" fb2G="" fbOcc="" fbOcc2="" fbRinv="false" fbGinv="false" fb2Rinv="false" fb2Ginv="false" fbset="false" fbusefield="false" ctciid1="" ctciid2="" ctcuid1="" ctcuid2="" ctcbus1="0" ctcbus2="0" ctcaddr1="0" ctcaddr2="0" ctccmdon1="false" ctccmdon2="false" ctcflip1="true" ctcflip2="true" ctciidled1="" ctciidled2="" ctcbusled1="0" ctcbusled2="0" ctcaddrled1="0" ctcaddrled2="0" ctcportled1="0" ctcportled2="0" ctcgateled1="0" ctcgateled2="0" ctcasswitchled1="false" ctcasswitchled2="false" frogiid="" buspol="0" frogtimer="0" addr0pol1="0" port0pol1="0" gate0pol1="0" addr0pol2="0" port0pol2="0" gate0pol2="0" addr1pol1="0" port1pol1="0" gate1pol1="0" addr1pol2="0" port1pol2="0" gate1pol2="0" frogaccessory="true" frogswitch="false" state="straight" wantedstate="straight" fieldstate="straight" remark="" param="0" ctcOutputStraight="" ctcOutputThrown="" ctcOutputPoint="" ctcColorStraight="0" ctcColorThrown="0" ctcColorPoint="0" ctcColorStraightOcc="0" ctcColorThrownOcc="0" ctcColorPointLock="0" froginvert="false">
    <accessoryctrl active="false" invert="false" automode="false" interval="0" delay="0" lockroutes="" freeblocks=""/>
  </sw>
  <sw type="right" ori="south" id="sw12" x="2" y="12" z="0" testing="false" switched="318" pre_move_x="2" pre_move_y="11" pre_move_z="0" porttype="2" frogporttype="0" prev_id="sw12" useshortid="false" shortid="" nr="0" desc="" decid="" blockid="" routeids="[bk01+]-[bk11+],[bk02+]-[bk11+]" manual="false" outoforder="false" operable="true" staticuse="false" exclude="false" accnr="0" subtype="default" savepos="none" dir="true" swtype="default" road="false" show="true" showid="true" sod="true" swapstraight="false" rectcrossing="false" iid="NEW" uidname="sw-1" bus="1001" addr1="1" port1="2" gate1="0" inv="false" singlegate="false" accessory="false" addr2="0" port2="0" gate2="0" inv2="false" actdelay="true" syncdelay="false" delay="500" param1="0" value1="161" param2="0" value2="180" prot="N" tdiid="" tdaddr="0" tdport="0" td="false" fbR="" fbG="" fb2R="" fb2G="" fbOcc="" fbOcc2="" fbRinv="false" fbGinv="false" fb2Rinv="false" fb2Ginv="false" fbset="false" fbusefield="false" ctciid1="" ctciid2="" ctcuid1="" ctcuid2="" ctcbus1="0" ctcbus2="0" ctcaddr1="0" ctcaddr2="0" ctccmdon1="false" ctccmdon2="false" ctcflip1="true" ctcflip2="true" ctciidled1="" ctciidled2="" ctcbusled1="0" ctcbusled2="0" ctcaddrled1="0" ctcaddrled2="0" ctcportled1="0" ctcportled2="0" ctcgateled1="0" ctcgateled2="0" ctcasswitchled1="false" ctcasswitchled2="false" frogiid="" buspol="4" frogtimer="0" addr0pol1="0" port0pol1="0" gate0pol1="0" addr0pol2="0" port0pol2="0" gate0pol2="0" addr1pol1="0" port1pol1="0" gate1pol1="0" addr1pol2="0" port1pol2="0" gate1pol2="0" frogaccessory="true" frogswitch="false" state="straight" wantedstate="straight" fieldstate="straight" remark="" param="0" ctcOutputStraight="" ctcOutputThrown="" ctcOutputPoint="" ctcColorStraight="0" ctcColorThrown="0" ctcColorPoint="0" ctcColorStraightOcc="0" ctcColorThrownOcc="0" ctcColorPointLock="0" froginvert="false">
    <accessoryctrl active="false" invert="false" automode="false" interval="0" delay="0" lockroutes="" freeblocks=""/>
  </sw>
  <sw type="right" id="sw21" x="13" y="12" z="0" switched="471" testing="false" ori="north" copy="false" pre_move_x="13" pre_move_y="11" pre_move_z="0" porttype="0" frogporttype="0" prev_id="sw21" useshortid="false" shortid="" nr="0" desc="" decid="" blockid="" routeids="[bk13+]-[bk01-],[bk01-]-[bk23+]" manual="false" outoforder="false" operable="true" staticuse="false" exclude="false" accnr="0" subtype="default" savepos="none" dir="true" swtype="default" road="false" show="true" showid="true" sod="true" swapstraight="false" rectcrossing="false" iid="" uidname="" bus="0" addr1="6" port1="1" gate1="0" inv="false" singlegate="false" accessory="true" addr2="0" port2="0" gate2="0" inv2="false" actdelay="true" syncdelay="false" delay="500" param1="0" value1="1" param2="0" value2="1" prot="D" tdiid="" tdaddr="0" tdport="0" td="false" fbR="" fbG="" fb2R="" fb2G="" fbOcc="" fbOcc2="" fbRinv="false" fbGinv="false" fb2Rinv="false" fb2Ginv="false" fbset="false" fbusefield="false" ctciid1="" ctciid2="" ctcuid1="" ctcuid2="" ctcbus1="0" ctcbus2="0" ctcaddr1="0" ctcaddr2="0" ctccmdon1="false" ctccmdon2="false" ctcflip1="true" ctcflip2="true" ctciidled1="" ctciidled2="" ctcbusled1="0" ctcbusled2="0" ctcaddrled1="0" ctcaddrled2="0" ctcportled1="0" ctcportled2="0" ctcgateled1="0" ctcgateled2="0" ctcasswitchled1="false" ctcasswitchled2="false" frogiid="" buspol="0" frogtimer="0" addr0pol1="0" port0pol1="0" gate0pol1="0" addr0pol2="0" port0pol2="0" gate0pol2="0" addr1pol1="0" port1pol1="0" gate1pol1="0" addr1pol2="0" port1pol2="0" gate1pol2="0" frogaccessory="true" frogswitch="false" state="straight" wantedstate="straight" fieldstate="straight" remark="" param="0" ctcOutputStraight="" ctcOutputThrown="" ctcOutputPoint="" ctcColorStraight="0" ctcColorThrown="0" ctcColorPoint="0" ctcColorStraightOcc="0" ctcColorThrownOcc="0" ctcColorPointLock="0" froginvert="false">
    <accessoryctrl active="false" invert="false" automode="false" interval="0" delay="0" lockroutes="" freeblocks=""/>
  </sw>
  <sw type="right" ori="south" id="sw22" x="14" y="12" z="0" switched="390" testing="false" copy="false" pre_move_x="14" pre_move_y="11" pre_move_z="0" porttype="0" frogporttype="0" prev_id="sw22" useshortid="false" shortid="" nr="0" desc="" decid="" blockid="" routeids="[bk01-]-[bk23+],[bk02-]-[bk23+]" manual="false" outoforder="false" operable="true" staticuse="false" exclude="false" accnr="0" subtype="default" savepos="none" dir="true" swtype="default" road="false" show="true" showid="true" sod="true" swapstraight="false" rectcrossing="false" iid="" uidname="" bus="0" addr1="11398" port1="1" gate1="0" inv="false" singlegate="false" accessory="true" addr2="0" port2="0" gate2="0" inv2="false" actdelay="true" syncdelay="false" delay="500" param1="47" value1="120" param2="0" value2="1" prot="D" tdiid="" tdaddr="0" tdport="0" td="false" fbR="" fbG="" fb2R="" fb2G="" fbOcc="" fbOcc2="" fbRinv="false" fbGinv="false" fb2Rinv="false" fb2Ginv="false" fbset="false" fbusefield="false" ctciid1="" ctciid2="" ctcuid1="" ctcuid2="" ctcbus1="0" ctcbus2="0" ctcaddr1="0" ctcaddr2="0" ctccmdon1="false" ctccmdon2="false" ctcflip1="true" ctcflip2="true" ctciidled1="" ctciidled2="" ctcbusled1="0" ctcbusled2="0" ctcaddrled1="0" ctcaddrled2="0" ctcportled1="0" ctcportled2="0" ctcgateled1="0" ctcgateled2="0" ctcasswitchled1="false" ctcasswitchled2="false" frogiid="" buspol="0" frogtimer="0" addr0pol1="0" port0pol1="0" gate0pol1="0" addr0pol2="0" port0pol2="0" gate0pol2="0" addr1pol1="0" port1pol1="0" gate1pol1="0" addr1pol2="0" port1pol2="0" gate1pol2="0" frogaccessory="true" frogswitch="false" state="turnout" wantedstate="turnout" fieldstate="turnout" locid="FUX" remark="" param="0" ctcOutputStraight="" ctcOutputThrown="" ctcOutputPoint="" ctcColorStraight="0" ctcColorThrown="0" ctcColorPoint="0" ctcColorStraightOcc="0" ctcColorThrownOcc="0" ctcColorPointLock="0" froginvert="false">
    <accessoryctrl active="false" invert="false" automode="false" interval="0" delay="0" lockroutes="" freeblocks=""/>
  </sw>
</swlist>

<lclist>
  <lc id="BR01" shortid="EME" addr="8984"/>
  <lc id="BR146" shortid="SFE" addr="146"/>
  <lc id="E03" shortid="" addr="103"/>
  <lc id="ICE" shortid="" addr="6051"/>
  <lc id="BROCO" shortid="" addr="4"/>
  <lc id="TGV" shortid="" addr="10233"/>
  <lc id="EST" shortid="" addr="6197"/>
  <lc id="MAE" shortid="" addr="9"/>
  <lc id="SFE" shortid="" addr="10020"/>
  <lc id="EME" shortid="" addr="5"/>
  <lc id="V200" shortid="" addr="200"/>
  <lc id="L7938" shortid="" addr="7938"/>
  <lc id="E94" shortid="" addr="8888"/>
  <lc id="HOGWRD" shortid="" addr="6666"/>
  <lc id="BUS" shortid="" addr="3"/>
  <lc id="Switch Dark" shortid="" addr="1111"/>
  <lc id="GRECO" shortid="" addr="15499"/>
  <lc id="BIGBOY" shortid="EME" addr="4884"/>
  <lc id="BR52" shortid="EME" addr="52"/>
  <lc id="V100" shortid="" addr="100"/>
  <lc id="FUX" shortid="" addr="6337"/>
</lclist>



<fn fnchanged="1" fnchangedstate="false" id="FUX" longclick="false" group="1" fncnt="2" addr="6337" f1="false" throttleid="" fndesc="FWD" controlcode="" slavecode="" server="infw03AB8B54" iid="" uid="0" sid="0" dir="true" secaddr="0" V="0" placing="true" blockenterside="false" blockenterid="" modeevent="false" mode="stop" modereason="checkroute" resumeauto="false" manual="true" shunting="false" standalone="false" blockid="bk01" destblockid="" fn="false" runtime="22458" mtime="0" rdate="1682275733" mint="0" active="false" waittime="0" scidx="-1" scheduleid="" tourid="" scheduleinithour="0" len="110" weight="0" train="" trainlen="110" trainweight="0" V_realkmh="0" fifotop="false" image="fuxing.png" imagenr="0" energypercentage="0" lookupschedule="false" pause="false" consist=""/>
<fn id="FUX" addr="6337" fncnt="2" f0="false" f1="false" f2="false" f3="false" f4="false" f5="false" f6="false" f7="false" f8="false" f9="false" f10="false" f11="false" f12="false" f13="false" f14="false" f15="false" f16="false" f17="false" f18="false" f19="false" f20="false" f21="false" f22="false" f23="false" f24="false" f25="false" f26="false" f27="false" f28="false" f29="false" f30="false" f31="false" f32="false" throttleid="rv2720" fnchanged="1" group="1"/>

<fn fnchanged="1" fnchangedstate="true" id="FUX" longclick="false" group="1" fncnt="2" addr="6337" f1="true" throttleid="" fndesc="FWD" controlcode="" slavecode="" server="infw03AB8B54" iid="" uid="0" sid="0" dir="true" secaddr="0" V="0" placing="true" blockenterside="false" blockenterid="" modeevent="false" mode="stop" modereason="checkroute" resumeauto="false" manual="true" shunting="false" standalone="false" blockid="bk01" destblockid="" fn="false" runtime="22513" mtime="0" rdate="1682275733" mint="0" active="false" waittime="0" scidx="-1" scheduleid="" tourid="" scheduleinithour="0" len="110" weight="0" train="" trainlen="110" trainweight="0" V_realkmh="0" fifotop="false" image="fuxing.png" imagenr="0" energypercentage="0" lookupschedule="false" pause="false" consist=""/>
<fn id="FUX" addr="6337" fncnt="2" f0="false" f1="true" f2="false" f3="false" f4="false" f5="false" f6="false" f7="false" f8="false" f9="false" f10="false" f11="false" f12="false" f13="false" f14="false" f15="false" f16="false" f17="false" f18="false" f19="false" f20="false" f21="false" f22="false" f23="false" f24="false" f25="false" f26="false" f27="false" f28="false" f29="false" f30="false" f31="false" f32="false" throttleid="rv2720" fnchanged="1" group="1"/>

<lc id="FUX" bus="0" addr="6337" V="0" fn="false" dir="true" usesecaddr="false" throttleid="rv2720" controlcode="" slavecode="" server="infw03AB8B54" fncnt="2" f0="false" f1="false" f2="false" f3="false" f4="false" f5="false" f6="false" f7="false" f8="false" f9="false" f10="false" f11="false" f12="false" f13="false" f14="false" f15="false" f16="false" f17="false" f18="false" f19="false" f20="false" f21="false" f22="false" f23="false" f24="false" f25="false" f26="false" f27="false" f28="false" f29="false" f30="false" f31="false" f32="false" mass="0" minstep="0" maxstep="0" pwm="0" pwmcorrdiv="10" V_step="0" V_min="30" V_max="100" V_mode="kmh" prot="P" protver="1" spcnt="100" oid="" identifier="" mode="stop" iid="" port="0" imagenr="0" informall="false" wio="true"/>
<lc id="FUX" bus="0" addr="6337" V="0" fn="false" dir="true" usesecaddr="false" throttleid="rv2720" controlcode="" slavecode="" server="infw03AB8B54" iid="" uid="0" sid="0" secaddr="0" placing="true" blockenterside="false" blockenterid="" modeevent="false" mode="stop" modereason="checkroute" resumeauto="false" manual="true" shunting="false" standalone="false" blockid="bk01" destblockid="" runtime="22478" mtime="0" rdate="1682275733" mint="0" active="false" waittime="0" scidx="-1" scheduleid="" tourid="" scheduleinithour="0" len="110" weight="0" train="" trainlen="110" trainweight="0" V_realkmh="0" fifotop="false" image="fuxing.png" imagenr="0" energypercentage="0" lookupschedule="false" pause="false" consist=""/>
<lc id="FUX" uid="0" sid="0" dir="true" addr="6337" secaddr="0" V="0" placing="true" blockenterside="false" blockenterid="" modeevent="false" mode="stop" modereason="checkroute" resumeauto="false" manual="true" shunting="false" standalone="false" blockid="bk01" destblockid="" fn="false" runtime="22478" mtime="0" rdate="1682275733" mint="0" throttleid="virtual" active="false" waittime="0" scidx="-1" scheduleid="" tourid="" scheduleinithour="0" len="110" weight="0" train="" trainlen="110" trainweight="0" V_realkmh="0" fifotop="false" image="fuxing.png" imagenr="0" energypercentage="0" lookupschedule="false" pause="false" consist=""/>

<lc id="FUX" bus="0" addr="6337" V="100" V_hint="max" fn="false" dir="true" usesecaddr="false" throttleid="rv2720" controlcode="" slavecode="" server="infw03AB8B54" fncnt="2" f0="false" f1="false" f2="false" f3="false" f4="false" f5="false" f6="false" f7="false" f8="false" f9="false" f10="false" f11="false" f12="false" f13="false" f14="false" f15="false" f16="false" f17="false" f18="false" f19="false" f20="false" f21="false" f22="false" f23="false" f24="false" f25="false" f26="false" f27="false" f28="false" f29="false" f30="false" f31="false" f32="false" mass="0" minstep="0" maxstep="0" pwm="0" pwmcorrdiv="10" V_step="0" V_min="30" V_max="100" V_mode="kmh" prot="P" protver="1" spcnt="100" oid="" identifier="" mode="stop" iid="" port="0" imagenr="0" informall="false" wio="true"/>
<lc id="FUX" bus="0" addr="6337" V="100" V_hint="max" fn="false" dir="true" usesecaddr="false" throttleid="rv2720" controlcode="" slavecode="" server="infw03AB8B54" iid="" uid="0" sid="0" secaddr="0" placing="true" blockenterside="false" blockenterid="" modeevent="false" mode="stop" modereason="checkroute" resumeauto="false" manual="true" shunting="false" standalone="false" blockid="bk01" destblockid="" runtime="22478" mtime="0" rdate="1682275733" mint="0" active="false" waittime="0" scidx="-1" scheduleid="" tourid="" scheduleinithour="0" len="110" weight="0" train="" trainlen="110" trainweight="0" V_realkmh="0" fifotop="false" image="fuxing.png" imagenr="0" energypercentage="0" lookupschedule="false" pause="false" consist=""/>
<lc id="FUX" uid="0" sid="0" dir="true" addr="6337" secaddr="0" V="100" placing="true" blockenterside="false" blockenterid="" modeevent="false" mode="stop" modereason="checkroute" resumeauto="false" manual="true" shunting="false" standalone="false" blockid="bk01" destblockid="" fn="false" runtime="22478" mtime="0" rdate="1682275733" mint="0" throttleid="virtual" active="false" waittime="0" scidx="-1" scheduleid="" tourid="" scheduleinithour="0" len="110" weight="0" train="" trainlen="110" trainweight="0" V_realkmh="0" fifotop="false" image="fuxing.png" imagenr="0" energypercentage="0" lookupschedule="false" pause="false" consist=""/>

*/

extern void mqttCallback(char *topic, byte *payload, unsigned int length);

extern void publishGetPowerState();
extern void publishSetPowerStop();
extern void publishSetPowerGo();
extern void publishSetPowerEBreak();
extern void publishGetLocList();
extern void publishGetLocInfo(char *locId);
extern void publishGetSwitchList();
extern void publishGetOutputsList();
extern void publishGetBlocksList();
extern void publishGetSignalsList();
extern void publishGetSwitchInfo(char *switchId);
extern void publishGetOutputInfo(char *outputId);
extern void publishGetBlockInfo(char *blockId);
extern void publishGetSignalInfo(char *signalId);
extern void publishLoc();
extern void publishLocSpeed(long locV);
extern void publishLocFn(int fn, bool fnValue);
extern void publishSwToggle();
extern void publishCoState();
extern void publishSgState();

extern PubSubClient mqttClient;