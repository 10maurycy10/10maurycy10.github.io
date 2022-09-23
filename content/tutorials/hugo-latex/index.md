---
title: "Adding KaTeX (JS based LaTeX rendering to hugo)"
date: 2022-09-23T09:42:11-07:00
math: true
tags: ["hugo", "latex"]
---

[KaTeX](katex.org) is a fast rendering library for LaTeX math, that runs in a web browser.

Including KaTeX simply requires adding some CSS and JS.

```html
<html>
<head>
<!-- TODO Check https://katex.org/docs/browser.html to verify this is the latest version -->
<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/katex@0.16.2/dist/katex.min.css" integrity="sha384-bYdxxUwYipFNohQlHt0bjN/LCpueqWz13HufFEV1SUatKs1cm4L6fFgCi1jT643X" crossorigin="anonymous">

<!-- The loading of KaTeX is deferred to speed up page rendering -->
<script defer src="https://cdn.jsdelivr.net/npm/katex@0.16.2/dist/katex.min.js" integrity="sha384-Qsn9KnoKISj6dI8g7p1HBlNpVx0I8p1SvlwOldgi3IorMle61nQy4zEahWYtljaz" crossorigin="anonymous"></script>

<!-- To automatically render math in text elements, include the auto-render extension: -->
<script defer src="https://cdn.jsdelivr.net/npm/katex@0.16.2/dist/contrib/auto-render.min.js" integrity="sha384-+VBxd3r6XgURycqtZ117nYw44OOcIax56Z4dCRWbxyPt0Koah1uHoK0o4+/RRE05" crossorigin="anonymous"
        onload="renderMathInElement(document.body);"></script>
...
</head>
<body>
	<p> Some content </p>
</body>
</html>
```

# Adding globally to hugo site

First create a new partial, ``katex.html`` in ``theme/[YourThemeName]/layouts/partials/katex.html`` with the following code:

```html
<!-- TODO Check https://katex.org/docs/browser.html to verify this is the latest version -->
<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/katex@0.16.2/dist/katex.min.css" integrity="sha384-bYdxxUwYipFNohQlHt0bjN/LCpueqWz13HufFEV1SUatKs1cm4L6fFgCi1jT643X" crossorigin="anonymous">

<!-- The loading of KaTeX is deferred to speed up page rendering -->
<script defer src="https://cdn.jsdelivr.net/npm/katex@0.16.2/dist/katex.min.js" integrity="sha384-Qsn9KnoKISj6dI8g7p1HBlNpVx0I8p1SvlwOldgi3IorMle61nQy4zEahWYtljaz" crossorigin="anonymous"></script>

<!-- To automatically render math in text elements, include the auto-render extension: -->
<script defer src="https://cdn.jsdelivr.net/npm/katex@0.16.2/dist/contrib/auto-render.min.js" integrity="sha384-+VBxd3r6XgURycqtZ117nYw44OOcIax56Z4dCRWbxyPt0Koah1uHoK0o4+/RRE05" crossorigin="anonymous"
        onload="renderMathInElement(document.body);"></script>
```

First edit the ``head.html`` partial, located in ``theme/[YourThemeName]/layouts/partials/head.html`` to include the following inside the ``<head>`` tag:

```html
{{ if .Params.math}}{{ partial "katex.html" . }}{{ end }}
```

It should look something like this:

```html
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="alternate" type="application/rss+xml" title="Post feed" href=/index.xml>
<link rel="stylesheet" href="/css/style.css">
{{ $title := print .Title " | " .Site.Title }}
{{ if .IsHome }}{{ $title = .Site.Title }}{{ end }}
{{ if .Params.math}}{{ partial "katex.html" . }}{{ end }} <!-- Newly added line -->
<title>{{ $title }}</title>
</head>
```

If ``theme/[themename]`` is a git submodule, you may have to fork/push to update your server.

# Using on a page

Now you can include KaTeX in a page by adding ``math: true`` to the front matter.

The default delimiters are ``$$`` for block math and  ``\\(Math\\)`` for inline math.

# Results

```md
---
math: true
---

This \\(x = {-b \pm \sqrt{b^2-4ac} \over 2a}\\) Math is inlined with text.

But this math is rendered seperatly:

$$ \sum^{n}_{i=0}{\frac{1}{2^i}}$$
```

## Produces

This \\(x = {-b \pm \sqrt{b^2-4ac} \over 2a}\\) Math is inlined with text.

But this math is rendered separately:

$$ \sum^{n}_{i=0}{\frac{1}{2^i}} $$

