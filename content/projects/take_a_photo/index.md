---
title: "Let's take a photo:"
date: 2025-08-07
tags: ["chemistry", "photography"]
---

It's been almost 200 years since the [oldest surviving photograph](https://en.wikipedia.org/wiki/View_from_the_Window_at_Le_Gras) was taken:

![A ghostly image of rooftops, recorded on a metal plate.](/projects/take_a_photo/first_photo.jpg)
<center style='color: gray'>The exact date isn't known, but this was taken sometime during 1826.</center>

This isn't a description of reality, like a painting or a sculpture.
This is a piece of reality caught in a trap and pinned up for viewing -- even two centuries later.

To take our own pictures, we'll need a light sensitive material:
I recommend using silver chloride or iron citrate because they are relatively forgiving and don't require any super nasty chemicals.

One catch is that a lot of nicer paper has a base added, which can interfere with the chemistry:
If your paper says something about being "buffered" or "archival", add some citric acid to the first solution or soak the paper in vinegar before using it. 

## Silver:

<div style="border: 1px solid yellow; padding: 0.5em;">
Silver nitrate solutions stain <em>everything</em>. 
Use gloves and cover up your work surface.
If that fails, a solution of sodium thiosulfate and potassium ferricyanide can help remove the stains.
</div>

For the silver chloride process, brush some 10% (by weight) silver nitrate solution onto watercolor paper, let it dry, and apply 3% table salt solution:

![White paper with a box labeled "silver"](/projects/take_a_photo/silver1.jpg)
<center style='color: gray'>Sensitized paper.</center>

The two salts react to white colored silver chloride, but when exposed to light, the precipitate turns black due to the formation of finely powdered silver:
 
![Dark gray spot on the paper](/projects/take_a_photo/silver2.jpg)
<center style='color: gray'>Paper after being exposed to ultraviolet from an LED</center>
<br>
<center><b>Ag(NO<sub>3</sub>) + NaCl ➔ AgCl<sub>(s)</sub> + Na(NO<sub>3</sub>)</b></center> 
<center><b>2 AgCl<sub>(s)</sub> + hv ➔ 2 Ag<sub>(s)</sub> + Cl<sub>2 (gas)</sub></b></center> 

To end the exposure, wash the paper in water followed by a 5% sodium thiosulfate solution to remove remaining silver chloride:

![Dark brown spot on paper](/projects/take_a_photo/silver3.jpg)
<center style='color: gray'>Fixed paper. The brown is oxidized silver powder.</center>

I recommend doing a final wash with water to remove any residual chemicals.

Everything between applying the table salt and thiosulfate should be done in a dimly lit environment to avoid unintended darkening. 
The sensitivity is quite low by photographic standards, so you don't need a dark room, but having a dim room is a good idea.

If washed to remove residual silver nitrate, and protected from light, the sensitized paper will stay usable for years. 
Just don't let it directly touch any metals... and mark which side was treated, because the front and back look identical once dry.

## Iron/Cyanotype:

For the iron based process, paint a solution containing 5 parts ferric ammonium citrate and 2 parts potassium ferricyanide onto paper:

![Yellow stained paper](/projects/take_a_photo/iron1.jpg)
<center style='color: gray'>Coated paper.</center>
<br>
<div style="border: 1px solid yellow; padding: 0.5em;">
Despite having cyanide in the name, these chemicals are not very toxic, and don't stain nearly as bad as silver nitrate does.
That being said, still use gloves unless you want to do a smurf impression the next time you go outside.
Also, you can add oxalic acid to improve the sensitivity, at the cost of being somewhat toxic.
</div>

When exposed to light, the citrate reduces the iron from +3 to +2 ions, which react with the ferricyanide to from Prussian blue:

![Yellow stain with bluish dot](/projects/take_a_photo/iron2.jpg)
<center style='color: gray'>Exposed paper.</center>
<br>
<center><b>Fe<sup> 3+</sup> + citrate<sup> 3-</sup> + hv ➔ Fe<sup> 2+</sup> + acetonedicarboxylate<sup> 2-</sup> + CO<sub>2</sub></b></center> 
<center><b>3 Fe<sup> 2+</sup> + Fe<sup> 3+</sup> + 2 Fe(CN)<sub>6</sub><sup> 3-</sup> ➔ Fe<sub>4</sub>(Fe(CN)<sub>6</sub>)<sub>3</sub></b></center> 

Because the Prussian blue is insoluble, the residual chemicals can be removed by washing the paper in water:

![Blue dot on paper](/projects/take_a_photo/iron3.jpg)
<center style='color: gray'>Washed and oxidized paper.</center>

When overexposed, some of the Prussian blue can be reduced, bleaching the color. 
In this case, the blue can be restored with dilute hydrogen peroxide or by waiting a few days for the air to do its thing. 

<div style="border: 1px solid white; padding: 0.5em;">
For better sensitivity, you do this as a two step process where the paper is coated in ferric ammonium citrate, exposed and then coated with ferricyanide.
</div>

The ferric ammonium citrate must be protected from light, even while it's still in the bottle.
If kept in the dark, the chemicals should last for years, but the solutions can develop mold.
The paper is highly variable, and can last anywhere from days to years depending on what's in it. 

## Contact printing:

The easiest way to record an object is to place it directly on the sensitive paper and shine a light on it:

![White-on-blue image of two MOSFETs](/projects/take_a_photo/fets.jpg)
<center style='color: gray'>Homemade photogram</center>

For the light source, I recommend the sun (fastest), UV lamps or bright white lights (slowest).

You can also print out inverted image onto transparency sheet and expose though it:

![Blue-on-white photo of a microwave tower on a hill](/projects/take_a_photo/fake_photo.jpg)
<center style='color: gray'>The drips are a result of insufficient washing. Don't skimp on the water.</center>

(This also works with film negatives if you have any)

Another option is to draw something onto clear glass or plastic, and use the paper to make copies.
Doing this was actually quite popular before computers and it's why so many old technical diagrams are blue. 

## Using a camera:

If you've played with a magnifying glass, you've probably seen a lens projecting an image --
if not, you're part of today's [lucky ten thousand](https://xkcd.com/1053/):

Hold a lens parallel to a piece of white paper, and adjust the distance until an image of what the lens is facing forms on the paper.
I find this works the best when pointing the lens out a window on a sunny day.

... so now we have a way to project an image of an object and a way to permanently record an image falling on paper:

![Diagram of a camera](/projects/take_a_photo/camera.svg)
<center style='color: gray'>Simple camera design</center>

The key design parameter is the distance between the lens and photographic paper: if it's wrong, everything will be out of focus. 
You'll need to measure the a good distance for your lens, and leave some adjustability for focusing.

Before taking a photo, go somewhere dark, load in a piece of treated paper, close the camera and cover the lens.
When your ready to take a picture, just uncover the lens and wait.
Once the exposure is done, cover the lens and take it somewhere dark to process the paper:

![Small, round, low res photos of outdoor scenes](/projects/take_a_photo/photos.jpg)
<center style='color: gray'>
ISO 0.00002, f/2-ish, Shutter speed: 2700 seconds<br>
<b>From top to bottom:</b><br>
A house, parking and a car.<br>
A rock in some grass.<br>
A spruce tree next to a road.
</center>

You really need a lot of light for this to work. 
Direct sunlight or bright long wave UV illumination is best. 

Lenses with a longer focal length will produce a larger image, and can record more detail, but the light will be spread out.
However, physically larger lenses will catch more light.
Avoid lenses that are small and have a long focal length, because those will need very long exposures.

For my lens, I used a 25mm Plössl telescope eyepiece, which is small, but has a correspondingly short focal length.
Even so, I still had to leave the camera for 45 minutes in direct sunlight.
On the bright side, I didn't need a viewfinder because I could just look at the photographic paper to check on focus and framing.

If your images have too much contrast, you can try pre-flashing, where the whole paper is exposed to a bit of light outside the camera.
When done right, this gets the paper out of the flat region of the transfer function and prevents the darker areas of the image from being clipped.
