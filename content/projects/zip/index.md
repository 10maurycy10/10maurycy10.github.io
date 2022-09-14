---
title: "Making Zip Bombs"
date: 2022-09-12T14:59:44-07:00
tags: []
draft: true
---

The zip format is the most common compression format in existance.

A simple attack agains systems using zip is to create a file that decompresses to several TB to exhast disk space/ram on the target.

Lets try to make such a file.

# The zip format

The specification of the format can be found at (https://pkware.cachefly.net/webdocs/casestudies/APPNOTE.TXT).

From the specification

```
   4.3.6 Overall .ZIP file format:

      [local file header 1]
      [encryption header 1]
      [file data 1]
      [data descriptor 1]
      . 
      .
      .
      [local file header n]
      [encryption header n]
      [file data n]
      [data descriptor n]
      [archive decryption header] 
      [archive extra data record] 
      [central directory header 1]
      .
      .
      .
      [central directory header n]
      [zip64 end of central directory record]
      [zip64 end of central directory loca

   4.3.7  Local file header:

      local file header signature     4 bytes  (0x04034b50)
      version needed to extract       2 bytes
      general purpose bit flag        2 bytes
      compression method              2 bytes
      last mod file time              2 bytes
      last mod file date              2 bytes
      crc-32                          4 bytes
      compressed size                 4 bytes
      uncompressed size               4 bytes
      file name length                2 bytes
      extra field length              2 bytes

      file name (variable size)
      extra field (variable size)
```

It seems like this "Local file header" thing is important, so lets try creating one.

```
- Local file header
04 03 4b 50 - magic number
00 00 - Version number
00 00 - Bit flag
00 00 - Compression
00 00 - Last modifyed time
00 00 - Last modifyed data
00 00 00 00 - CRC 32
00 00 00 00 - Compressed size
00 00 00 00 - Uncompressed size
00 00 - File name length
00 00 - Extra length
[Somedata] - filename
[Extra] - extra
```

