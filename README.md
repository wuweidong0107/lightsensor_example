# Build

```Bash
$ make
$ make CROSS_COMPILE=aarch64-linux-    # for cross compile
```

# Run
```Bash
$ lightsensor
Usage: lightsensor [OPTION...]
    -l, --lightsensor     char device path
    -i, --input           input device path
    -e, --enable          enable device
    -r, --read            read event
    -h, --help            show this help message
Example:
    lightsensor -l /dev/lightsensor -i /dev/input/event3 -e 1 -r
```

```Bash
$ lightsensor -l /dev/lightsensor -i /dev/input/event3 -e 1 -r
waiting event...          
EV_SYN       SYN_REPORT           00000000            
EV_ABS       ABS_MISC             00000000            
EV_SYN       SYN_REPORT           00000000            
EV_ABS       ABS_MISC             00000001            
EV_SYN       SYN_REPORT           00000000            
EV_ABS       ABS_MISC             00000004            
EV_SYN       SYN_REPORT           00000000 
```
