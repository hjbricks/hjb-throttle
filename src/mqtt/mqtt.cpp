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
#include "mqtt.h"
#include "AiEsp32RotaryEncoder.h"
#include "XMLParser.h"
#include "display/display.h"
#include "esplog.h"
#include "rrbattery.h"
#include "rritems/bklist.h"
#include "rritems/colist.h"
#include "rritems/lclist.h"
#include "rritems/sglist.h"
#include "rritems/swlist.h"
#include "state.h"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// ugly global, but for now I didn't find a nice alternative without a lot of code!
extern AiEsp32RotaryEncoder rotaryEncoder;

/*
    The rocrail messeages that need to be handled are:
   <clock  -> update the clock on the display

   <lc     -> feedback on the current selected loco to see changes

   <swlist>  -> get the list of switches: id="<name>" is needed for now
     <sw id="<id>" ...> .... </sw>
     ...
   </swlist>

   <lclist>  -> get the list of loco's: id="<name>"  and addr="<address>"
     <lc id="<name>" ... -> see wat is needed />
     Also the next form can me used, lets see what is needed here
     <lc ....>
        <fundef ...
     </lc>
   </lclist>


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

*/
void handleClock(const char *message);
void handleLCList(const char *message);
void handleSWList(const char *message);
void handleCoList(const char *message);
void handleBkList(const char *message);
void handleSgList(const char *message);
void handleFbinfo(const char *message);
void handleLc(const char *message, bool has_prev);
void handleFn(const char *message);
void handleSw(const char *message, bool from_infotopic);
void handleCo(const char *message);
void handleBk(const char *message);
void handleFundef(const char *message);
void handleFb(const char *message);
void handleSg(const char *message);
void handleSys(const char *message);

/*
    handle all mqtt messages

    is_infotopic, is true when the message comes from the info topic,
    this is done because some messages are only posted on the info topic and
    you have to handle them differently

    The command topic is the prevered topic to listen on!
*/
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    char *pos;
    bool is_infotopic;
    bool has_previd;
    char *prev_id = nullptr;
    payload[length-1] = 0;
    // parse the rocrail mqtt messages, all of them,
    is_infotopic = ((strstr((char *)topic, MQTT_INFO)) != nullptr);
    has_previd = XMLParser::tryReadCharAttr((char *)payload, "prev_id", &prev_id);
    if (prev_id)
        free(prev_id);
    if ((pos = strstr((char *)payload, "<clock ")) != nullptr && (!is_infotopic)) {
        // found <clock
        handleClock(pos);
    } else if ((pos = strstr((char *)payload, "<lclist")) != nullptr) {
        handleLCList(pos + 5);
    } else if ((pos = strstr((char *)payload, "<swlist")) != nullptr) {
        handleSWList(pos + 5);
    } else if ((pos = strstr((char *)payload, "<colist")) != nullptr) {
        handleCoList(pos + 5);
    } else if ((pos = strstr((char *)payload, "<bklist")) != nullptr) {
        handleBkList(pos + 5);
    } else if ((pos = strstr((char *)payload, "<sglist")) != nullptr) {
        handleSgList(pos + 5);
    } else if ((pos = strstr((char *)payload, "<fbinfo")) != nullptr) {
        handleFbinfo(pos + 4); // this looks like a load of a plan.
    } else if ((pos = strstr((char *)payload, "<lc ")) != nullptr) {
        if ((is_infotopic && has_previd) || !is_infotopic)
            handleLc(pos + 2, has_previd);
    } else if ((pos = strstr((char *)payload, "<fn ")) != nullptr) {
        if ((is_infotopic && has_previd) || !is_infotopic)
            handleFn(pos + 2);
    } else if ((pos = strstr((char *)payload, "<sw ")) != nullptr) {
        if ((is_infotopic && has_previd) || !is_infotopic)
            handleSw(pos + 2, has_previd);
    } else if ((pos = strstr((char *)payload, "<co ")) != nullptr) {
        if ((is_infotopic && has_previd) || !is_infotopic)
            handleCo(pos + 2);
    } else if ((pos = strstr((char *)payload, "<bk ")) != nullptr) {
        if ((is_infotopic && has_previd) || !is_infotopic)
            handleBk(pos + 2);
    } else if ((pos = strstr((char *)payload, "<fb ")) != nullptr) {
        if (is_infotopic)
            handleFb(pos + 2);
    } else if ((pos = strstr((char *)payload, "<sys ")) != nullptr) {
        if (!is_infotopic)
            handleSys(pos + 3);
    } else if ((pos = strstr((char *)payload, "<sg ")) != nullptr) {
        if (is_infotopic)
            handleSg(pos + 2);
    }
}

void handleFbinfo(const char *message)
{
    // let it be a trigger to reload the locomotives and the switches
    publishGetLocList();
    publishGetSwitchList();
}
// <clock divider="1" hour="21" minute="29" wday="2" mday="18" month="4" year="2023" time="1681846146" temp="20" bri="255" lux="0" pressure="0" humidity="0" cmd="sync" wio="true"/>

void handleClock(const char *message)
{
    // find "hour" and "minute"
    int hour, minute;
    if (!XMLParser::tryReadIntAttr(message, "hour", &hour)) {
        logger.log(LOG_WARN, "MQTT: Received 'clock' command, but couldn't read 'hour' attribute.");
        return;
    }
    if (!XMLParser::tryReadIntAttr(message, "minute", &minute)) {
        logger.log(LOG_WARN, "MQTT: Received 'sys' command, but couldn't read 'cmd' attribute.");
        return;
    }
    logger.vlogf(LOG_DEBUG, "Clock: %02d:%02d", hour, minute);
    display.updateClock(hour, minute);
}

/*

<lc id="FUX" addr="6337" prev_id="FUX" shortid="" roadname="" owner="" color=""
number="" home="Main Station Terminus" desc="" dectype="" decfile="nmra-rp922.xml"
docu="" image="fuxing.png" imagenr="0" remark="" len="110" radius="0" weight="0"
nraxis="0" nrcars="0" manuid="" catnr="" purchased="" value="" identifier=""
show="true" active="false" useshortid="false" mint="0" throttlenr="0"
manually="false" bus="0" uid="0" secaddr="0" iid="" informall="false" oid=""
prot="P" protver="1" spcnt="100" secspcnt="100" fncnt="2"
V_min="30" V_mid="50" V_cru="75" V_max="100" V_maxsec="14"
KMH_min="0" KMH_mid="0" KMH_cru="0" KMH_max="0"
KMH_Rmin="0" KMH_Rmid="0" KMH_Rcru="0" KMH_Rmax="0"
KMH_Smin="0" KMH_Smid="0" KMH_Scru="0" KMH_Smax="0"
V_Rmin="0" V_Rmid="0" V_Rcru="0" V_Rmax="0"
V_Smin="0" V_Smid="0" V_Scru="0" V_Smax="0"
V_step="0" mass="0" minstep="0" maxstep="0" pwm="0" pwmcorrdiv="10"
Vmidpercent="30" Vmaxmin="20" Vmaxmax="255" Vmaxkmh="0" Vmidset="true"
V_mode="kmh" invdir="false" polarisation="true" regulated="true" restorefx="false"
restorefxalways="false" restorespeed="false" info4throttle="false" dirpause="0"
adjustaccel="false" maxload="0" accelmin="0" accelmax="0"
decelerate="0" accelcv="3" accelcvindex="0" vmaxcv="5"
vmaxcvindex="0" vmidcv="6" vmidcvindex="0" camhost="" camport="8081"
camtype="0" camfile="stream.mjpg" camskip="0" camoption="0" blockwaittime="10"
maxwaittime="0" evttimer="0" minenergypercentage="0" swaptimer="0" ent2incorr="100"
priority="10" usescheduletime="false" commuter="true" shortin="false" inatpre2in="false"
usemanualroutes="false" useownwaittime="false" startupscid="" startuptourid=""
check2in="true" usedepartdelay="true" freeblockonenter="true"
reducespeedatenter="false" routespeedatenter="false" v0onswap="false"
resetplacing="false" manual="true" lookupschedule="false" lookupschedulevirtual="false"
generated="false" swapondir="false" screcord="false" decoupler="false" engine="electric"
cargo="ice" secondnextblock="true" secondnextblock4wait="false" era="5" class=""
consist_syncfunmap="0" standalone="false" consist_lightsoff="false" consist_syncfun="false"
consist_synclights="false" consist="" usebbt="false" bbtsteps="10" bbtstartinterval="10"
bbtmaxdiff="250" bbtcorrection="25" bbtkey="0" cvnrs="1,2,3,4,5,6,17,18,29"
destblockid="" cmdDelay="0" pause="false" mode="stop" fifotop="false"
energypercentage="0" V="0" fx="0" throttleid="" trainlen="110" trainweight="0"
cmd="modify" signalaspect="red" dir="true" resumeauto="false" fn="false" blockid="bk01"
scidx="-1" blockenterid="" blockenterside="false" modereason="checkroute" waittime="0"
V_hint="max" rdate="1682275733" runtime="22513" placing="true" sid="0" modeevent="false"
shunting="false" mtime="0" scheduleid="" tourid="" scheduleinithour="11" train=""
V_realkmh="0" controlcode="" slavecode="" server="infw04FEA784" V_maxkmh="0"
gotoblockid="" cmdFixed="true" modeblock="bk01" arrivetime="1665687554">
  <fundef fn="1" text="FWD" sound="" icon="" timer="0" iconnr="1" addr="0" onpos="0" offpos="0" mappedfn="0" pushbutton="false"/>
  <fundef fn="2" text="REV" sound="" icon="" timer="0" iconnr="2" addr="0" onpos="0" offpos="0" mappedfn="0" pushbutton="false"/>
</lc>

GET
id="FUX"
V_max="100"
V_Rmax="0"
V_Smin="0" V_Smax="0"
V_mode="kmh"
V="0"
dir="true"

if found shortid, we got the one from

When a command is send, we get 3 info's
<lc id="SFE" addr="10020" dir="false" V="27" server="mqttreader" fncnt="3"
    f0="false" f1="false" f2="false" f3="false" f4="false" f5="false" f6="false" f7="false"
    f8="false" f9="false" f10="false" f11="false" f12="false" f13="false" f14="false" f15="false"
    f16="false" f17="false" f18="false" f19="false" f20="false" f21="false" f22="false" f23="false"
    f24="false" f25="false" f26="false" f27="false" f28="false" f29="false" f30="false" f31="false" f32="false"
    mass="0" minstep="0" maxstep="0" pwm="0" pwmcorrdiv="10" V_step="0" V_min="25" V_max="100" V_mode="kmh"
    bus="0" prot="P" protver="1" spcnt="100" fn="false" oid="" identifier="" mode="stop" iid="" imagenr="0"
    informall="false" wio="true"/>

<lc id="SFE" addr="10020" dir="true" V="27" server="mqttreader" iid="" shortid="" uid="0" sid="0" secaddr="0"
    placing="false" blockenterside="true" blockenterid="" modeevent="false" mode="stop" modereason=""
    resumeauto="false" manual="false" shunting="false" standalone="false" blockid="" destblockid=""
    fn="false" runtime="82966" mtime="0" rdate="1710880714" mint="0" throttleid="" active="true"
    waittime="0" scidx="-1" scheduleid="" tourid="" scheduleinithour="0" len="50" weight="0" train=""
    trainlen="50" trainweight="0" V_realkmh="0" fifotop="false" image="LEGO Santa Fe.png" imagenr="0"
    energypercentage="0" lookupschedule="false" pause="false" consist=""/>

<lc id="SFE" shortid="" uid="0" sid="0" dir="true" addr="10020" secaddr="0" V="27"
    placing="false" blockenterside="true" blockenterid="" modeevent="false" mode="stop" modereason=""
    resumeauto="false" manual="false" shunting="false" standalone="false" blockid="" destblockid=""
    fn="false" runtime="82966" mtime="0" rdate="1710880714" mint="0" throttleid="virtual" active="true"
    waittime="0" scidx="-1" scheduleid="" tourid="" scheduleinithour="0" len="50" weight="0" train=""
    trainlen="50" trainweight="0" V_realkmh="0" fifotop="false" image="LEGO Santa Fe.png" imagenr="0"
    energypercentage="0" lookupschedule="false" pause="false" consist=""/>

*/
void handleLc(const char *message, bool has_prev)
{
    char *id = nullptr;
    XMLParser::tryReadCharAttr(message, "id", &id);
    logger.vlogf(LOG_DEBUG, "handle lc %s", id);
    if (currentLc->id && strstr(id, currentLc->id) != nullptr) {

        char *Vmode;
        // we have a winner
        XMLParser::tryReadIntAttr(message, "V_max", &(currentLc->Vmax));
        // XMLParser::tryReadIntAttr(message, "V_Rmax", &(currentLoc->VRmax));
        // XMLParser::tryReadIntAttr(message, "V_Smax", &(currentLoc->VSmax));
        if (XMLParser::tryReadCharAttr(message, "V_mode", &Vmode)) {
            if (strstr(Vmode, "kmh") != nullptr) {
                currentLc->vModePercent = false;
            } else {
                currentLc->vModePercent = true;
            }
            free(Vmode);
        }
        XMLParser::tryReadIntAttr(message, "V", &(currentLc->V));      // current speed
        XMLParser::tryReadBoolAttr(message, "dir", &(currentLc->dir)); // current direction
        // placing="true" blockenterside="true"
        if (has_prev) {
            bool placing, blockenterside;
            XMLParser::tryReadBoolAttr(message, "placing", &(placing));
            XMLParser::tryReadBoolAttr(message, "blockenterside", &(blockenterside));
            currentLc->invdir = !placing; // ignore stuff
        }

        if (!currentLc->dir) {
            currentLc->V = -currentLc->V;
            currentLc->dir = true;
        }

        if (currentLc->invdir) {
            currentLc->V = -currentLc->V;
        }
        if (currentLc->newSpeed != currentLc->V || currentLc->initiated) {
            // only change if we have a speed change, igonre 0 because that is our
            currentLc->newSpeed = currentLc->V;
            rotaryEncoder.setBoundaries(-currentLc->Vmax, currentLc->Vmax, false);
            rotaryEncoder.setEncoderValue(currentLc->V);
            currentLc->initiated = false;
        }
        // see if we have fn stuff
        // <lc id="BR01" addr="8984" dir="true" V="0" server="mqttreader"
        // fncnt="2" f0="false" f1="false" f2="false" f3="false" f4="false" f5="false" f6="false" f7="false"
        // f8="false" f9="false" f10="false" f11="false" f12="false" f13="false" f14="false" f15="false"
        // f16="false" f17="false" f18="false" f19="false" f20="false" f21="false" f22="false" f23="false"
        // f24="false" f25="false" f26="false" f27="false" f28="false" f29="false" f30="false" f31="false"
        // f32="false" mass="0" minstep="0" maxstep="0" pwm="0" pwmcorrdiv="10"
        // V_step="0" V_min="40" V_max="120" V_mode="kmh" bus="0" prot="P" protver="1" spcnt="255"
        // fn="false" oid="" identifier="" mode="stop" iid="" imagenr="0" informall="false" wio="true"/>
        char fn[5];
        for (int i = 0; i < 33; i++) {
            fn[2] = 0;
            fn[3] = 0;
            fn[4] = 0;
            snprintf(fn, 4, "f%d", i);
            XMLParser::tryReadBoolAttr(message, fn, &(currentLc->fn[i].fn));
        }
        // handle
        //  <fundef fn="1" text="FWD" sound="" icon="" timer="0" iconnr="0" addr="0" onpos="0" offpos="0" mappedfn="0" pushbutton="true" ledaddr="0" ledcount="0" ledbrightness="100"/>
        // <fundef fn="2" text="REV" sound="" icon="" timer="0" iconnr="0" addr="0" onpos="0" offpos="0" mappedfn="0" pushbutton="true" ledaddr="0" ledcount="0" ledbrightness="100"/>
        // <fundef fn="3" text="OFF" sound="" icon="" timer="0" iconnr="0" addr="0" onpos="0" offpos="0" mappedfn="0" pushbutton="true" ledaddr="0" ledcount="0" ledbrightness="100"/>
        // scan for pushbutton and store this in the loco devinition, if it is a push button, send on and off events
        char *pos = (char *)message;
        // iterate through all <fundef>s
        bool firstFundef = false;
        while ((pos = strstr((char *)pos, "<fundef ")) != nullptr) {
            if (!firstFundef) {
                // clear all definitions
                for (int i = 0; i < 33; i++) {
                    currentLc->fn[i].pushbutton = false;
                }
                firstFundef = true;
            }
            handleFundef(pos);
            pos += 6; // make shure we try and find the next fundef
        }

        // display stuff ;-)
        display.updateLc(currentLc, rotaryEncoder.readEncoder());
    }
    if (id)
        free(id);
}

// handle
//  <fundef fn="1" text="FWD" sound="" icon="" timer="0" iconnr="0" addr="0" onpos="0" offpos="0" mappedfn="0" pushbutton="true" ledaddr="0" ledcount="0" ledbrightness="100"/>

void handleFundef(const char *message)
{
    int fn;
    bool pushbutton;
    XMLParser::tryReadIntAttr(message, "fn", &fn);
    XMLParser::tryReadBoolAttr(message, "pushbutton", &pushbutton);
    logger.vlogf(LOG_DEBUG, "fundef  %d, %d", fn, pushbutton);
    currentLc->fn[fn].pushbutton = pushbutton;
}

/*
<fn id="V100" addr="100" fncnt="2" f0="false" f1="false" f2="false" f3="false" f4="false" f5="false" f6="false" f7="false"
f8="false" f9="false" f10="false" f11="false" f12="false" f13="false" f14="false" f15="false" f16="false" f17="false" f18="false"
f19="false" f20="false" f21="false" f22="false" f23="false" f24="false" f25="false" f26="false" f27="false" f28="false" f29="false"
f30="false" f31="false" f32="false" throttleid="rv16964" fnchanged="0" group="0"/>

<fn fnchanged="0" fnchangedstate="false" id="V100" longclick="false" group="0" fncnt="2" addr="100" f0="false"
throttleid="virtual" controlcode="" slavecode="" server="infw03AA4984" iid="" uid="0" sid="0" dir="false"
secaddr="0" V="0" placing="true" blockenterside="true" blockenterid="sb01" modeevent="false" mode="stop" modereason=""
resumeauto="false" manual="false" shunting="false" standalone="false" blockid="" destblockid="" fn="false" runtime="30318"
mtime="0" rdate="1683568078" mint="0" active="true" waittime="0" scidx="-1" scheduleid="" tourid="" scheduleinithour="0" len="25"
 weight="0" train="" trainlen="25" trainweight="0" V_realkmh="0" fifotop="false" image="v100.png" imagenr="0" energypercentage="0"
 lookupschedule="false" pause="false" consist=""/>

*/
void handleFn(const char *message)
{
    char *id = nullptr;
    XMLParser::tryReadCharAttr(message, "id", &id);
    logger.vlogf(LOG_DEBUG, "handle fn %s", id);
    if (currentLc->id && strstr(id, currentLc->id) != nullptr) {
        char fn[5];
        for (int i = 0; i < 33; i++) {
            fn[2] = 0;
            fn[3] = 0;
            fn[4] = 0;
            snprintf(fn, 4, "f%d", i);
            XMLParser::tryReadBoolAttr(message, fn, &(currentLc->fn[i].fn));
        }
        // display stuff ;-)
        display.updateLc(currentLc, rotaryEncoder.readEncoder());
    }
    if (id)
        free(id);
}

void handleLCList(const char *message)
{
    const char *pos = message;
    logger.vlogf(LOG_DEBUG, "Going for delete loco list.");
    for (auto p : locs) {
        delete p;
    }
    locs.clear(); // delete all previous locs
    logger.vlogf(LOG_DEBUG, "Going for loco list.");
    lc *loc = NULL;
    while ((pos = strstr(pos, "<lc")) != nullptr) {
        // found a loc
        char *id = NULL;
        char *addr = NULL;
        if (!XMLParser::tryReadCharAttr(pos, "id", &id)) {
            // id can not be empty
        }
        if (!XMLParser::tryReadCharAttr(pos, "addr", &addr)) {
            // just ignore for now
        }
        loc = new lc(id, addr, NULL, 0, 0, 0);
        locs.push_back(loc);
        pos++;
    }
    logger.vlogf(LOG_DEBUG, "Got for loco list.");
}

void handleSWList(const char *message)
{
    const char *pos = message;
    for (auto p : switches) {
        delete p;
    }
    switches.clear(); // delete all previous locs
    logger.vlogf(LOG_DEBUG, "Going for switch list.");
    sw *swtch = NULL;
    while ((pos = strstr(pos, "<sw")) != nullptr) {
        char *id = NULL;
        if (!XMLParser::tryReadCharAttr(pos, "id", &id)) {
            // id can not be empty
        }
        swtch = new sw(id);
        switches.push_back(swtch);
        logger.vlogf(LOG_DEBUG, "id '%s'", id);
        pos++;
    }
}

void handleCoList(const char *message)
{
    const char *pos = message;
    for (auto p : outputs) {
        delete p;
    }
    outputs.clear(); // delete all previous locs
    logger.vlogf(LOG_DEBUG, "Going for co list.");
    co *outpt = NULL;
    while ((pos = strstr(pos, "<co")) != nullptr) {
        char *id = NULL;
        if (!XMLParser::tryReadCharAttr(pos, "id", &id)) {
            // id can not be empty
        }
        outpt = new co(id);
        outputs.push_back(outpt);
        logger.vlogf(LOG_DEBUG, "id '%s'", id);
        pos++;
    }
}

/*
<bklist>
  <bk id="bk01" desc=""/>
  <bk id="bk02" desc=""/>
  <bk id="bk22" desc=""/>
  <bk id="bk21" desc=""/>
  <bk id="bk11" desc=""/>
  <bk id="bk13" desc=""/>
  <bk id="bk23" desc=""/>
  <bk id="bk12" desc=""/>
</bklist>
*/
void handleBkList(const char *message)
{
    const char *pos = message;
    for (auto p : blocks) {
        delete p;
    }
    blocks.clear(); // delete all previous blocks
    logger.vlogf(LOG_DEBUG, "Going for bk list.");
    bk *blk = NULL;
    while ((pos = strstr(pos, "<bk")) != nullptr) {
        char *id = NULL;
        if (!XMLParser::tryReadCharAttr(pos, "id", &id)) {
            // id can not be empty
        }
        blk = new bk(id);
        blocks.push_back(blk);
        logger.vlogf(LOG_DEBUG, "id '%s'", id);
        pos++;
    }
}
/*
recogninsed type:
type="dcrossing"
type="threeway"
type="right"
type="left"
type="decoupler"

 state="straight"
 state="turnout"
 state="left"
 state="right"

*/
void handleSw(const char *message, bool from_infotopic)
{
    char *id = nullptr;
    XMLParser::tryReadCharAttr(message, "id", &id);
    logger.vlogf(LOG_DEBUG, "handle sw %s", id);
    if (currentSw->id && strstr(id, currentSw->id) != nullptr) {
        char *swtype = nullptr;
        if (XMLParser::tryReadCharAttr(message, "type", &swtype)) {
            if (strstr(swtype, "left") != nullptr) {
                currentSw->swtype = swLeft;
            }
            if (strstr(swtype, "right") != nullptr) {
                currentSw->swtype = swRight;
            }
            if (strstr(swtype, "threeway") != nullptr) {
                currentSw->swtype = swTriple;
            }
            if (strstr(swtype, "dcrossing") != nullptr) {
                currentSw->swtype = swDouble;
            }
            if (strstr(swtype, "decoupler") != nullptr) {
                currentSw->swtype = swDecoupler;
            }
        }
        char *swstate = nullptr;
        if (from_infotopic) {
            if (XMLParser::tryReadCharAttr(message, "state", &swstate)) {
                if (strstr(swstate, "left") != nullptr) {
                    currentSw->swstate = stLeft;
                }
                if (strstr(swstate, "right") != nullptr) {
                    currentSw->swstate = stRight;
                }
                if (strstr(swstate, "straight") != nullptr) {
                    currentSw->swstate = stStraight;
                }
                if (strstr(swstate, "turnout") != nullptr) {
                    currentSw->swstate = stTurnout;
                }
            }
        } else {
            if (XMLParser::tryReadCharAttr(message, "cmd", &swstate)) {
                if (strstr(swstate, "left") != nullptr) {
                    currentSw->swstate = stLeft;
                }
                if (strstr(swstate, "right") != nullptr) {
                    currentSw->swstate = stRight;
                }
                if (strstr(swstate, "straight") != nullptr) {
                    currentSw->swstate = stStraight;
                }
                if (strstr(swstate, "turnout") != nullptr) {
                    currentSw->swstate = stTurnout;
                }
            }
        }
        logger.vlogf(LOG_DEBUG, "swicth '%s' '%d' '%d'", id, currentSw->swtype, currentSw->swstate);
        display.updateSw(currentSw->swtype, currentSw->swstate);
        if (swtype)
            free(swtype);
        if (swstate)
            free(swstate);
    }
    if (id)
        free(id);
}
/*
<co id="co1" x="18" y="1" z="0" porttype="1" prev_id="co1" nr="1" desc="straat lampen" decid="" show="true"
    showid="true" svgtype="0" svgacctype="false" blockid="" routeids="" grpid="" tristate="false"
    toggleswitch="true" operable="true" showbri="true" iid="" bus="0" uidname="" addr="1" port="0"
    gate="0" param="0" paramoff="0" value="0" valueoff="0" delay="0" publish="" subscribe="" cmd_on=""
    cmd_off="" prot="D" inv="false" blink="false" colortype="false" plancolor="false" native="false"
    asswitch="false" accessory="false" redChannel="0" greenChannel="0" blueChannel="0" whiteChannel="0"
    white2Channel="0" brightnessChannel="0" state="off" actor="user">
  <color red="0" green="0" blue="0" white="0" white2="0" saturation="254" rgbType="0" brightness="0" id=""/>
</co>

*/
void handleCo(const char *message)
{
    char *id = nullptr;
    XMLParser::tryReadCharAttr(message, "id", &id);
    logger.vlogf(LOG_DEBUG, "handle co %s", id);
    if (currentCo->id && strstr(id, currentCo->id) != nullptr) {

        char *costate = nullptr;
        if (XMLParser::tryReadCharAttr(message, "state", &costate)) {
            if (strstr(costate, "on") != nullptr) {
                currentCo->on = true;
            }
            if (strstr(costate, "off") != nullptr) {
                currentCo->on = false;
            }
        }
        display.updateCo(currentCo->on);
        if (costate)
            free(costate);
    }
    if (id)
        free(id);
}
/*
<bk id="bk01" x="6" y="13" z="0" entering="false" managerid="" shunting="false" fifoids=""
    embeddedfbplus="false" embeddedfbmin="false" locid="FUX" state="open" prev_id="bk01"
    desc="" platform="" len="0" lenenter2in="0" radius="0" offsetplus="0" offsetminus="0"
    departdelay="0" fifosize="0" fifogap="0" randomrate="10" showlocoimage="true"
    electrified="true" gomanual="true" acceptghost="false" acceptident="false"
    terminalstation="false" wait="true" road="false" allowchgdir="true" smallsymbol="false"
    extstop="false" allowbbt="true" bbtfix="0" mainline="false" sleeponclosed="false"
    freeblockonenter="true" freeblockonentermaxlen="0" freeblockonenterplus="true"
    freeblockonentermin="true" freeblockonenterroute="false" allowaccessoncars="true"
    centertrain="false" secondnextblock4wait="false" polarisation="true" rearprotection="false"
    commuter="yes" show="true" virtual="false" slaveblocks="" ttid="" codesen="" signal="sg01-"
    wsignal="" signalR="sg01+" wsignalR="" statesignal="" openblocksignal="false" openblockid=""
    openblocksignalR="false" openblockidR="" resetsignalonexit="false" speed="mid" exitspeed="cruise"
    noreduceonexit="true" stopspeed="min"
    type="none,local,ice,goods,shunting,turntable,regional,light,lightgoods,post" nonewaittype=""
    incline="0" waitmode="fixed" waitside="0" maxkmh="0" minwaittime="1" maxwaittime="10" waittime="3"
    mvdistance="0" mvmph="false" mvrecord="false" evttimer="0" evttimer2="0" forceblocktimer="false"
    selectshortestblock="false" reservedevents="false" iid="" addr="0" port="0"
    td="false" tdV0="false" tdlinkblocks="true"
    typeperm="none,goods,local,mixed,cleaning,ice,post,light,lightgoods,regional,shunting"
    class="" era="63" masterid="" reserved="false" controlcode="" slavecode="" server="infw02ED7364"
    pre_move_x="5" pre_move_y="13" pre_move_z="0" offerid="" offervia="" clone="false">
  <fbevent id="fb01+" action="in" from="all-reverse"/>
  <fbevent id="fb01+" action="enter" from="all" byroute="all" endpuls="false" use_timer2="false"/>
  <fbevent id="fb01-" action="in" from="all" byroute="all" endpuls="false" use_timer2="false"/>
  <fbevent id="fb01-" action="enter" from="all-reverse"/>
</bk>
*/

void handleFbevent(const char *message)
{
    char *id = nullptr;
    XMLParser::tryReadCharAttr(message, "id", &id);
    if (id)
        currentBk->addSensor(id);
}

void handleBk(const char *message)
{
    char *id = nullptr;
    XMLParser::tryReadCharAttr(message, "id", &id);
    logger.vlogf(LOG_DEBUG, "handle bk %s", id);
    if (currentBk->id && strstr(id, currentBk->id) != nullptr) {
        char *pos = (char *)message;
        // iterate through all <fundef>s
        bool firstFbEvent = false;
        while ((pos = strstr((char *)pos, "<fbevent ")) != nullptr) {
            if (!firstFbEvent) {
                // clear all definitions
                currentBk->clearSensors();
                firstFbEvent = true;
            }
            handleFbevent(pos);
            pos += 6; // make shure we try and find the next fundef
        }
        display.updateBk(firstFbEvent, sensor_undefined, sensor_undefined);
    }
    if (id)
        free(id);
}

void handleFb(const char *message)
{
    char *id = nullptr;
    sensorT *sensor = nullptr;
    uint8_t count;
    XMLParser::tryReadCharAttr(message, "id", &id);
    logger.vlogf(LOG_DEBUG, "handle Fb %s", id);
    if (currentBk && (sensor = currentBk->findSensor(id, &count))) {
        bool newState;
        sensorStates states[2] = {sensor_undefined, sensor_undefined};
        logger.vlogf(LOG_DEBUG, "fb (%s) count %d", id, count);
        XMLParser::tryReadBoolAttr(message, "state", &(newState));
        sensor->state = newState ? sensor_on : sensor_off;
        currentBk->getSensorstates(states);
        display.updateBk(true, states[1], states[0]);
    }
    if (id)
        free(id);
}

void handleSgList(const char *message)
{
    const char *pos = message;
    for (auto p : signals) {
        delete p;
    }
    signals.clear(); // delete all previous blocks
    logger.vlogf(LOG_DEBUG, "Going for sg list.");
    sg *signal = NULL;
    while ((pos = strstr(pos, "<sg")) != nullptr) {
        char *id = NULL;
        int aspects = 0;
        if (!XMLParser::tryReadCharAttr(pos, "id", &id)) {
            // id can not be empty
        }
        signal = new sg(id);
        signals.push_back(signal);
        logger.vlogf(LOG_DEBUG, "id '%s'", id);
        pos++;
    }
}
/* handle sg
<sg type="semaphore" signal="distant" id="vs12-" x="4" y="2" z="0" addr1="23678" state="green"
manual="false" aspect="0" ori="east" porttype="0" prev_id="vs12-" nr="0" desc="" decid=""
blockid="bk12" freeid="" blankid="" road="false" oppositeid="false" routeids="[bk11-]-[bk12+]"
manualreset="true" operable="true" accnr="0" show="true" showid="true" sod="true" resetid="" iid=""
uidname="" bus="0" addr2="23678" addr3="23678" addr4="0" port1="1" port2="2" port3="3" port4="0" gate1="0"
gate2="0" gate3="0" gate4="0" prot="D" aspects="3" symbolprefix="" dwarf="false" usesymbolprefix="false"
usepatterns="0" inv="false" pair="false" asswitch="false" actdelay="false" delay="0" accessory="true" cmdtime="0"
dim="10" bri="100" param="0" ctciid1="" ctciid2="" ctciid3="" ctcuid1="" ctcuid2="" ctcuid3="" ctcbus1="0" ctcbus2="0"
ctcbus3="0" ctcaddr1="0" ctcaddr2="0" ctcaddr3="0" ctcflip1="false" ctcflip2="false" ctcflip3="false" ctciidled1=""
ctciidled2="" ctciidled3="" ctcbusled1="0" ctcbusled2="0" ctcbusled3="0" ctcaddrled1="0" ctcaddrled2="0" ctcaddrled3="0"
ctcportled1="0" ctcportled2="0" ctcportled3="0" ctcgateled1="0" ctcgateled2="0" ctcgateled3="0" ctcasswitchled1="false"
ctcasswitchled2="false" ctcasswitchled3="false" green="0" red="0" yellow="0" white="0" blank="0" greennr="0" rednr="0"
yellownr="0" whitenr="0" blanknr="0" a1nr="0" a2nr="0" a3nr="0" a4nr="0" a5nr="0" greenvalue="0" redvalue="0" yellowvalue="0"
whitevalue="0" blankvalue="0" a1value="0" a2value="0" a3value="0" a4value="0" a5value="0" aspectnames="" actor="user" clone="false"/>

get aspects -> number of aspects

<sg id="sg02-" cmd="flip" manualcmd="true"/>

2:
red
green

3:
red
green
yellow

4:
red
green
yellow
greem yellow

5:
red
green
yellow green
red white white
white

*/

void handleSg(const char *message)
{
    char *id = nullptr;
    // sg can be without id
    if (XMLParser::tryReadCharAttr(message, "id", &id)) {
        logger.vlogf(LOG_DEBUG, "handle sg %s", id);
        if (currentSg->id && strstr(id, currentSg->id) != nullptr) {
            int aspects = 0;
            int aspect = 0;
            char *state = nullptr;
            if (XMLParser::tryReadIntAttr(message, "aspects", &aspects)) {
                currentSg->setAspects(aspects);
            }
            if (XMLParser::tryReadIntAttr(message, "aspect", &aspect)) {
                currentSg->setAspect(aspect);
            }
            if (aspect == 0) {
                XMLParser::tryReadCharAttr(message, "state", &state);
                if (strstr(state,"red") != nullptr) {
                    currentSg->setAspect(0);
                } else if (strstr(state,"green") != nullptr) {
                    currentSg->setAspect(1);
                } else if (strstr(state,"yellow") != nullptr) {
                    currentSg->setAspect(2);
                }
            }
            display.updateSg(currentSg->aspect, currentSg->getAspects());
        }
    }
    if (id)
        free(id);
}

void handleSys(const char *message)
{
    char *cmd = nullptr;
    XMLParser::tryReadCharAttr(message, "cmd", &cmd);
    if (strstr(cmd, "shutdown") != nullptr) {
        // set the controller back to the initial state
        state.setInitial();
        display.setInitial();
        display.cls();
        display.displayAll(state.getMenu(), state.getSubMenu(), liionbat.getPercentage(), liionbat.getVoltage());
    }
    if (cmd)
        free(cmd);
}
void publishGetBlockInfo(char *blockId)
{
    char request[200];
    snprintf(request, 200, "<model cmd=\"bkprops\" val=\"%s\"/>", blockId);
    mqttClient.publish("rocrail/service/client", request);
}
void publishGetPowerState()
{
    mqttClient.publish("rocrail/service/client", "<state cmd=\"getstate\"/>");
}

void publishSetPowerStop()
{
    mqttClient.publish("rocrail/service/client", "<sys cmd=\"stop\" informall=\"true\"/>");
}
void publishSetPowerGo()
{
    mqttClient.publish("rocrail/service/client", "<sys cmd=\"go\" informall=\"true\"/>");
}
void publishSetPowerEBreak()
{
    mqttClient.publish("rocrail/service/client", "<sys cmd=\"ebreak\" informall=\"true\"/>");
}

void publishGetLocList()
{
    mqttClient.publish("rocrail/service/client", "<model cmd=\"lclist\" val=\"short\"/>");
}

void publishGetLocInfo(char *locId)
{
    char request[200];
    snprintf(request, 200, "<model cmd=\"lcprops\" val=\"%s\"/>", locId);
    mqttClient.publish("rocrail/service/client", request);
}

void publishGetSwitchList()
{
    mqttClient.publish("rocrail/service/client", "<model cmd=\"swlist\" val=\"short\"/>");
}

void publishGetSwitchInfo(char *switchId)
{
    char request[200];
    snprintf(request, 200, "<model cmd=\"swprops\" val=\"%s\"/>", switchId);
    mqttClient.publish("rocrail/service/client", request);
}

void publishGetOutputInfo(char *outputId)
{
    char request[200];
    snprintf(request, 200, "<model cmd=\"coprops\" val=\"%s\"/>", outputId);
    mqttClient.publish("rocrail/service/client", request);
}

void publishGetSignalInfo(char *signalId)
{
    char request[200];
    snprintf(request, 200, "<model cmd=\"sgprops\" val=\"%s\"/>", signalId);
    mqttClient.publish("rocrail/service/client", request);
}

void publishLoc()
{
    char request[200];
    snprintf(request, 200, "<lc id=\"%s\"/>", currentLc->id);
    mqttClient.publish("rocrail/service/client", request);
}

void publishLocSpeed(long locV)
{
    char request[200];
    bool dir = locV > 0;
    snprintf(request, 200, "<lc id=\"%s\" addr=\"%s\" dir=\"%s\" V=\"%ld\"/>", currentLc->id, currentLc->addr,
             dir ? "true" : "false", abs(locV));
    mqttClient.publish("rocrail/service/client", request);
}

void publishLocFn(int fn, bool fnValue)
{
    char request[200];
    snprintf(request, 200, "<fn id=\"%s\" addr=\"%s\" fnchanged=\"%d\" f%d=\"%s\" />", currentLc->id, currentLc->addr, fn, fn, fnValue > 0 ? "true" : "false");
    mqttClient.publish("rocrail/service/client", request);
}

void publishSwToggle()
{
    char request[200];
    snprintf(request, 200, "<sw id=\"%s\"  cmd=\"flip\" manualcmd=\"true\"/>", currentSw->id);
    mqttClient.publish("rocrail/service/client", request);
}

void publishCoState()
{
    char request[200];
    snprintf(request, 200, "<co id=\"%s\"  cmd=\"%s\"/>", currentCo->id, currentCo->on ? "on" : "off");
    mqttClient.publish("rocrail/service/client", request);
}
void publishSgState()
{
    char request[200];
    snprintf(request, 200, "<sg id=\"%s\"  cmd=\"flip\" manualcmd=\"true\"/>", currentSg->id);
    mqttClient.publish("rocrail/service/client", request);
}

void publishGetOutputsList()
{
    mqttClient.publish("rocrail/service/client", "<model cmd=\"colist\" val=\"short\"/>");
}

void publishGetBlocksList()
{
    mqttClient.publish("rocrail/service/client", "<model cmd=\"bklist\" val=\"short\"/>");
}

void publishGetSignalsList()
{
    mqttClient.publish("rocrail/service/client", "<model cmd=\"sglist\" val=\"short\"/>");
}