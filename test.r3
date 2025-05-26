Rebol [
	title: "Rebol/Ultraleap extension test"
]

print ["Running test on Rebol build:" mold to-block system/build]

;; make sure that we load a fresh extension
try [system/modules/ultraleap: none]
system/options/modules: to-real-file %build/

leap: import ultraleap
? leap

;- do some test with the extension -;

probe conn: leap/connect
data: leap/LEAP_DATA!
loop 10 [
	wait 0.1
	print ["hands:" data/hands "framerate:" data/framerate "policy:" data/policy]
]

probe leap/pause
loop 10 [
	wait 0.1
	print ["hands:" data/hands "framerate:" data/framerate "policy:" data/policy "paused"]
]
probe leap/resume

loop 100 [
	wait 0.1
	L: data/left
	R: data/right
	print ajoin [
		"^[[H^[[J" ;; clears screen
		reform ["hands:" data/hands "framerate:" data/framerate] LF
		reform ["left:" L/id "pinch:" L/pinch_strength "grab:" L/grab_strength "pos:" mold/flat L/palm/position] LF
		reform ["righ:" R/id "pinch:" R/pinch_strength "grab:" R/grab_strength "pos:" mold/flat R/palm/position] LF
		R/palm
	]
]
print ["status:" conn/status "pid:" conn/device-id]
leap/pause

print "done"
;halt

