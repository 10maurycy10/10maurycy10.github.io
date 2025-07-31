---
title: "The joys of IPv4:"
date: 2025-07-31
tags: ["programming"]
---

I recently moved this blog over to it's own server, so now it has it's very own IP address: <a href=http://65.109.172.162/>65.109.172.162</a>

There are only around 2.3 billion IPv4 addresses, much less then the number of internet connected devices.
In most cases, multiple devices can share the same address using NAT, but this isn't possible for servers that must accept connections.

Instead, hosting providers reuse addresses:
My address was previously used by a Finnish online shopping company.

http://anvil.bhggroup.fi/

```txt
# drill anvil.bhggroup.fi

...

anvil.bhggroup.fi.      3600    IN      A       65.109.172.162

...
```

I'm not evil, but stuff like this could allow some random person steal authentication cookies or set up a very convincing phishing page.
HTTPS won't save you because lots of certificate authorities use plaintext HTTP to check domain ownership:
Just run certbot and have that little green lock ready in 5 seconds.

... it's not just web browsers that are vulnerable, if the domain was used by an auto-updater or device management tool,
an attacker could compromise the entire organization before anyone notices.

IP addresses are like domains:
Once you stop paying, they're up for grabs.
Remember to remove the DNS records of abandoned services.
