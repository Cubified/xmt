## xmt: The X11 multitool

A collection of various X11 utility scripts I've written.  My goal is for this tool to become a "multitool" in a similar (yet smaller) way to busybox, although it remains a work in progress.

### Current tools

Currently, `xmt` contains:

- `fade`: Fade the root window's background between two colors
- `bgd`: Set the root window's background color
- `tree`: List the tree of all open windows
- `img`: Set the display's wallpaper to an image
- `float`: A minimal floating window manager

And plans exist to add:

- `tile`: A minimal tiling window manager
- `web`: Start a web server which forwards the display to a browser (effectively a minimal [xpra](https://xpra.org/trac/wiki/Clients/HTML5))

### Building and Running

`xmt` depends only upon Xlib, and can be compiled and run with:

     $ make
     $ ./xmt

Additionally, all tools support passing "help" as their first argument for additional information.

If you would like to decrease compilation time, `tcc` can be used as a drop-in replacement for `gcc`:

     $ make fast

While the standard target compiles in 2-3 seconds on my system, this compiles in less than a tenth of a second (at the expense of added size and a slower runtime speed).

### Why should I use this over {feh, xsetroot, xpra, ...}?

This project does not aim to be a replacement for any software which does the same things (and likely does them better), but rather a "swiss army knife" which centralizes a number of my once-disorganized X utilities into a single manageable, minimal, and fast binary.
