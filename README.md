# PIMU - A Switch 2 Pro controller simulator
PIMU is a combination of desktop app and Raspberry pico firmware to play Switch 2 games with keyboard and mouse!

![The desktop app](./Media/DesktopScreenshot.png)

The goal of this tool is to convert mouse (Pointer) signals to gyro (IMU) controls. This is achieved by having a Pico be plugged into the switch 2 via USB and simulating pro controller 2 behavior, which receives it's inputs from a desktop running the PIMU desktop app (connected via bluetooth or a UART TTL interface).

# TODO
Most of the software works, except for
- Gyro Controls
- UART TTL connection