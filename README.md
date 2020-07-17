## xmt: The X11 multitool

A collection of various X11 utility scripts I've written.  My goal is for this tool to become a "multitool" in a similar (yet smaller) way to busybox, although it remains a work in progress.

### Current tools

Currently, `xmt` contains:

- `fade`: Fade the root window's background between two colors
- `bgd`: Set the root window's background color
- `tree`: List the tree of all open windows

### Building and Running

`xmt` depends only upon Xlib, and can be compiled and run with:

     $ make
     $ ./xmt

Additionally, all tools support passing "help" as their first argument for additional information.
