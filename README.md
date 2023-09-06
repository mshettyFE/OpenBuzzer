An ESP8266-based quizbowl buzzer system.

# Downloading Code and Setting up Environment

An important note: I developed this on a Windows machine using WSL. I have no reason to believe that this code won't work across platforms, but your mileage may vary.

You first need to download VSCode and get the PlatformIO extension.

VSCode can be downloaded from [here](https://code.visualstudio.com/), while the PlatformIO instructions can be found [here](https://platformio.org/install/ide?install=vscode). The [quick start guide](https://docs.platformio.org/en/latest/integration/ide/vscode.html#quick-start) will give you an idea of the basic functionality of the extension.

You will also need the C/C++ Extension Pack (Can search and install with extension manager )

Once the above development tools are installed, open up a terminal and download the repository with the command

```
git clone https://github.com/mshettyFE/OpenBuzzer.git
```

NOTE: for Windows with WSL, you need to open up a WSL terminal instead of a typical powershell windows terminal.

cd into downloaded repository, then fire up VSCode with ```code .``` and click on the PlatformIO icon on the left hand taskbar. Choose the "Pick a Folder" option and select the root folder of the git repo you just downloaded. Wait for all the configurations to get setup.

# Uploading Code (Client)

In general, you need to do the following steps:

1. Copy over a .cpp file in the [scripts](/scripts/) directory into [main.cpp](/src/main.cpp).
2. Plug in the ESP8266 NODEMCU dev board to a USB port
   * Aside: If you are using WSL, you need to passthrough the USB connection from Windows to WSL. See [here](https://learn.microsoft.com/en-us/windows/wsl/connect-usb) for instructions on how to do so.
3. Navigate to the PlatformIO icon in the taskbar, then select "Upload". It should just work.

For client machines (ie. each individual buzzer), copy over [Client.cpp](/scripts/ClientSide/Client.cpp) to main.

## Uploading filesystem (Server only)

For the main control unit (ie. server), you also need to flash the filesystem image prior to uploading the code. Follow steps 1-2 above, then under the PlatformIO tab, select ```Build Filesystem Image```. Afterwards, select ```Upload Filesystem Image```.

Once that is done, you can copy over [Server.cpp](/scripts/ServerSide/Server.cpp) to main and upload like normal.

# Ordering PCBs

The gerber files need to create the PCBs are located at [here](/design/PCBs/Gerber). These specific design files work for [JLCPCB](https://jlcpcb.com/). Just drag and drop the zip files and order however many you want (Suggested is 5 Server boards and 15 client boards).

If you are ordering from a different manufacturer, or some of your component footprints are egregiously misaligned with the board (the NODEMCU form factor is not particularly well standardized), you can directly modify the design files located [here](/design/PCBs/KiCad/).