---
title: "Making a 3d renderer from scratch [part 3]"
date: 2023-06-27T18:43:38-07:00
tags: ["programming", "3d"]
draft: false
---

[Currently the render works quite nicely](../part2), but calling it 3d is a bit of a strech. 
While does provide a first person veiwpoint, the entire map is a the same height.
Games with this limitation were, like Wolfenstein 3D, were released and did quite well, but modern audences want a bit more 3d in their 3d.

The one thing that could realy sell this as a 3d renderer would be allowing different wall and ceiling heights, where each room can have different floors and ceiling heights.

To render walls that are not centered on the y axis, the 3d projection code has to take a point on the z axis on the wall, and output a point on the screen's y axis.
It also has to be called twice for each wall endpoint, once for the top and once of the bottom.
The math for this is basicly the same as the original:

```py
fov = 0.4

screen_x = rotated.x / rotated.y / fov
screen_y = z / rotated.y / fov
```

To be able to move the camera around on the z axis, just subtract the camera's z from the wall's z before projecting.

Here is the test pillar with with some 3Dness added, the floor gets higher behind the pillar:
![A view of the pillar with accending wall heights but no steps rendered](stairs1.png)

The walls look fine but somethings missing on the floor; There is no step between rooms of different heights.

With the added 3rd dimention, portals need more sections, a top, bottom, and center.
The center is rendered as a portal, and the top and bottom sections as a wall.
The height of the bottom section should be the the new floor hight minus the current floor height.
The height of the top section should be the old ceiling height minus the new celing hieght.
If one of these are negative, the section should not be rendered.

This requires projecting 4 more points for portals, the start and end of the actual portal (center) area, at each endpoint, then interpolatating them for every x to get the start and end y values of the actual portal.
2 additional vertical lines have to be drawn, from the bottom of the ceiling to the top of the center area (top section), and from the bottom of the center area to the start of the floor (bottom section).
The bounds also have to be updated according to the bounds of the center area.

![A pillar, with accending wall heights, and correctly rendered steps between heights](stairs2.png)

Essensaly, what this is doing is rendering 2 walls above and below the portal, if a step is nessicary.

That's it, a working 3d software renderer.
While not capable of rendering arbitary 3d geometry, it is actualy slightly more capable than the DOOM renderer.

Part 4 will cover adding texturing to the render, to make thing look more intresting.

[Here is the code used to render the images, including the renderer, basic (and buggy) movement code, and map loading.](https://github.com/10maurycy10/3d_engine/tree/2ed75fbd396e85dcb48ebaaa3de588889e8fed88)



