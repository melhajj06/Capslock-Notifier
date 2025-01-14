# CapsLock Notifier
This is a small program that I created since it's hard to tell on my keyboard if capslock is on or not. I made a small icon in the Windows notification tray tell me instead.
### WINDOWS ONLY
#
## Building
I used this project as an introduction to [premake](https://premake.github.io/) as well, so I've included a `premake5.lua` script that creates project files and places them in a `build/` directory.\
Run `premake5 xxxx` where `xxxx` is the preferred toolchain (`premake5 vs2022` would create project files for Visual Studio 2022).\
Once the project files are created, the project can be built and the executable will be placed in `build/bin/`.
## Additional Remarks
To have this program start at Windows startup, follow these instructions:
- Create a shortcut of the executable
- Press `win + r` (or manually open `run`)
- Type `shell:startup` and hit enter
- Place the shortcut in the folder that opens
