---
title: "You can make up HTML tags:"
date: 2025-08-02
tags: ['programming']
---

Instead of writing HTML like this:

```html
<div class=cool-thing>
Hello, World!
</div>
```

... you can write HTML like this:

```html
<cool-thing>
Hello, World!
</cool-thing>
```

<!--more-->

... and CSS like this:

```css
cool-thing {
	display: block;
	font-weight: bold;
	text-align: center;
	filter: drop-shadow(0 0 0.5em #ff0);
	color: #ff0;
}
```

<style>
cool-thing {
	display: block;
	font-weight: bold;
	text-align: center;
	filter: drop-shadow(0 0 0.5em #ff0);
	color: #ff0;
}
</style>

<cool-thing>
Hello, World!
</cool-thing>

<div style="display: none">
For those using RSS, click <a href=https://maurycyz.com/misc/make-up-tags/>here</a> to see it in action.
</div>

Browsers handle unrecognized tags by treating them as a generic element, with no effect beyond what's specified in the CSS.
This isn't just a weird quirk, but is [standardized behavior](https://html.spec.whatwg.org/multipage/dom.html#htmlunknownelement).
If you include hyphens in the name, you can guarantee that your tag won't appear in any future versions of HTML.

While you should use descriptive built-in tags if they exist, if it's a choice between &lt;div&gt; and &lt;span&gt;, 
making up your own tag provides better readability then using a a bunch of class names.

As an example, if you have a bunch of nested tags:

```html
<div class=article>
<div class=article-header>
<div class=article-quote>
<div class=quote-body>
... a bunch more HTML ...
</div>
</div>
</div>
</div>
```

Good luck trying to insert something inside of "article-heading" but after "article-quote" on the first try.
This problem vanishes if yu use descriptive tag names --- no &lt;/div&gt; counting required:

```html
<main-article>
<article-header>
<article-quote>
<quote-body>
... a bunch more HTML ...
</quote-body>
</article-quote>
<!-- here! -->
</article-header>
</main-article>
````
