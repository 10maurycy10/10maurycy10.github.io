---
title: New virus (Windows Copypasta)
tags: ["rant", "linux", "windows"]
date: 2022-09-14
draft: true
---

![Windows 10 logo](./windows10.svg)

Have you heard of the new Computer virus? The virus is called Windows, and 
while not itself new, A new strain is even more dangerous than the last.

It installs bloat on your PC that slows down your CPU and uses almost half of 
your ram, and installs spyware that tracks your every keystroke. But the worst 
part is that you have to pay 100 to change your desktop background and another 
100 to use office programs, and remove ads on startup. Most people assume there
old computers have gotten slow, when a worsening Windows infection is to blame.

There are many versions of the Windows virus, but the most common one is 
"Windows 10" with "Windows 11" quickly catching up. Windows is very dangerous 
as many normal people don't know that it is a virus, and it backdoors your 
system allowing *thousands* of other viruses in. (The only way to stop this is 
by installing *paid* software like Avast.)

# Prevalence

What is worse is that the virus is very common, infecting oven 74% of desktops 
and 28% of all devices, making the most prevalent virus.

In fact, lots of computers come with the virus from the factory!

# Testing

At this point, you might be wondering if your device is infected with the 
Windows virus. Fortunately, the virus is quite easy to notice, just look for 
these indicators:

- Computer hot when idle (Very common effect on older devices)

- Name of drive changed to C:\ (Universal)

- Some variant of the 4 flags/squares logo on your screen during boot. (Universal)

- High ram usage when idle. (>1GB)

- "Activate Windows: Go to settings to activate Windows" watermark in bottom right of screen. (If you have not paid)

- Other virus infection. (Most viruses use Windows to infect your system)

- Ads on boot and in start menu

If your computer shows more than 2-3 of these, you probably have windows.

To be sure go to Start Menu -> Run, type ``winver`` and hit enter.  If you have
a windows infection, you should get a window with some information such as version.

# Removal

Because of the viruses very deep embedding in your PC, removal can be quite 
challenging and requires a substantial amount of work to remove (and a USB drive,
just a 4 GB will do).

0. First, make sure you have backups of important data, this can be done on the 
same USB as used to install it, but the backups must be made after step \#4.

1. Download the latest version of Rufus, (https://rufus.ie/en/#download), This 
tool can bypass the virus's self protection measures.

2. Download a clean system from (https://linuxmint.com/download.php).

3. Run Rufus (as administrator) and select the downloaded .iso file and USB 
drive. Select "iso" mode when prompted.

4. If you want to use the same drive for backups and installing, copy then 
files now. If you have a separate drive, verify you backups are readable.

5. Restart your PC and enter the setup menu. This is done by pressing the "BIOS key" 
while the computer is turning on. (The key is sometimes shown during boot. If 
not, use the internet to find the key.)

6. Go thru the menu looking for these options (Names can very) (Don't worry, 
you cant break anything):
	
	- Secure boot: Should be *disabled* to prevent windows from protecting itself
	
	- EFI over USB/USB Boot: Should be enabled

	- OS Optimized defaults: Disabled or set to other

	- Boot order: Move "USB HDD" or the name of your drive to the top, enabling if needed. (This may require a second reboot)

7. Select "Exit, saving settings" or similar option and you should be greeted 
with the linux mint logo (stylized 'm'). If you are not make sure the options 
are set right values, or get help from a friend.

8. Select "Try Linux mint", and make sure that all the hardware you rely on 
works fine. (Windows infections often cause hardware not work properly without 
the virus.)

9. Finally you can run the installer. (It has adequate explanation)

10. You probably want to select the "Use entire disk" to remove all traces of 
the virus from your system.

11. When prompted, remove the drive and reboot, you should now have removed the Windows virus.

12. After the initial setup, you are done. Make sure to install updates (this 
can be done from the settings screen), to keep viruses out.

Congratulations, you now have a Windows free system. If this was an older computer,
you will likely be amazed by how fast it is without windows.

Use FOSS and be safe.

--- Feel free to share this copypasta, atribuition not required.

[Copy/Download this](./windows.txt)
