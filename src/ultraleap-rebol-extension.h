//   ____  __   __        ______        __
//  / __ \/ /__/ /__ ___ /_  __/__ ____/ /
// / /_/ / / _  / -_|_-<_ / / / -_) __/ _ \
// \____/_/\_,_/\__/___(@)_/  \__/\__/_// /
//  ~~~ oldes.huhuman at gmail.com ~~~ /_/
//
// Project: Rebol/Ultraleap extension
// SPDX-License-Identifier: MIT
// =============================================================================
// NOTE: auto-generated file, do not modify!


#include "LeapC.h"
#include "rebol-extension.h"

#define SERIES_TEXT(s)   ((char*)SERIES_DATA(s))

#define MIN_REBOL_VER 3
#define MIN_REBOL_REV 19
#define MIN_REBOL_UPD 1
#define VERSION(a, b, c) (a << 16) + (b << 8) + c
#define MIN_REBOL_VERSION VERSION(MIN_REBOL_VER, MIN_REBOL_REV, MIN_REBOL_UPD)

typedef struct REBOL_LEAP_DATA {
	REBCNT hands;
	float  framerate;
	REBI64 timestamp;
	REBCNT policy;
	REBCNT tracking;
	LEAP_HAND left;
	LEAP_HAND right;
} REBOL_LEAP_DATA;

typedef struct reb_leap_connection {
	LEAP_CONNECTION*  connectionHandle;
	const LEAP_DEVICE_INFO* deviceInfo;
	REBOL_LEAP_DATA*  data;
} reb_leap_connection;

extern REBCNT Handle_UltraleapHandle;

extern u32* arg_words;
extern u32* type_words;

enum ext_commands {
	CMD_ULTRALEAP__INIT,
	CMD_ULTRALEAP_CONNECT,
	CMD_ULTRALEAP_POLICY,
	CMD_ULTRALEAP_TRACKING,
	CMD_ULTRALEAP_PAUSE,
	CMD_ULTRALEAP_RESUME,
};


int cmd__init(RXIFRM *frm, void *ctx);
int cmd_connect(RXIFRM *frm, void *ctx);
int cmd_policy(RXIFRM *frm, void *ctx);
int cmd_tracking(RXIFRM *frm, void *ctx);
int cmd_pause(RXIFRM *frm, void *ctx);
int cmd_resume(RXIFRM *frm, void *ctx);

enum ma_arg_words {W_ARG_0,
	W_ARG_DEVICE_ID,
	W_ARG_STATUS
};
enum ma_type_words {W_TYPE_0
};

typedef int (*MyCommandPointer)(RXIFRM *frm, void *ctx);

#define ULTRALEAP_EXT_INIT_CODE \
	"REBOL [Title: \"Rebol/Ultraleap Extension\" Name: ultraleap Type: module Version: 0.1.0 Author: Oldes Date: 26-May-2025/14:29:12 License: MIT Url: https://github.com/Oldes/Rebol-Ultraleap]\n"\
	"_init: command [data [struct!] args [block!] type [block!]]\n"\
	"connect: command [\"Open connection\"]\n"\
	"policy: command [\"Set or clear policy flags\" set [integer!] clear [integer!]]\n"\
	"tracking: command [\"Set TrackingMode\" mode [integer!]]\n"\
	"pause: command [\"Pause the service\"]\n"\
	"resume: command [\"Unpause the service\"]\n"\
	"register LEAP_VECTOR! make struct! [\n"\
	"    x [float32!]\n"\
	"    y [float32!]\n"\
	"    z [float32!]\n"\
	"]\n"\
	"register LEAP_QUATERNION! make struct! [\n"\
	"    x [float32!]\n"\
	"    y [float32!]\n"\
	"    z [float32!]\n"\
	"    w [float32!]\n"\
	"]\n"\
	"register LEAP_BONE! make struct! [\n"\
	"    prev [struct! LEAP_VECTOR!] \"The base of the bone closer to the heart.\"\n"\
	"    next [struct! LEAP_VECTOR!] \"The end of the bone further from the heart.\"\n"\
	"    width [float32!] {The average width of the flesh around the bone in millimeters.}\n"\
	"    rotation [struct! LEAP_QUATERNION!] {Rotation in world space from the forward direction.}\n"\
	"]\n"\
	"register LEAP_DIGIT! make struct! [\n"\
	"    finger_id [int32!]\n"\
	"    metacarpal [struct! LEAP_BONE!]\n"\
	"    proximal [struct! LEAP_BONE!]\n"\
	"    intermediate [struct! LEAP_BONE!]\n"\
	"    distal [struct! LEAP_BONE!]\n"\
	"    is_extended [uint32!]\n"\
	"]\n"\
	"register LEAP_PALM! make struct! [\n"\
	"    position [struct! LEAP_VECTOR!]\n"\
	"    stabilized [struct! LEAP_VECTOR!]\n"\
	"    velocity [struct! LEAP_VECTOR!]\n"\
	"    normal [struct! LEAP_VECTOR!]\n"\
	"    width [float32!]\n"\
	"    direction [struct! LEAP_VECTOR!]\n"\
	"    orientation [struct! LEAP_QUATERNION!]\n"\
	"]\n"\
	"register LEAP_HAND! make struct! [\n"\
	"    id [uint32!]\n"\
	"    flags [uint32!]\n"\
	"    type [int32!]\n"\
	"    confidence [float32!]\n"\
	"    visible_time [uint64!]\n"\
	"    pinch_distance [float32!]\n"\
	"    grab_angle [float32!]\n"\
	"    pinch_strength [float32!]\n"\
	"    grab_strength [float32!]\n"\
	"    palm [struct! LEAP_PALM!]\n"\
	"    thumb [struct! LEAP_DIGIT!]\n"\
	"    index [struct! LEAP_DIGIT!]\n"\
	"    middle [struct! LEAP_DIGIT!]\n"\
	"    ring [struct! LEAP_DIGIT!]\n"\
	"    pinky [struct! LEAP_DIGIT!]\n"\
	"    arm [struct! LEAP_BONE!]\n"\
	"]\n"\
	"register LEAP_DATA!: make struct! [\n"\
	"    hands [uint32!]\n"\
	"    framerate [float32!]\n"\
	"    timestamp [int64!]\n"\
	"    policy [uint32!]\n"\
	"    tracking [uint32!]\n"\
	"    left [struct! LEAP_HAND!]\n"\
	"    right [struct! LEAP_HAND!]\n"\
	"]\n"\
	"protect 'LEAP_DATA!\n"\
	"_init LEAP_DATA! [device-id status] []\n"\
	"protect/hide '_init\n"

#ifdef  USE_TRACES
#include <stdio.h>
#define debug_print(fmt, ...) do { printf(fmt, __VA_ARGS__); } while (0)
#define trace(str) puts(str)
#else
#define debug_print(fmt, ...)
#define trace(str) 
#endif

