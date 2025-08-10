---
title: "Optimized cyanotypes:"
date: 2025-08-08
tags: ["chemistry", "photography"]
---

As far as I'm aware, this is the most sensitive cyanotype formula on the internet, and is getting into the range of practicality for in-camera photography: <!-- more -->

|Sensitizer:|(by mass)|
|-|-|
|&nbsp;0.05|Ferric ammonium citrate&nbsp;|
|&nbsp;0.1|Oxalic acid|
|&nbsp;0.85|Water|

<br>

|Developer:|(by mass)|
|-|-|
|&nbsp;0.05|Potassium Ferricyanide&nbsp;|
|&nbsp;0.01|Citric acid|
|&nbsp;0.94|Water|

The sensitizer solution must be protected from blue and UV light.
The developer very slightly light sensitive, but realistically, it should be fine.


## Procedure:

<style>
li {margin-top: 0.5em;}
li::marker {font-weight: bold;}
</style>

1. Spread the sensitizer on the paper. It doesn't take much, just slightly wet the surface.
I find spreading with a glass rod works better then brushing it on.
2. Let the paper dry in a dark area.
3. Expose the paper.
4. Apply the developer solution. No finesse required: just pour it on.
5. Wash the print with water for a minute or so to remove the unreacted chemicals. Even an invisible amount of residue can fog the image.

The paper should be protected from stay light during the process. 
The developer solution can be reused multiple times: apply it liberally and collect the excess. 

## Results:

![Exposure test strips](/projects/fast_cyanotype/test_strips.jpg)
<center style="color: gray">Left: Classic (5 seconds/tick). Right: Mine (1 second/tick)</center>

My version is around 5 times as sensitive, and has well preserved highlights, allowing it to achieve compatible results in 1/20th the time of the classic formula:
enough to turn what would be a 3 hour exposure into a 10 minute exposure.

Using sunlight, a good exposure is between 100 kilolux seconds and 1000 kilolux seconds, and the effective ISO is around 0.0001. 
(The original method has an ISO of around 0.000005)

It doesn't get as dark as the [classic formula](/projects/take_a_photo/#ironcyanotype), maxing out at the dark blue as shown in the image.
This can actually an advantage for photography because it keeps the contrast manageable:
The original formula tends to have very dark shadows, bright highlights and little in the way of midtones. 

## Design notes:

The standard iron-ferricyanide/cyanotype formula has a number of problems:

1. The reaction is self limiting.

Because the pigment is formed during the exposure, it blocks light and slows down the reaction.
The result is that it needs an exposure that's much longer then it needs to be.

2. Pigment washout.

A lot of pigment gets lost during washing.
Even though they are insoluble, small particles can get suspended in water and carried away ---
resulting in missing highlights at best and the entire image disappearing at worst.

3. Limited paper compatibility.

Alkaline buffered paper just doesn't work.
The base effect the photochemistry itself, leading to a blotchy appearance and also bleaches the pigment over time. 

4. Poor QE.

The final problem is that citrate really isn't a good electron donor for photo-reduction. 
Of all the carboxillic acids, iron (III) oxalate is best at responding to light.
The reaction is also pH sensitive, and works best in an acidic environment, something that isn't present in the classic formula.

[1] can be fixed by using a two step process, where the iron (III) salt is applied to paper, exposed and only then treated with ferricyanide.
For [4], ferric ammonium oxalate is available, but it's easier to just add oxalic acid to ferric ammonium citrate.
The excess acid also takes care of the pH issue.

As a bonus, the oxalic acid also takes care of [2] because it results in larger pigment crystals and [3] because it neutralizes any buffers that may be present.

Iron (III) oxalate based formulas tend to leave a yellow stain composed of Iron (II) oxalate on the paper, which can be dissolved in citric acid.
Doing this during development also allows the otherwise trapped iron to contribute to image formation.

## Testing notes:

Slowest to fastest:

- Classic [18% of max @ 25s in sun]
- Mike Ware's "New Cyanotype"
- 2-Step classic "Cyanotype Rex"
- 2-Step: Ferric ammonium citrate + citric acid
- Blue sheet: Classic with ferr**o**cyanide
- 2-Step blue sheet: Ferr**o**cyanide developer.
- 2-Step: Ferric ammonium citrate + oxalic acid [18% of max @ 1s in sun]

I did not test Mike Ware's "New Cyanotype", because I don't have ferric ammonium oxalate, and don't want to play with dichromate.
[This test](https://slyka.net/blog/2022/cyanotype-comparison/) puts it between classic and two step.

## Formula reference:

|Classic||
|-|-|
|&nbsp;5|Ferric ammonium citrate|
|&nbsp;2|Potassium ferricyanide|
|&nbsp;30|Water|

This is similar to Herschel's original, but with a different ratio of citrate to ferricyanide. 
Probobly the most common contemporary mixture.

<hr>

|New Cyanotype (Mike Ware)||
|-|-|
|&nbsp;30|Ferric ammonium oxalate|
|&nbsp;10|Potassium ferricyanide|
|&nbsp;0.1|Ammonium dichromate (optional)|
|&nbsp;100|Water|

**Note:** A concentrated solution should be prepared, which will form crystals of Ferric potassium oxalate. 
These need to be discarded, and then the remaining liquid is diluted before using.

<hr>

|Blue sheet||
|-|-|
|&nbsp;5|Ferric ammonium citrate|
|&nbsp;2|Potassium ferrocyanide|
|&nbsp;30|Water|

The sensitized paper is blue due to the lack of the intense yellow of ferricyanide and the presence of trace Prussian blue.
Similar mixtures are commonly used in commercial blue printing. 

The main product is the reduced form, Prussian white, so the print must be oxidized with hydrogen peroxide before viewing.

<hr>

|Blue sheet 2-step|Sensitizer|
|-|-|
|5|Ferric ammonium citrate|
|50|Water|

|Blue sheet 2-step|Developer|
|-|-|
|5|Potassium ferrocyanide|
|50|Water|

The main product is the reduced form, Prussian white, so the print must be oxidized with hydrogen peroxide before viewing.
This formula produces a slightly fogged result. 

<hr>

|2-step Classic "Rex"|Sensitizer|
|-|-|
|5|Ferric ammonium citrate|
|30|Water|

|2-step Classic "Rex"|Developer|
|-|-|
|2|Potassium ferricyanide|
|30|Water|

No frills, (and least sensitive) two-step process. 
Popularized by [hands-on-pictures.com](https://web.archive.org/web/20091218074543/https://www.hands-on-pictures.com/Tutorials/Cyanotype.html)

<hr>

|2-step Acidic|Sensitizer|
|-|-|
|5|Ferric ammonium citrate|
|10|Citric acid|
|95|Water|

|2-step Acidic|Developer|
|-|-|
|5|Potassium ferricyanide|
|1|Citric acid|
|94|Water|

Acidified two-step process: more sensitive then the standard two-step.

This is a usable alternative if you don't have oxalic acid.

<hr>

|2-step Oxalated|Sensitizer|
|-|-|
|5|Ferric ammonium citrate|
|10|Oxalic acid|
|95|Water|

|2-step Oxalated|Developer|
|-|-|
|5|Potassium ferricyanide|
|1|Citric acid|
|94|Water|

Two step acidified with oxalic acid, which is quite strong, and the resulting oxalate ion is better then citrate at photoreduction.
Current record holder in my testing.
