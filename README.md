# moxa-v2406c-hotswapd

### Files:
mxhtsp_lx.c - source of library
mxhtspd.c mxhtspd.h - source of mxhtspd daemon
mxsetled.c - source code to setup led status
mxhtsp_ioctl.h - define ioctl number and structure negotiating with driver

#### Compile and install the hotswap daemon
```
$ make
$ make install
$ systemctl daemon-reload
$ systemctl enable mx_hotswapd.service
```

#### Clean the hotswap daemon
```
$ make clean
```
