---
title: "How helpful languages create bugs."
date: 2023-07-20T13:44:43-07:00
tags: ["programming"]
draft: false
---



A key part of programming is that you will spend more time looking at you code then the compiler.
While of course it is important for the computer to be able to figure out what to do, it is equally important for you to be able to tell what the computer is doing.

Take this function in C:

```c
float foo(float a, float b) {
	return a+b
}
```

Looking at this, you can immediately know what the computer will do when this function is called:
It will add two floating point numbers together and return the result

This is the same function writen in Python:

```py
def foo(a, b):
	return a + b
```

While this is a bit shorter, you no longer know what the computer is doing.
Is it doing floating point addition, or integer addition, or string concatenation?

Debugging is simply a process of comparing what the code is supposed to do, with what it actually does. 
If you dont know what the computer is actually doing, how are you supposed to know if it's doing the right thing?


Operator overloading results in a similar problem, consider this C++ code:

```c
Vector2f multiply(Vector2f b, Vector2f b) {
	return a * b
}
```

All the types of the arguments are specified, but this is still problematic;
What is the `*` operator doing here, piecewise multiplication or cross product?
When I ask people about this, half say "That's obvious, its' a cross product", and the other half say "That's obvious, it's piecewise multiplication".
Not only do people not know, but they don't know that they don't know.

A similar case occurs with function overloading.
If there is an add function that is overloaded to work on both ints and floats, what happens when you pass different datatypes?

```c
int a = 1
float b = 1.1

add(a, b) // What does this return?
```
Now it's unclear what precision the calculation is carried out with, that .1 *might* be silently dropped, or it *might* not.
This gets a lot worse when you combine multiple overloaded/generic functions together, while the computer might be able to figure it out, a human often can't.

While code in Python or JavaScript or C++ looks readable, it can often be literally impossible to know what code does before it is run (and it can even change at runtime):

```py
value = some_complex_function_that_involves_half_of_you_codebase()
# Is this repeating a string 2 times, or multiplying a number by 2?
value * 2
```

The readability of languages like C++, C#, Dart, Python, JavaScript, Kotlin, Scala, etc is an illusion. 
You might think the computer is doing one thing, when in reality it is doing something completely different.

In fact, the computer often also has trouble, C++ has notoriously slow compile times due to it's heavy use of these types of features.
On the internet, you will find people complaining that their projects that several minuets to recompile after changing a single line change.

If a computer takes several minutes to figure out what a single file in a program does, how is a human supposed to do it?

Garbage collection is an interesting one, becuase while it creates problems, the problem is not lack of readability, but lack of control;
You don't have any control over the garbage collector.
As the programmer, you might know that a particular heap allocated object is still in use, but that won't stop the garbage collector from checking every single one in your program (possibly millions or more).
This checking either adds massive delays proportional to how many objects are allocated, or a large slowdown every time a object is manipulated as the GC updates reference counts.

People say a garbage collector is good because it avoids memory leaks, but many tools, like valgrind exist to check if you forgot to deallocate something, or accessed freed memory, and tell you exactly where the mistake.
With these tools, debugging memory leaks take seconds, while working around garbage collection can take hours, and leave you code full of awful hacks.

## But why?

Why do language designers often make choices that make code impossible to read (or optimize)?

A big part of this is the emphasis placed on conciseness by language designers.
The goal is often to minimize how much code is needed.
By this metric, python is much better then c:

C:

```c
Vector2f foo(Vector2f a, Vector2f b) {
	return vector2f_cross(a, b);
}
```

Python:

```py
def foo(a, b):
	a * b
```

But in the process, the language makes the actual behavior of code ambiguous or at best very unclear.
Declaring variables, specifying types, explicitly calling function not only makes it easier for the computer to figure out what to do (compare the performance of C vs JS) but also for a human to figure out what the computer is doing.
However, people often assume that performance was the only reason for this, without considering readability (understandability might be a better term here).

There is a saying "Don't take down a fence until you know why it was put up", but perhaps a better saying would be "Don't take down a fence until you know all the reasons it was put up"

## Errors are good

Language designers often try to avoid giving programmers errors.
For example, JavaScript lets you access nonexistent properties of an object.
This has the side effect that code may breaking while still seaming to work (no error, no warning):

```js
scores = {};

scores["player1"] = 5;

// This does not update the player1 property, instead it creates a new Player1 property
scores["Player1"] += 1;
```

Languages like Python and JavaScript will also automatically convert types for you and automatically declare variables for you.

```js
var player1 = 1

// This does not update player1, but creates a new varable called player_1
player_1 = 2
```

When languages make you be explicit with defining variables, defining types, type conversions and error handling, they allow the compiler to catch when the programmer makes a mistake, catching bugs *before they happen*.
If variables are explicitly defined, the compiler will tell you if you misspell a variable.
If types are explicit, the compiler will tell you if you use the wrong type.

These do result in more typing for the programmer, as they have to specify the intended outcome of an operation, but this ensure that the code does what the programmer intended.
By doing this, the vast majority of bugs are caught before you can take you finger off the enter key, instead of spending hours tearing your hair out chasing an invisible bug.
