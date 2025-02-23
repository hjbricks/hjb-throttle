# rrmaus

## TODOs

- [x] rewrite using locid and switchid
- [x] have fn's showing
- [x] handle mqtt fn
- [x] have fn's working
- [x] have stop working
- [x] OTA updates
- [x] working speed update
- [x] have direction working
- [x] working switches
- [x] auto update the version number
- [x] make gilab generate and upload a new firware version
- [x] have version 1 of the pcb, fail, wemos wifi battey blue-tooth has an extrem bad usb port
- [x] moving to an esp32 exp board with separte battery managemant board
- [x] make pcb for version 2
- [x] make software compatible with 240x320 and 240x240
- [x] make pcb for version 3
- [x] show OTA errors on screen
- [x] allow < co /> objects to be controlled
- [x] create 3d printable case


## How it works

This is a Rocrail&reg; throttle.

The code is based on sending asynchronous mqtt messages to and get a response from Rocrail&reg;.
Changes are send to Rocrail&reg; and the response is displayed.

**You need at least RocRail&reg version 2.1.4368** otherwise not everything (almost nothing) works.

The four menu buttons under the display do:
 - (1) select the locomotives 
 - (2) select a output or signals by pressing this button and rotate the button
 - (3) ebreak
 - (4) select the switches
If both 'select the locomotives' and 'select the switches' are pressed for 4 seconds, then the ESP will restart.
Holding the output button (2) and simultaneous rotating the button, you are able to select outputs and blocks.

The other six buttons from top to button
 - stop locomotive/flip switch
 - shift the function groups, the green circles on the screen represent the current selected group
 - f0
 - f1, f5, f9 or f13 depending on the group.
 - f2, f6, f10 or f14
 - f3, f7, f11 or f15
 - f4, f8, f12 or f16

 If you press the rotary button you can select either a locomotive or a switch. After pressing the rotary button again, you select the the item you want to control.

 By rotating the button in train mode, you can vary the speed of the locomotive.

 In the switch mode it shows the selected switch and it stage (the same as in Rocrail&reg;). For now the switch is in its default position and not as it is placed in the layout.
  
## configuration

The configuration is a binary generated file from a json file, why binary, it loads easier and there is no json library overhead.
Please make a copy of the config directory, to something like my-config and also make a my-data directory. When executing the command 
```python3.exe ..\bin\convert-json-bin.py --src .\my-config\config.json --dest .\my-data``` it converts the config.json file to binary files and stores them in my-data.

In the ```platformio.ini``` there is a line ```data_dir = $PROJECT_DIR/my-data```, this line is used by "Upload Filesystem Image" and "Upload Filesystem Image OTA".

```
  {
    "version"  : 3,
    "hostname" : "<hostname>",
    "wifi": { "SSID": "<ssid>", "password" : "<password>" },
    "loglevel": "DEBUG|INFO|WARNING|ERROR|FATAL", 
    "mqtt"   : { "broker" : "<mqtt.example.com>", 
                 "port"   : 1883,
                 "username" : "<not used>",
                 "password" : "<not used>", 
                 "ebreakOnDisconnect" : false },
    "update" : { "configUrl" : "http://192.168.178.10/esp/config/",
                 "manifestUrl": "http://192.168.178.10/esp/firmware/manifest",
                 "firmwareType": "esp32-rr2bricks-rrmaus"},
    "OTAPassword" : "<OTAPassword>"
  }
```
If you don't have a webserver (can be local) or do not want to auto update, leave ```configUrl``` and ```manifestUrl``` empty.

The config file should speak for itself, change it to values that work for you.

## platformio.ini, my_platformio.ini

Copy the ```my_platformio.ini.example``` to ```my_platformio.ini``` and change the values to make it work for you:
```
[env:esp32doit-devkit-v1_ota]
upload_port = <hostname>
upload_flags = 
  --auth=<OTAPassword>
  -P <OTA Port, needed for firewalls>
```
If you add values in my_platformio.ini that are also in platform.ini, I don't know what values are used, so take care!
The ```-P <port>``` is added to fix the OTA port and make it work if your ESP is in an other VLAN as your development host and have a firewall in between. Make sure that that port is open on both networks, otherwise it will not work!

## OTA password

The OTAPassword in ```my_platformio.ini``` and the uploaded ```config``` file need to be the same in order to have OTA firmware and config updates work. 

## auto versioning the firmare

The script ```../bin/auto_firmware_version.py```, defines the ```AUTO_VERSION``` variable and is filed with the content of ```git describe```. It is known to work on Windows 11, MacOS and Linux, your probably need to install Git for windows! 
 
