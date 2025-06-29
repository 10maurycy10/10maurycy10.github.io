---
title: "Using an MDX-540 CNC mill from Linux"
date: 2025-06-29
tags: ["programming"]
draft: false
---

Roland's CNC machines can't naively run G-code.
Instead, they include a *printer driver* that converts G-code into a binary format before it gets sent to the machine. 

This normally works fine, but makes it impossible to use from Linux. 
(and could be a problem under Windows in the future, given that the machine is long discontinued.)

As a workaround, I wrote a standalone program to convert G-code into the text-based but naively supported RML format:
[gcode2rml.c](gcode2rml.c)

Compilation:

```sh
gcc -lm gcode2rml.c -o gcode2rml
```

Usage:

```sh
./gcode2rml [input file.nc] [output file.rml] [nc]
```

If a file is not specified, standard input and standard output is used instead. 
Pass `nc` if the mill is set to "NC" mode, omit if it's set to "RML" mode.
(or else everything will be off by a factor of 10)

The generated RML code can be "printed" to run it on the machine:

```sh
tee /dev/usb/lp0 < file.rml
```

The mill's starting position will be used as the origin. 
If you want to use the coordinate system origin, prepend this sequence to the generated files:

```sh
!MC0;
^PA;
!ZEZ0;
!ZEX0Y0;
^PR;
```

(This will move to the origin, so make sure that's set somewhere where it won't crash)

# Supported G-code commands:

|Commands|Function|
|-|-|
|G00, G01|Linear movement (Same feed rate used for both)|
|G02, G03|Circular movement (Only with I,J,K)|
|G17, G18, G19|Set circular movement plane|
|G10|Set offset (tool radius offset is not supported)|
|G20, G21|Set units (mm/in)|
|G90, G91|Relative/absolute positioning|
|G49, G40|Clear tool length offset, ignored|
|S|Set spindle speed (approximate, should be within 400 RPM)|
|F|Set feed rate|
|M00, M01, M02, M30|Stop program, ignored|
|M03, M04|Start spindle (Always clockwise)|
|M05|Stop spindle|
|M06|Tool change, ignored|

# Notes:

I've done a fair bit of milling, but there may still be some bugs.
I recommend running a test cut on air to make sure the toolpath's sane.

Because it uses the (documented) text-based format, resolution is limited to 10 microns (even in NC mode).
This is somewhat annoying as the machine is capable of micron scale movements, and has a backlash of around 3 microns. 

4-axis machining is supported, although 4-axis polar interpolation is not.
The automatic tool changer is not supported because I don't have one. 

Because it uses only the documented text-based commands, it should work on other Roland machines, but I don't have any to test it on.
