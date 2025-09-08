---
title: "No adblocker detected."
date: 2025-09-08
tags: ["programming"]
---

Internet ads are horrible: 
They waste your time, and the advertising industry makes the internet a worse place.
Payouts are so small that the only way to survive is to turn your site into an ad filled hellhole with no real substance.

If you want to support your favorite authors: send then money.
A dollar helps more then viewing ads ever would.

However, most people see advertising as a part of the internet experience, which is why I added this message to my site:

<style>
	ad-note {
                display: block;
		margin: auto;
                width: 14em;
                border: white 1px solid;
                background-color: #111111;
                padding: 1em;
        }
        ad-note-content-wrapper {
                margin-top: 0em;
                margin-bottom: 0em;
        }
	#hide {
		opacity: 0;
	}
</style>

<ad-note id=box>
<ad-note-content-wrapper>
No adblocker detected. Consider using an extension like <a href="https://ublockorigin.com/">uBlock Origin</a> to save time and bandwidth. <u>Click here to close.</u>
</ad-note-content-wrapper>
</ad-note>

It's shown off to the side, and never covers content.
It won't be shown if there isn't enough space.
The close button actually works and it stays closed.

The specific recommendation is important because a lot of people have only *heard of adblockers from ads*.
Commercial adblockers range from sketchy to outright scams:
If they are paying to be promoted, they must expect to make money from users. 

## Technical details:

The page itself contains a div to hold the message and tries to load a script called "nativeads.js":

```html
<!-- Rest of the page goes here -->

<script defer src="/nativeads.js"></script>

<div
  id="ad-note-hidden"
  class="ftf-dma-note ad native-ad native-ad-1 ytd-j yxd-j yxd-jd aff-content-col aff-inner-col aff-item-list ark-ad-message inplayer-ad inplayer_banners in_stream_banner trafficjunky-float-right dbanner preroll-blocker happy-inside-player blocker-notice blocker-overlay exo-horizontal ave-pl bottom-hor-block brs-block advboxemb wgAdBlockMessage glx-watermark-container overlay-advertising-new header-menu-bottom-ads rkads mdp-deblocker-wrapper amp-ad-inner imggif bloc-pub bloc-pub2 hor_banner aan_fake aan_fake__video-units rps_player_ads fints-block__row full-ave-pl full-bns-block vertbars video-brs player-bns-block wps-player__happy-inside gallery-bns-bl stream-item-widget adsbyrunactive happy-under-player adde_modal_detector adde_modal-overlay ninja-recommend-block aoa_overlay message"
>
  <p id="ad-note-content-wrapper">
  </p>
</div>
```

The script adds the actual message into the document:

```js
// /nativeads.js

function hide() {
        document.getElementById("ad-note").id = 'ad-note-hidden';
	document.getElementById("ad-note-content-wrapper").innerHTML = "";
        document.cookie = "notice-shown=true;path=/";
}

if (!document.cookie.includes("notice-shown")) {
	document.getElementById("ad-note-hidden").id = 'ad-note';
	document.getElementById("ad-note-content-wrapper").innerHTML = "No adblocker detected. " + 
	"Consider using an extention like <a href=https://ublockorigin.com/>uBlock Origin</a> to save time and bandwidth." +
	 " <u onclick=hide()>Click here to close.</u>";
}
```

Finally, there's a bit of CSS to make it look nice:


```css
#ad-note-hidden, #ad-note {
        display: none;
}

@media (min-height: 30em) { @media (min-width: 75em) {
        #ad-note {
                display: block;
                position: fixed;
                bottom: 1em;
                right: 1em;
                width: 14em;
                border: white 1px solid;
                background-color: #111111;
                padding: 1em;
        }
        #ad-note-content-wrapper {
                margin-top: 0em;
                margin-bottom: 0em;
        }
}}
```

The message won't be visible if an adblocker removes the &lt;div&gt; element -- which has a lot of ad-like class names -- or 
blocks the network request for "nativeads.js".

The network request ensures that it doesn't miss blockers like uBlock Origin Lite, which by default will only filter network requests, not page content.

Unfortunately, I have no way to detect DNS based blocking short of loading an actual ad. 
Instead, I made the message unobtrusive and easy to close.

The message won't be shown in browsers that don't support JavaScript, because those don't need adblockers to begin with.
It should be functional without CSS, but I can't think of any browsers that support JS but not CSS.
(but if you use external CSS, it's quite common for the request to fail resulting in an unstyled page.)

Thanks to [Stefan Bohacek](https://stefanbohacek.com/project/detect-missing-adblocker-wordpress-plugin/) for the original idea. I've modified it to reduce false positives and explicitly scoped the cookie so the message is only shown once.
