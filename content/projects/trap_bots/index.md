---
title: "Trap bots on your server:"
date: 2025-08-05
tags: ["programming"]
---

Here's the code for my [infinite nonsense](https://maurycyz.com/babble/entry_point) crawler trap:<br>

- [process.py](/projects/trap_bots/process.py): Text preprocessor
- [babble.c](/projects/trap_bots/babble.c): Garbage server

What follows is an explanation of how to set it up...
<!-- more -->

## Training the Markov chain:

First, you'll want to find three long-ish sources of text, between 1000 and 50,000 words. 
I used ebooks from [Project Guttenberg](https://www.gutenberg.org/), but long blog posts or Wikipedia articles will also work. 

Save the text in files named "`book1.txt`", "`book2.txt`" and "`book3.txt`".
Remove any page numbers, headings, copyright notices and similar.

If your texts are somewhat short (less then 3000 words), change the "`context_length`" variable in process.py to 1. 
This results in less repetitive text, but for longer bodies of text, setting it to "2" produces more coherent results.

With the text prepared, run the [python script](/projects/trap_bots/process.py):

```sh
$ python3 process.py
```

You should now have 3 "`chainX.txt`" files full of framented text. 

## The server:

First, configure your web server to forward all requests under an unused path to `localhost:[some unused port]`.
Here's what that looks like using NGINX:

```py
# Proxy traffic to the Markov babbler
# Place inside a server { } block
location /babble/ {
	proxy_pass http://localhost:1414/;
}
```

Next, open the [C code](/projects/trap_bots/babble.c) and change "`PORT`" to the chosen port number and "`URL_PREFIX`" to the path that your server is forwarding:

```c
...
#define PORT 1414
...
// Must begin and end with "/"s
const char* URL_PREFIX = "/babble/"
...
```

With the configuration done, compile the garbage server:

```sh
$ gcc -O3 -lm -o babble babble.c
```

... and run it in the same directory as the "`chainX.txt`" files:

```sh
$ ./babble
[*] Loading files
    Loading chain1.txt...
    Loading chain2.txt...
    Loading chain3.txt...
[*] Creating socket
[*] Serving garbage!
```

It may take a few seconds to parse the files if they are particularly big.
Once finished, you should be able to visit a URL under the configured path and get garbage:

```txt
https://your.domain.example.com/babble/test
```
<center style='color:gray'>Make sure to test the links</center>

If you see a 5XX (500, 502...) error, make sure you set the right path and port number in your web server's configuration.
If the page loads but the links are broken, check the "`URL_PREFIX`" and recompile the code.

If everything works, you'll want the program to run in the background.
With systemd on Linux, create a file called:

```txt
/etc/systemd/system/babbler.service
```

... with the following content: 
(don't forget to change the file paths)

```ini
[Unit]
Description=Markov chain based crawler trap
After=network.target
StartLimitIntervalSec=0

[Service]
Type=simple
Restart=always
RestartSec=1
User=nobody
WorkingDirectory=/path/to/markov/chains/
ExecStart=/path/to/binary/babble

[Install]
WantedBy=multi-user.target
```

... and run these commands as root:

```sh
$ systemctl daemon-reload
$ systemctl start babble
$ systemctl enable babble
```

If you run some other init system, you probobly know enough to figure it out youself.

## Catching wild bots:

You don't really need any bot detection:
just linking to the garbage from your main website will do.
Because each page links to five more garbage pages, the crawler's queue will quickly fill up with an exponential amount of garbage until it has no time left to crawl your real site.

If you don't want your site to disappear from search results, exclude the garbage URLs in `/robots.txt`:

```txt
User-agent: Googlebot Bingbot Kagibot
Disallow: /babble/*
```

If you want prevent bots from ever seeing any of your content,
[ai.robots.txt](https://github.com/ai-robots-txt/ai.robots.txt/tree/main) has nice lists of User-Agents. 
Just change the rules to rewrite or redirect instead of blocking.

Here's what that looks like NGINX:

```py
# Send scrapers garbage
# Regex from https://github.com/ai-robots-txt/ai.robots.txt/blob/main/nginx-block-ai-bots.conf
if ($http_user_agent ~* "(AddSearchBot|AI2Bot|Ai2Bot\-Dolma|aiHitBot|Amazonbot|Andibot|anthropic\-ai|Applebot|Applebot\-Extended|Awario|bedrockbot|bigsur\.ai|Brightbot\ 1\.0|Bytespider|CCBot|ChatGPT\ Agent|ChatGPT\-User|Claude\-SearchBot|Claude\-User|Claude\-Web|ClaudeBot|CloudVertexBot|cohere\-ai|cohere\-training\-data\-crawler|Cotoyogi|Crawlspace|Datenbank\ Crawler|Devin|Diffbot|DuckAssistBot|Echobot\ Bot|EchoboxBot|FacebookBot|facebookexternalhit|Factset_spyderbot|FirecrawlAgent|FriendlyCrawler|Gemini\-Deep\-Research|Google\-CloudVertexBot|Google\-Extended|GoogleAgent\-Mariner|GoogleOther|GoogleOther\-Image|GoogleOther\-Video|GPTBot|iaskspider/2\.0|ICC\-Crawler|ImagesiftBot|img2dataset|ISSCyberRiskCrawler|Kangaroo\ Bot|LinerBot|meta\-externalagent|Meta\-ExternalAgent|meta\-externalfetcher|Meta\-ExternalFetcher|MistralAI\-User|MistralAI\-User/1\.0|MyCentralAIScraperBot|netEstate\ Imprint\ Crawler|NovaAct|OAI\-SearchBot|omgili|omgilibot|Operator|PanguBot|Panscient|panscient\.com|Perplexity\-User|PerplexityBot|PetalBot|PhindBot|Poseidon\ Research\ Crawler|QualifiedBot|QuillBot|quillbot\.com|SBIntuitionsBot|Scrapy|SemrushBot\-OCOB|SemrushBot\-SWA|Sidetrade\ indexer\ bot|Thinkbot|TikTokSpider|Timpibot|VelenPublicWebCrawler|WARDBot|Webzio\-Extended|wpbot|YaK|YandexAdditional|YandexAdditionalBot|YouBot)") {
	rewrite ^(.*)$ /babble/$1;
}
# Also send garbage to firefox's AI summaries
if ($http_x_firefox_ai ~* "1") {
	rewrite ^(.*)$ /babble/$1;
}
```

This will also ensure that AI chatbots and summarizers only ever get garbage...
after all, the easiest thing to turn into garbage is garbage -- *See, we're just helping!*

Just beware that there is a significant amount of scraper using residential IP and browser User-Agents,
so I'd recommend hiding a link even if you filter by User-Agents.

## The cost:

On my machine, generating a page takes anywhere from 50-200 thread micro-seconds and the server uses 100k to 5 MB of RAM depending on configuration. 
Unless you are using a very slow server, CPU load will be minimal even when hit with hundreds of requests per second. 
Each response weighs around 1 kB gzipped, so bandwidth shoundn't be a problem.

Just be careful if you're behind any cloud service that bills by request.
