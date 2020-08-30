## xmt: The X11 multitool

A collection of various X11 utility scripts I've written.  My goal is for this tool to become a "multitool" in a similar (yet smaller) way to busybox, although it remains a work in progress.

### Current tools

Currently, `xmt` contains:

- `fade`: Fade the root window's background between two colors
- `bgd`: Set the root window's background color
- `tree`: List the tree of all open windows
- `img`: Set the display's wallpaper to an image
- `float`: Start a minimal floating window manager
- `web`: (Work in progress) Start a web server which forwards the display to a browser (effectively a minimal [xpra](https://xpra.org/trac/wiki/Clients/HTML5))
- `term`: Forward the contents of the display to a terminal (standalone project available at [https://github.com/Cubified/xtermview](https://github.com/Cubified/xtermview))

### Building and Running

`xmt` depends upon Xlib, two of its extensions, and `xdotool`, which will most likely be packaged under the names:

- libX11(-devel)
- libXdamage(-devel)
- libXfixes(-devel)
- xdotool

`xmt` can be compiled and run with:

     $ git clone --recurse-submodules https://github.com/Cubified/xmt
     $ make
     $ ./xmt

Additionally, all tools support passing "help" as their first argument for additional information.

If you would like to decrease compilation time, `tcc` can be used as a drop-in replacement for `gcc`:

     $ make fast

While the standard target compiles in 2-3 seconds on my system, this compiles in less than a tenth of a second (at the expense of added size and a slower runtime speed).

To test in a small X server (primarily for testing `xmt float`), run (requires Xephyr):

     $ ./xephyr/run.sh

To install to `~/.local/bin`:

     $ make install

### Why should I use this over {feh, xsetroot, xpra, ...}?

This project does not aim to be a replacement for any software which does the same things (and likely does them better), but rather a "swiss army knife" which centralizes a number of my once-disorganized X utilities into a single manageable, minimal, and fast binary.

### Credits

- [stb_image.h](https://github.com/nothings/stb)
- [httpserver.h](https://github.com/jeremycw/httpserver.h)
- [wsServer](https://github.com/Theldus/wsServer)
- [miniz](https://github.com/richgel999/miniz)
- [pako](https://github.com/nodeca/pako)
