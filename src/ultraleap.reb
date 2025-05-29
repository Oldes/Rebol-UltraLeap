;- This code is being imported into the native extension -;
REBOL [
	title:  "Rebol/Ultraleap Extension"
	name:    ultraleap
	type:    module
	date:    22-May-2025
	home:    https://github.com/Oldes/Rebol-Ultraleap
	version: 0.0.1
	author: @Oldes
	License: MIT
]
;; A three element, floating-point vector.
register LEAP_VECTOR! make struct! [
	x [float32!] ;; The x spatial coordinate.
	y [float32!] ;; The y spatial coordinate.
	z [float32!] ;; The z spatial coordinate.
]
;; A four element, floating point quaternion.
register LEAP_QUATERNION! make struct! [
	x [float32!] ;; The x coefficient of the vector portion of the quaternion.
	y [float32!] ;; The y coefficient of the vector portion of the quaternion.
	z [float32!] ;; The z coefficient of the vector portion of the quaternion.
	w [float32!] ;; The scalar portion of the quaternion.
]
;; Describes a bone's position and orientation.
register LEAP_BONE! make struct! [
	prev     [struct! LEAP_VECTOR!]     ;; The base of the bone closer to the heart.
	next     [struct! LEAP_VECTOR!]     ;; The end of the bone further from the heart.
	width    [float32!]                 ;; The average width of the flesh around the bone in millimeters.
	rotation [struct! LEAP_QUATERNION!] ;; Rotation in world space from the forward direction.
]
;; Describes the digit of a hand.
register LEAP_DIGIT! make struct! [
	;; The Leap identifier of this finger.
	finger_id    [int32!]
    ;; The finger bone wholly inside the hand.
    ;; For thumbs, this bone is set to have zero length and width, an identity basis matrix,
    ;; and its joint positions are equal.
    ;; Note that this is anatomically incorrect; in anatomical terms, the intermediate phalange
    ;; is absent in a real thumb, rather than the metacarpal bone. In the Ultraleap Tracking model,
    ;; however, we use a "zero" metacarpal bone instead for ease of programming.
	metacarpal   [struct! LEAP_BONE!]
	proximal     [struct! LEAP_BONE!] ;; The phalange extending from the knuckle.
	intermediate [struct! LEAP_BONE!] ;; The bone between the proximal phalange and the distal phalange.
	distal       [struct! LEAP_BONE!] ;; The distal phalange terminating at the finger tip.

	is_extended  [uint32!] ;; Reports whether the finger is more or less straight.
]
;; Properties associated with the palm of the hand.
register LEAP_PALM! make struct! [
	;; The center position of the palm in millimeters from the Ultraleap Tracking camera device origin.
	position    [struct! LEAP_VECTOR!]
	;; The time-filtered and stabilized position of the palm.
	;;
	;; Smoothing and stabilization is performed in order to make
	;; this value more suitable for interaction with 2D content. The stabilized
	;; position lags behind the palm position by a variable amount, depending
	;; primarily on the speed of movement.
	stabilized  [struct! LEAP_VECTOR!]
	;; The rate of change of the palm position in millimeters per second.
	velocity    [struct! LEAP_VECTOR!]
	;; The normal vector to the palm. If your hand is flat, this vector will
	;; point downward, or "out" of the front surface of your palm.
	normal      [struct! LEAP_VECTOR!]
	;; The estimated width of the palm when the hand is in a flat position.
	width       [float32!]
	;; The unit direction vector pointing from the palm position toward the fingers.
	direction   [struct! LEAP_VECTOR!]
	;; The quaternion representing the palm's orientation
	;; corresponding to the basis {normal x direction, -normal, -direction}
	orientation [struct! LEAP_QUATERNION!]
]
;; Describes a tracked hand.
register LEAP_HAND! make struct! [
	id             [uint32!]     ;; A unique ID for a hand tracked across frames.
	flags          [uint32!]     ;; Reserved for future use.
	type           [int32!]      ;; Identifies the chirality of this hand
	confidence     [float32!]    ;; How confident we are with a given hand pose. Not currently used (always 1.0).
	visible_time   [uint64!]     ;; The total amount of time this hand has been tracked, in microseconds.
	pinch_distance [float32!]    ;; The distance between index finger and thumb.
	grab_angle     [float32!]    ;; The average angle of fingers to palm.
	pinch_strength [float32!]    ;; The normalized estimate of the pinch pose - a pinch is between
	                             ;; the thumb and any other finger.
	grab_strength  [float32!]    ;; The normalized estimate of the grab hand pose - a grab is all four fingers
	                             ;; (excluding thumb) curled towards the palm.
	palm   [struct! LEAP_PALM!]  ;; Additional information associated with the palm.
	;; The fingers of this hand.
	thumb  [struct! LEAP_DIGIT!]
	index  [struct! LEAP_DIGIT!]
	middle [struct! LEAP_DIGIT!]
	ring   [struct! LEAP_DIGIT!]
	pinky  [struct! LEAP_DIGIT!]
	;; The arm to which this hand is attached.
	arm    [struct! LEAP_BONE!]
]

;; Main data struct used to track both hands 
register LEAP_DATA!: make struct! [
	hands     [uint32!]        ;; The number of hands tracked in the last event.
	framerate [float32!]       ;; Current tracking frame rate in hertz.
	timestamp [int64!]         ;; 
	policy    [uint32!]
	tracking  [uint32!]        ;; Current tracking mode
	left  [struct! LEAP_HAND!]
	right [struct! LEAP_HAND!]
]

;; protect the struct's value from modification
protect 'LEAP_DATA!


