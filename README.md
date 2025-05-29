![rebol-ultraleap](https://github.com/user-attachments/assets/2be73e03-ab9f-499e-877c-40026c3c162c)

# Rebol/Ultraleap

Ultraleap extension for [Rebol3](https://github.com/Oldes/Rebol3) (version 3.19.1 and higher)

## Usage
```rebol
;; Import the extension
leap: import ultraleap

;; Connect the device
conn: leap/connect

;; The LEAP_DATA! struct is updated automatically by the connection
;; see: https://github.com/Oldes/Rebol-UltraLeap/blob/main/src/ultraleap.reb
data: leap/LEAP_DATA!

;; Shortcuts to left and right hand
L: data/left
R: data/right
;; Access this data as needed
loop 10 [
	wait 0.1 ;; small delay to let data update
	print ["hands:" data/hands "framerate:" data/framerate "policy:" data/policy]
    print ["L:" L/id "pinch:" L/pinch_strength "grab:" L/grab_strength "pos:" mold/flat L/palm/position] LF
    print ["R:" R/id "pinch:" R/pinch_strength "grab:" R/grab_strength "pos:" mold/flat R/palm/position] LF
]
```

## Extension commands:

#### `connect`
Open connection

#### `policy` `:set` `:clear`
Set or clear policy flags
* `set` `[integer!]`
* `clear` `[integer!]`

#### `tracking` `:mode`
Set TrackingMode
* `mode` `[integer!]`

#### `pause`
Pause the service

#### `resume`
Unpause the service


