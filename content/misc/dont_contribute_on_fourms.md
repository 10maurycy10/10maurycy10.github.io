---
title: "Please don't contribute anything of value on web forums"
date: 2023-01-27T15:22:30-08:00
tags: []
draft: true
---

Nothing of value should solely be posted on web forums like Reddit, HN, Facebook, Discord, Slashdot, Instagram or any similar site.
Web forums typical have short lifespans, few have lasted longer than 10 years.
Popularity is also no guarantee of continued service, [hundreds of well known and used services have been shut down by large companies like google, with all data lost](https://killedbygoogle.com/).

TLDR: Web forums are short lived with no backups or archives available, they have complete control over your content and your readers. Any data on such sites is eventually lost. Avoid web forums, instead, get your own website.

# Issue: No backups

When a web forum shuts down, the data is often forever left inaccessible.
Web forum owners very rarely release archives or data dumps, meaning that if a user has not archived (and published elsewhere) all of the content, it is forever lost.
Most forums have no public facing API that could allow a backup or archive to be created, so even *if* a user wants to, and has the resources to, preserve the information, they have no viable way to do so.
Even web crawlers like the ones used by [archive.org](https://archive.org) are often unable to archive web forums.
For example, the Internet Archive doesn't archive Discord threads, nor Facebook pages.

Backing up a simple website is super easy, nearly every site on the internet being backed up on archive.org.

# Issue: No replication

Even *if* a backup does exist, most of social media sites are fully closed source, with no third party clients, making it impossible to view the backup as anything else than a database dump.
If you managed to get an data dump of Reddit, all you would have several terabytes of unformated, unorganized, unsearchable data.
Creating a server to handle that data, and a frontend to present it would be a massive project, beyond the reach of most programmers. (Hosting would also be problematic)

Most websites however, use an open source static site generator, or can simply be hosted on a standard web server, making replication easy.

# Issue: Censorship and rule dictation

Even if the forum survives, your content might not.
YouTube has been known to automatically remove videos it thinks might violate the community guidelines, often removing the entire channel because of 1 too many "infractions".
Lots of creators have had there entire channel deleted due to a decision made by an automated system, with no way to appeal.
The same holds for any other social media sites.

Because there is no easy way to create a backup of your data, moving to [another platform is very difficult.](https://en.wikipedia.org/wiki/Switching_barriers)
Some platforms go the extra step and prevent you from linking to competing platforms. (Twitter flags links to the compateing and superior Mastodon as malware)

No such issue exists with a personal website, It is very easy to swap hosting providers (which tend to have very lax rules and no automatic enforcement).
Often, you don't even have to change the URL.

# Issue: UI dictation

The owner of social media sites has complete control over how the site looks or behaves.
They can place celebrity gossip next to your content, or promote trolls in the comment section.
They can require an account to view *your* content, or they can ask for money from the viewer, or *you*.

Social media is not typically very accessible to people with impaired vision, use screen readers, or Braille displays.
According to the WHO, over 285 million people have some form of vision disability, and have difficulty with such sites.

All of these problems are avoided with a personal website, which can be designed as you wish, and are typically very accessible.

# Example: Discord

Many projects and games are moving to Discord as the only avalable platform for support and documenation.
Discord has all of the listed problems and more.
There is no way to backup a discord server, and even if a backup is created, there is no way to replicate the Discord UI.
They has been known to delete entire servers with no notice or appeal.
Compromized bots or admins have also done this, with no way to roll back the server.
Servers are not indexed by search engines, and require an account to view, and not archived by archive.org.
Overall, information posted to Discord is hard to find, hard to archive, and may be deleted at any time.

# The alternative

The alternative to web forums is a simple web site.
These are often easier to find (search engines work better), use, and archive.
[This site is on archive.org](https://web.archive.org/web/*/https://10maurycy10.github.io/), meaning that even if I leave and take my copy with me, and GitHub deletes the site, it will stay accessible.
It is also trivial for *anyone* to create a replicable copy, just one command is required ([alternatively, you can download a zip over HTTPS](https://github.com/10maurycy10/10maurycy10.github.io/archive/refs/heads/master.zip)):

```sh
git clone https://github.com/10maurycy10/10maurycy10.github.io.git
```

Replication is just as simple:

```sh
hugo server # If you run `hugo`, it will generate html in public/
```

There is also very little switching costs with such sites, if GitHub decides to cut service, I can just move all the data somewhere else.

You might think that is hard to get viewers/readers on a website, but all it takes is to link it on social media, but dont make this the only way of getting updates. (RSS is super easy to setup.)

# Some dead forums, with no archive/backup released.

(This list took just a few minuets to compile)

- https://theradioboard.com/
- https://forum.duolingo.com/
- https://forums.gearboxsoftware.com/
- http://hamsterhideout.com/forum/
- https://forum.supercell.com/
- http://myce.com/
- http://cdfreaks.com/
- https://bethesda.net/community/
- https://zyngaplayerforums.com/farmville/
- https://community.imgur.com/
- http://www.offtopicz.com/
- https://myspace.com/ (Still online, but lost all old data)
- http://geocities.com/



