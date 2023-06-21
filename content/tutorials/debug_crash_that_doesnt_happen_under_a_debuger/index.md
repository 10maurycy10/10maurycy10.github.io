---
title: "Debug crash that doesn't happen under a debugger"
date: 2023-06-21T16:17:25-07:00
tags: []
draft: false
---

There are many cases where a program crashes in a hard to reproduce way.
For example using debugger like gdb can change the layout in memory, or change the timing of a race condition.

Under Linux, there is a super simple, but somewhat little known way to debug the program in this case, coredumps.
A coredump stores the state of a program at the moment it crashed: memory, registers, everything.

If the system uses systemd, `coredumpctl -r` will show all recent coredumps made by a system in reverse chronological order (newest first). (they are deleted after a week or so to save space)
To get the coredump, the easiest way is to used `coredumpctl dump -r [EXECUTABLE] > coredump`, replacing `[EXECUTABLE]` with the name of the program. 

Loading this file into a debugger is debugger specific, but under gdb, first load the program as normal, then run `core [COREDUMP]` replacing `[COREDUMP]` with the coredump file.


