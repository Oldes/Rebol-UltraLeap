[![Rebol-Ultraleap CI](https://github.com/Oldes/Rebol-C-Extension-Ultraleap/actions/workflows/main.yml/badge.svg)](https://github.com/Oldes/Rebol-C-Extension-Ultraleap/actions/workflows/main.yml)

# Rebol/Ultraleap

Ultraleap extension for [Rebol3](https://github.com/Oldes/Rebol3) (version 3.14.1 and higher)

## Usage
```rebol
ultraleap: import ultraleap

```

## Extension commands:


#### `_init` `:data` `:args` `:type`

* `data` `[struct!]`
* `args` `[block!]`
* `type` `[block!]`

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


## Other extension values:
```rebol
```
