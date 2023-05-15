---
title: "How not to (securely) delete a file"
date: 2023-05-11T10:56:19-07:00
tags: []
draft: false
---

So, you have a file, that you don't want anymore.
Everyone knows that moving it to the trash won't permanently delete it, but what about the permanent delete option?
While it might seem like the file is gone for good, it still remains on the drive, you just can't find it (data recovery and forensics tools can however).

To test this, first create a small file:

DISCLAIMER: These instructions work on linux and possibly other Unix like OS's like BSD or MacOS. Windows will require different steps.

```sh 
cat > test_file <<EOF
This is some sample file data, it does not matter, as long as it is recognizable
EOF
```
Now, use this command to find where the file's data is located on the drive, this will need root (prefix with `sudo` or run in a root shell).

```sh
hdparm --fibmap test_file
```

The output will vary depending on your system's configuration, on mine it produces this:

```
test_file:
 filesystem blocksize 4096, begins at LBA 0; assuming 512 byte sectors.
 byte_offset  begin_LBA    end_LBA    sectors
           0  241046952  241046959          8
```

Basically, this tells us that the file starts at start of sector (if `byte_offest`  in zero) 241046952 and ends at 241046959, spanning 8 sectors.
(Large files may be broken up into multiple parts, which will be listed in separate rows)
But what even is a sector? To summarize, it is section of the drive, typically 512 bytes.

You can `dd` to directly read that sector from the drive, use `findmnt /` to find it. 
(this won't work if the file is not on your root partition, if it is not, call findmnt with the mount point of the partition it is on)

```sh
dd if=[DEVICENAME] skip=[START SECTOR] bs=[SECTOR SIZE] count=[SECTOR COUNT IN FILE]
```

This, unsurprisingly, prints the content of the file:

```
This is some sample file data, it does not matter, as long as it is recognizable
8+0 records in
8+0 records out
4096 bytes (4.1 kB, 4.0 KiB) copied, 0.000114052 s, 35.9 MB/s
```

Ok, now lets try permanently deleting the file. (The sync command makes sure all changes to the filesystem get written on the disk, and don't stay around in cache)

```sh
rm test_file
sync
```

The file sure looks gone, but running dd reveals that the data is still on the drive:


```
This is some sample file data, it does not matter, as long as it is recognizable
8+0 records in
8+0 records out
4096 bytes (4.1 kB, 4.0 KiB) copied, 0.000114052 s, 35.9 MB/s
```

The `rm` command only removed the entry in filesystem for the file, not the actual file data.
Lets try the `shred` command (you will have to recreate the file, get it's location and update the `dd` command if you rm'ed it.)
The `shred` command claims to "Overwrite the specified FILE(s) repeatedly, in order to make it harder for even very expensive hardware probing to recover the data".

```sh
shred test_file
sync
```

The file is now full of random data, but the text is *still there* on the drive.
Wait, shred claims to overwrite the file, where did its data get written?
Running `hdparm` on the file, reveals that it is now stored in a new location.
*The data shred wrote, got written to a different part of the drive, leaving the original data intact*.

What about using dd to wipe the sectors of the file on the drive level? (this is dangerous, it can mess up your filesystem if you mistype the location and size)

```sh
sudo dd if=/dev/zero of=[DEVICENAME] skip=[START SECTOR] bs=[SECTOR SIZE] count=[SECTOR COUNT IN FILE]
```

The problem with this, is that as already demonstrated, the filesystem constantly moves files around.
You have removed the copy you found, but there are probably others.
What about the trick you will sometimes see recommended of creating a very big file until you run out of disk space?
This should overwrite the file right?


```sh
dd if=/dev/urandom of=spacefilling_file
syn
rm spacefilling_file
```

Wrong, while most of the free drive space is filled, some remains untouched (including the file!).
If the file that got deleted was large, only fragments might remain, but that is not ideal.

Some time later, after creating and working with large files, the file data is gone from the drive.
So simply waiting and using the computer can help, but I would not trust this.

Even if you do manage to delete all copies of a file, information about it can remain, in shell history file, thumbnail caches, recently opened lists and more.
The only real way to guarantee a files deletion is to wipe the drive it is on, and any computer you used to work on it.

**WARNING, THIS WILL DELETE ALL DATA ON THE DEVICE**

```sh
sudo dd if=/dev/urandom of=[DEVICENAME]
```

This might not even be enough, while as far as the OS is concerned, all the data is deleted, the drive may still have some data hidden.
Mechanical hard drives have bad sectors, which once detected are mapped out (instead of using that sector, a spare good sector is used).
While HDD's only sometimes do this, SSD's [constantly change where on the drive a given address is stored to avoid wearing out a given part of the SSD faster.](https://en.wikipedia.org/wiki/Wear_leveling)

To do this (and be able to tolerate failed SSD cells) A typical 1000 GB drive is actually a 1024 GB drive, with 24 GB reserved as "spare".
Any data that ends up in the spare cells (or failed), can't be directly deleted.
Fortunately, [both NVME and SATA provide a way to securely delete all data on a drive](https://wiki.archlinux.org/title/Solid_state_drive/Memory_cell_clearing) ,this will erase all of the drive, including spare cells.
Some drives (like many USB drives and SD cards) don't provide such a mechanism, or in cases where the [data is extremely sensitive](https://en.wikipedia.org/wiki/Data_remanence), the best option is physical destruction.
The typical method is to shred and incinerate the drive, so just blowtorch the flash chips or platters and break them up into multiple pieces before discarding.

