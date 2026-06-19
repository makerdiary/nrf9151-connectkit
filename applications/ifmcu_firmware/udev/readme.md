# udev rules on Linux

On Linux, particularly Ubuntu 16.04+, you must configure udev rules to allow pyOCD to access the CMSIS-DAP interface from user space. Otherwise you will need to run pyOCD as root, using sudo, which is very highly discouraged. (You should never run pyOCD as root on any OS.) To help with this, the udev rule is included in this directory with the name `50.zephyr-dap.rules`.

To install, copy the rules file in this directory to `/etc/udev/rules.d/` on Ubuntu:

``` bash
sudo cp 50.zephyr-dap.rules /etc/udev/rules.d
```

To see your changes without a reboot, you can force the udev system to reload:

``` bash
sudo udevadm control --reload
```

``` bash
udo udevadm trigger
```

By default, the rules provide open access to the CMSIS-DAP interface for all users (0666 permissions). If you share your Linux system with other users, or just don't like the idea of write permission for everybody, you can replace `MODE:="0666"` with `OWNER:="yourusername"` to create the device owned by you, or with `GROUP:="somegroupname"` and mange access using standard Unix groups.
