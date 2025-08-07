---
title: "Messing with AI crawlers"
date: 2025-08-01
tags: ["programming", "bots"]
---

Large language models, the foundation of the recent wave of AI chatbots, need to be trained of vast quantities of text. 
The cheapest way for companies to get this text is scrape it off random websites using crawlers.

Ignoring the moral and legal aspects, these crawlers are just terribly behaved.
On larger websites, they can easily send hundreds of requests per second, 24/7:
Once they run out of pages, they just go back and redownload old content ad nauseam. 

It's a DDoS attack on the entire internet. 

I can't really do anything about all the crawlers, but I have a website, so I can do my part in making the web just a little bit more hostile towards them.

## Decompression bombs:

The initial plan was to send them gigabytes of HTML.
My hope was that this would waste enough memory or computing time that crawling my site wouldn't be worth it:

```html
<html>
<head>
<title>Just 20 GB of div tags.</title>
</head>
<body>
<div><div><div><div><div><div><div><div><div><div><div><div>
<!-- ...and a couple billion more -->
```

Once gzipped for transit, a file like this is just a few megabytes, but expands to tens of gigabytes: enough to crash most browsers and HTML parsing tools.
Here's a link if you want to try this yourself: <a href=/bomb/20gb rel=nofollow>/bomb/20gb</a>

To make sure the crawlers found it, I hid a few links to it on my blog, as well as blacklisting it in [/robots.txt](https://en.wikipedia.org/wiki/Robots.txt) so GoogleBot and friends would be unaffected.

Unfortunately, the flow of bots seemed to be unaffected. 
I'm guessing that they either check for these types of tricks, or that the 1000 fold advantage from gzip just isn't enough to cause any problems for billion dollar companies. 

## Tarpits:

So, what if we give the scrapers what they want: content
... randomly generated content.

I decided to use a *Markov Chain*, where a corpus of text is broken down into overlapping word pairs, which are then randomly strung together into new text.
Here's what what this algorithm produces after being fed a [recent blog post](/misc/cc):

```txt
Here’s what colors are very good at.  Right infinite abyss of overlap between
the white balance is space nothing is problematic.  It to so the camera’s sensor
look like no way to the others are supposed to the brightness of cone which
corresponds to fix this nebula. The same colors red yielding this red green
and blue light entering your eye. The world: very complex. This description
is the camera’s filters which would be wrong... and blue at all the red green
stars which corresponds to be seen by my camera’s filters won’t always be
```

> <center>"The world: very complex": AI WILL REWRITE ALL HUMAN KNOWLEDGE!</center>
> -- every crackpot, everywhere

The output tends to be syntactical correct, but semantically meaningless:
perfect for ruining training data.

I set up a page with two paragraphs of garbage and five links to more garbage.
This should cause the crawler's queue would fill up with an exponentially increasing amount of trash, until it had no resources left to crawl my real site.

Here's one of the pages if you want to check it out: <a href=/babble/entry-point rel=nofollow>/babble/</a>

<div style="border: 1px solid; border-color: yellow; padding: 0.5em">
If you want to do this, I urge you to write your own implementation. 
The more different everyone's garbage is, the harder it will be to filter out.
<br/>
<br/>
That being said, here's the source code for <a href=/projects/trap_bots/>my version.</a>
</div>

Once again, I hid a few links around my blog and waited.

## The bots:

I saw two bots that identified themselves.
The best known is GPTBot, from [ChatGPT](https://platform.openai.com/docs/bots):

```txt
Mozilla/5.0 AppleWebKit/537.36 (KHTML, like Gecko; compatible; GPTBot/1.2; +https://openai.com/gptbot)
```

Another bot in the honest-but-ignores-robots.txt category is Thinkbot:

```txt
Mozilla/5.0 (compatible; Thinkbot/0.5.8; +In_the_test_phase,_if_the_Thinkbot_brings_you_trouble,_please_block_its_IP_address._Thank_you.)
```

However, despite the usage agent string, it rotates though 75 IP addresses, so blocking by address would be quite annoying.

I also saw a few dishonest but obvious crawlers, spamming my server with requests from a single IP address.
There was one crawler from a residential address pretending to be Firefox (2 requests per second, always sent HEAD requests):

```txt
Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:129.0) Gecko/20100101 Firefox/129.0
```

I also saw 43.134.189.59 hitting my server with 20 requests/second while pretending to be Chrome:

```txt
Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/547.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36
```

The most interesting thing I found was a network of stealthy bots that pretend to be browsers.
Here's a sample of the IP addresses they used:

```txt
170.150.246.83   
170.79.230.64    
170.83.29.24     
177.106.195.210  
177.124.107.1    
177.172.98.41    
177.33.96.60     
177.67.38.55     
177.92.31.66     
179.124.182.172  
179.97.27.235    
181.235.187.139  
186.116.198.158  
186.13.225.45    
186.137.122.204  
186.18.15.67     
186.193.61.124   
186.211.149.8    
186.232.44.66    
187.180.212.170  
189.34.198.142   
189.82.27.130    
189.90.121.172   
190.89.110.25    
191.7.148.202    
200.52.28.229    
201.159.144.174  
201.254.222.66   
201.27.107.80    
216.234.209.246  
222.253.91.118   
41.212.22.193    
4.227.36.96      
43.130.12.43
... and many, many more.

2001:ee0:464a:d23d:bccf:fbbe:2e26:a83e
2001:ee0:46d6:adef:d898:378c:d40b:32f4
2001:ee0:4908:ef44:e9b1:fcc8:f888:1580
2001:ee0:4b6c:b6be:9d1d:d79e:9558:869e
2001:ee0:4f3d:2a2b:b455:5359:57f3:3fc1
2001:ee0:4f7c:c347:bda8:444f:7f1b:d3fb
2001:ee0:4f7c:f1cc:8536:8311:7fe6:d895
2001:ee0:51c4:bbf1:192e:6e25:4c19:e819
2001:ee0:5384:b03f:2989:384e:c02e:d18e
```

Here's some of the user agent strings they use:

```txt
Mozilla/5.0 (iPad; CPU OS 17_0_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.0.1 Mobile/15E148 Safari/604.1
Mozilla/5.0 (iPhone; CPU iPhone OS 13_2_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.3 Mobile/15E148 Safari/604.1
Mozilla/5.0 (iPhone; CPU iPhone OS 17_2 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) EdgiOS/121.0.2277.107 Version/17.0 Mobile/15E148 Safari/604.1
Mozilla/5.0 (Linux; Android 10; K) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Mobile Safari/537.36
Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/121.0.0.0 Safari/537.36 Edg/121.0.0.0
Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/121.0.0.0 Safari/537.36 Edg/121.0.0.0 Herring/95.1.1930.31
Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/121.0.0.0 Safari/537.36 Edg/121.0.0.0 Trailer/93.3.3695.30
Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/121.0.0.0 Safari/537.36 Edg/121.0.0.0 Viewer/99.9.9009.89
Mozilla/5.0 (X11; Linux x86_64; rv:123.0) Gecko/20100101 Firefox/123.0
```

These bots cycle though residential IPs, and each IP is only used for a single request.
Looking at the logs, they are quite obvious because they hit "deep" URLs from brand new addresses, which are never seen again.

... very different from a human clicking links. 

However, detecting them in real time is quite difficult (at least without annoying users by serving challenges instead of content)
because by the time you've can tell it's a bot, it's too late to ban it.
