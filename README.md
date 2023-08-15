# Build
```
$ make
```

# Run
```Bash
$ sudo ./led 
Usage: led [OPTION...]
  -l, list              list available led device
  -p, path              led sysfs path
  -w, --write           write to led
  -h, --help            show this help message
```

```Bash
$ sudo ./led -l
phy0-led
input2::numlock
input27::scrolllock
input27::numlock
input2::capslock
input27::capslock
input2::scrolllock
input27::kana
input27::compose
mmc0::
```

```Bash
$ sudo ./led -p input2::capslock -w 1
1
$ sudo ./led -p input2::capslock -w 0
0
```
