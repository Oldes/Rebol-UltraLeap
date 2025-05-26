REBOL [
	title:  "Rebol/Ultraleap module builder"
	type:    module
	date:    22-May-2025
	home:    https://github.com/Oldes/Rebol-Ultraleap
	version: 0.0.1
	author: @Oldes
]

;- all extension command specifications ----------------------------------------
commands: [
	_init: [
		;; used internaly only!
		data [struct!] ;; data struct which will contain resolved data
		args [block!]
		type [block!]
	]
	connect:  ["Open connection"]
	policy:   ["Set or clear policy flags" set [integer!] clear [integer!]]
	tracking: ["Set TrackingMode" mode [integer!] "Desktop = 0, Head-mounted = 1, ScreenTop = 2"]
	pause:    ["Pause the service"]
	resume:   ["Unpause the service"]
]

ext-values: {}

handles: make map! [
	ultraleap-handle: [
		"Ultraleap handle"
		device-id    integer!  none                "Device ID"
		status       integer!  none                ""
	]
]

arg-words:   copy []
handles-doc: copy {}

foreach [name spec] handles [
	append handles-doc ajoin [
		LF LF "#### __" uppercase form name "__ - " spec/1 LF
		LF "```rebol"
		LF ";Refinement       Gets                Sets                          Description"
	]
	foreach [name gets sets desc] next spec [
		append handles-doc rejoin [
			LF
			#"/" pad name 17
			pad mold gets 20
			pad mold sets 30
			#"^"" desc #"^""
		]
		append arg-words name
	]
	append handles-doc "^/```"
]
;print handles-doc
arg-words: unique arg-words

type-words: [
	;@@ Order is important!
]


;-------------------------------------- ----------------------------------------
reb-code: ajoin [
	{REBOL [Title: "Rebol/Ultraleap Extension"}
	{ Name: ultraleap Type: module}
	{ Version: 0.1.0}
	{ Author: Oldes}
	{ Date: } now/utc
	{ License: MIT}
	{ Url: https://github.com/Oldes/Rebol-Ultraleap}
	#"]"
]
logo: next {
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
}

enu-commands:  "" ;; command name enumerations
cmd-declares:  "" ;; command function declarations
cmd-dispatch:  "" ;; command functionm dispatcher

ma-arg-words: "enum ma_arg_words {W_ARG_0"
ma-type-words: "enum ma_type_words {W_TYPE_0"

;- generate C and Rebol code from the command specifications -------------------
foreach [name spec] commands [
	append reb-code ajoin [lf name ": command "]
	new-line/all spec false
	append/only reb-code mold spec

	name: form name
	replace/all name #"-" #"_"
	replace/all name #"?" #"q"
	
	append enu-commands ajoin ["^/^-CMD_ULTRALEAP_" uppercase copy name #","]

	append cmd-declares ajoin ["^/int cmd_" name "(RXIFRM *frm, void *ctx);"]
	append cmd-dispatch ajoin ["^-cmd_" name ",^/"]
]

;- additional Rebol initialization code ----------------------------------------

foreach word arg-words [
	word: uppercase form word
	replace/all word #"-" #"_"
	replace/all word #"?" #"Q"
	append ma-arg-words ajoin [",^/^-W_ARG_" word]
]

foreach word type-words [
	word: uppercase form word
	replace/all word #"-" #"_"
	append ma-type-words ajoin [",^/^-W_TYPE_" word]
]

append ma-arg-words "^/};"
append ma-type-words "^/};"

append reb-code LF
append reb-code mold/only load %ultraleap.reb
append reb-code ajoin [{
_init LEAP_DATA! } mold/flat arg-words SP mold/flat type-words {
protect/hide '_init}
]
;append reb-code {}

print reb-code

;- convert Rebol code to C-string ----------------------------------------------
init-code: copy ""
foreach line split reb-code lf [
	replace/all line #"^"" {\"}
	append init-code ajoin [{\^/^-"} line {\n"}] 
]

print init-code

;-- C file ultraleaps -----------------------------------------------------------
header: {$logo

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

enum ext_commands {$enu-commands
};

$cmd-declares

$ma-arg-words
$ma-type-words

typedef int (*MyCommandPointer)(RXIFRM *frm, void *ctx);

#define ULTRALEAP_EXT_INIT_CODE $init-code

#ifdef  USE_TRACES
#include <stdio.h>
#define debug_print(fmt, ...) do { printf(fmt, __VA_ARGS__); } while (0)
#define trace(str) puts(str)
#else
#define debug_print(fmt, ...)
#define trace(str) 
#endif

}
;;------------------------------------------------------------------------------
ctable: {$logo

#include "ultraleap-rebol-extension.h"
MyCommandPointer Command[] = {
$cmd-dispatch};
}

;- output generated files ------------------------------------------------------
write %ultraleap-rebol-extension.h reword :header self
write %ultraleap-commands-table.c  reword :ctable self



;- README documentation --------------------------------------------------------
doc: clear ""
hdr: clear ""
arg: clear ""
cmd: desc: a: t: s: readme: r: none

parse commands [
	any [
		quote init-words: skip
		|
		set cmd: set-word! into [
			(clear hdr clear arg r: none)
			(append hdr ajoin [LF LF "#### `" cmd "`"])
			set desc: opt string!
			any [
				set a word!
				set t opt block!
				set s opt string!
				(
					unless r [append hdr ajoin [" `:" a "`"]]
					append arg ajoin [LF "* `" a "`"] 
					if t [append arg ajoin [" `" mold t "`"]]
					if s [append arg ajoin [" " s]]
				)
				|
				set r refinement!
				set s opt string!
				(
					append arg ajoin [LF "* `/" r "`"] 
					if s [append arg ajoin [" " s]]
				)
			]
			(
				append doc hdr
				append doc LF
				append doc any [desc ""]
				append doc arg
			)
		]
	]
]


try/with [
	readme: read/string %../README.md
	readme: clear find/tail readme "## Extension commands:"
	append readme ajoin [
		LF doc
		LF LF
		LF "## Other extension values:"
		LF "```rebol"
		trim/tail ext-values
		LF "```"
		LF
	]
	write %../README.md head readme
] :print



