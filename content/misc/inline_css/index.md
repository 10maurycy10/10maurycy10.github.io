---
title: "Don't use external CSS:"
date: 2025-07-23
tags: ["programming"]
draft: false
---

At my house, an HTTP request over cellular takes 2 seconds. 
Even if the response is just a single byte, it still takes two seconds.
Browsers won't render anything until the stylesheet is loaded, so changing:

```html
<title>Some random blog post</title>
<!-- Load CSS from external URL -->
<link rel="stylesheet" href="css/style.css">
```

to:

```html
<title>Some random blog post</title>
<!-- Here you go! -->
<style>
	body {
		background-color: black;
		color: white;
	}
</style>
```

... shaves seconds off the time it takes the page to load.

CSS isn't heavy: the styling for this page weighs around 500 bytes compressed.
Loading 500 bytes is practically instant on any modern network, but a request isn't. 

## Why are requests so slow?

Establishing a TCP connection takes one round trip.
Setting up encryption with TLS takes two additional round trips. 
Next, the browser sends the actual HTTP request, but receiving a response takes least one additional round trip.

In total, even the smallest request takes 4 round trips. 

Even with fairly good 50 ms ping times, that's still 0.2 seconds, a noticeable delay.
Cell networks in rural and crowded areas can be much slower.
I've seen multi-second ping times on otherwise usable connections ---
Some people will be waiting an extra 10 seconds just for a stylesheet.

Four round trips isn't universal, it's possible for browsers to reuse an old connection for a new request.
In this case, embedding CSS only saves one round trip, but that's still be noticeable.

Connection reuse can only happen if...

- The main page is small enough to load in one round trip. (~14 kB of compressed data. This one is 4.3 kB, and that includes junk like the missing adblocker warning message.)
- Only one resource is needed to render the page.
- That resource is on the same domain as the page.

A lot of sites like Substack and Reddit use separate domains for CSS, which prevents connection reuse and also triggers an additional DNS request:

|CSS configuration|Round trips|Time<br>(50 ms ping)|Time<br>(700 ms ping)|
|-|-|-|-|
|Embedded|4|0.2 s|2.8 s|
|External (Keep-alive)|5|0.25 s| 3.5 s|
|External|8|0.4 s | 5.6 s|
|External (Different domain)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|9|0.45 s | 6.3 s|

The same thing applies to critical Javascript. 
If the script has to run in order for the user to see anything, put it in the main request. 
If that's not possible, at least put a short preview of the content in there. 
