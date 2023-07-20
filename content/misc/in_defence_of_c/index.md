---
title: "In defense of C or \"Why you spend 5 hours debugging only to find a typo\""
date: 2023-07-19T15:12:39-07:00
tags: ["programming", "rants"]
draft: false
---

C is a language that gets a lot of hate, for having very little features, manual memory management, and generally a lot of footguns.
However, In C, I end up spending much *less* time debugging code then any other language.
It's not just me, with the TIOBE index ranking C consistently within the top 2 most popular languages for the past 23 years.

So just why is C, a language that is more then 50 years old, still so popular?

Perhaps a better question would be, why do all the languages that try to be better then C fail?
New languages often try to be concise, allowing you to do the most possible with the least code.
Garbage collection allows you to not worry about managing memory, dynamic typing lets you avoid declaring types, type coercion lets you forget about type conversions, operator overloading lets you work with complex data easily.
However, these features often create ambiguity and hard to spot bugs.

<!--
For instance, consider garbage collection, a highly prized feature amount languages like Java and Python, notably absent in C.
It's a great feature early in development, letting you allocate all the memory you want without having to worry about freeing it when your done.
However, later, when optimizing the code, garbage collection adds constant, non negotiable overhead to almost every operation.

Ever notice how Java games drop frames, no mater how fast your computer is?
That lag is the garbage collector, checking every single object in memory to see if it is still needed.
Some languages continually garbage collect, but that results in almost every operation invoking the garbage collector, slowing down the program.
Manually freeing memory adds almost zero overhead, as you can just free memory when your done with it instead of constantly checking.
Garbage collection doesn't typical introduce bugs, but it requires you to code very carefully to avoid massive performance penalties (multiple second lags are not uncommon).
-->

A common helpful feature is function overloading, allowing you to use the same function for multiple types.

Consider an `add` function, that is overloaded for both floats and ints. 
What happens when you add a float to an int? Is the result a float, or a int?

```c
float a = 0.1
int b = 1

add(a, b) // Does this return 1.1 or 1?
```
What about `add(b, a)`? does that do something different?
A function like `addf(a, b)` or `addi()` is much clearer, is it realy so much harder to type?

A related helpful feature is operator overloading, allowing you to do things like add vectors with `+` as if they were normal numbers.
But this can easly create ambiguity:

```c
vec2 a = vector2(1, 2);
vec2 b = vector2(3, 4);

a * b
```

Is this piecewise multiplication or a cross product?

With a function like `vec2_mul_cross(a, b)` or `vec2_mul_piecewise(a, b)`, it is immediately clear, and obvious if you used the wrong one.
In the process of saving a handful of key presses, it has potentially created an extremely hard to spot bug.


Dynamic typing is another helpful feature, that can create ambiguity and hard to spot bugs.

For example, in python:
```py
scores = {}

scores[1] = 1
scores["1"] = 2
scores[1] # This is still 1
```

This happened to me once, one part of a games code was using integers as player ids, and another was using strings representing them.
I wasted a whole day trying to find that bug.

JavaScript has this helpful feature, when you try to access a nonexistent member of an object, it will create it for you.
This means that if you try to increment a property, and misspell it, JS will just ignore it:

```js
scores = {}

scores.player1 = 5

// This looks like it increment player1's score, but it doesn't
scores.player_1 += 1
```

Again, I have spent hours looking for bugs that have resulted from trivial typos like this.
A less helpful language, like C would throw an error when you try to access the property.

Another helpful feature of JS and Python is that you don't have to declare variable, but again, this can turn small typos into hard to find bugs:

```py
fill_color = "white"

# This does not result in an error, but simply creates a new varable called fill_colour.
fill_colour = "black"
```

Again, countless hours of my (and doubtless many other's) time have been spent looking for a error that a less helpful compiler would find instantly.

Lots of languages will helpfully convert types for you, but this can again lead to hard to spot bugs.
For exmaple, in JavaScript:
```js
var a = "2"
var b = 1

a+b // This is actualy "21", not 3
b+a // and this is "12"
```

This is easy to see here, but imagine if the values comes from some other part of a large codebase.
This would result in a bug that only happens under very specific conditions, where as less helpful language would refuse to compile the moment you mix up your types.

JavaScript gets a lot of hate for this, but other languages are not much better.
Accidentally loosing precision by passing an float to a function that takes an it, or storing a double in a float is super common, and hard to debug.
A strongly typed language will throw an error instead, and C will give a warning, but most languages will compile no questions asked.

The real issue here is by reducing the code required, a language reduces the ability to detect errors, both run time and compile time.
Errors are good. When a compiler makes you be more specific, it ensures that the computer is doing what you think it is doing.
Having to fix a few compile errors is far far better then spending hours (or days) tracking down bugs.

You need to be able to know what the computer is doing when you read code.
Any ambiguity between what you see and what you read and what the computer does results in hard to debug bugs.
Is that `+` intiger additon? vector math? string concatination? Is `*` a cross product or peicewize multiplication?

In short: A language forcing you to be explicit is good.

While the C language is not perfect, the disadvantages are mininmal; 
The typical segfault is resolved in just a few seconds by using a debugger, which will imediatly tell you where it happened.
Memory leaks are also fairly easy with the right tools, like valgrind.
(C has one of best devoloped ecosystem of debugging tools of any langauge)

Some functions in C are rather horrible, such as (sn)printf, exactly because they don't catch errors.
But overall, C actually results in less hard to find bugs then other langauges, exactly because it is unhelpful.


