---
title: "From 1 fps to 500 fps, optimizing the physics engine"
date: 2023-06-09T18:06:21-07:00
tags: []
draft: false
---

[The simple physics engine works fine](/tutorials/a_super_simple_physics_engine/), but is quite slow, only able to handle at most 1000 particles in real time, and enabling compiler optimizations doesn't do much to improve this.

The slowest part of it is collision checking, which is quite slow becuase a check is being run between *every* pair of particles.
This is not only slow, but gets slower very fast as more particles are added, proportional to the amount of particles squared. (O(n^2) time complexity)
Most of these particles are very far apart, and checking collisions is pointless, but how can these be ruled these out without checking one at a time?

The simplest solution is to split the world into a grid of cells, such that a particle in a cell can only ever collide with the neighboring cells.
(Cell size must be less than or equal to 2 times the radius.)

To use this for collision checking, each object must be sorted into a cell. 
Then for every object, find what cell it is in, and collide it only with object in it's own and neighboring cell.

Not only does this avoid unnecessary checks, but it doesn't nest loops over all the particles, meaning that it only gets slower linearly (O(n) time complexity).
This means that doubling the object count doubles the time it takes, instead of quadrupling it.

This article won't include the full code for the grid datastructure, as it is fairly uninteresting and C specific.
The important bit is here, the helper functions for [allocation, freeing, and access can be found on GitHub](https://github.com/10maurycy10/physics-verlet/):

```c
// A datastructure used to optimize collision detection
// This works by breaking up space into a array of grid cells, binning all objects into a cell, to be able to quickly find nearby objects.
// Basicly, this is a lookup table for objects based on position
//
// The shape is defined by start_x, start_y, which are the lowest X and Y cordinates in the grid, and the start of the first cell
// x_size and y_size define how many cells the grid has, and cellsize is the sidelength of a single cell 
typedef struct AccessGrid {
        float start_x;
        float start_y;
        float cellsize;

        int x_size;
        int y_size;
        int** object_list_length;
        int** object_list;
} AccessGrid;
```

The actual collision checking is done in a separate function, it checks to make sure an object is not being collided with itself, and tries to prevent duplicate checks.

```c
void physics_single_check(World* w, int idx1, int idx2) {
        // Avoid checking a cell against itself
        if (idx1 == idx2) return;
        // Avoid duplicate checks
        if (idx1 < idx2) return;
        float mindistance = w->objects[idx1].radius + w->objects[idx2].radius;

        Vector2* object1 = &w->objects[idx1].position;
        Vector2* object2 = &w->objects[idx2].position;
        Vector2 difference = vector_sub(*object1, *object2);

        float distance = vector_length(difference);

        // Check for intersections
        if (mindistance > distance) {
                float delta = (mindistance - distance) / 2;
                Vector2 adjustment = vector_mul_scaler(vector_mul_scaler(difference, 1.0/distance), delta);
                *object1 = vector_add(*object1, adjustment);
                *object2 = vector_sub(*object2, adjustment);
        }

}
```

This is a short function that simply runs the collision checks between every object in a cell and a given object.
It also checks if the cell is actually in the grid datastructure.

```c
// Do collison checks between all objects in a cell and an object
void collide_with_cell(World* w, AccessGrid* grid, int x, int y, int idx) {
        if (x < 0 || x >= grid->x_size || y < 0 || y >= grid->y_size) return;

        int* indecies = access_grid_get(grid, x, y);
        int length = grid->object_list_length[x][y];
        for (int i = 0; i < length; i++)
                physics_single_check(w, idx, indecies[i]);
}
```

From here a simple implementation of the optimized collision solver is rather simple.
First it populates the grid cells with the objects in each cell, then checks for collisions between objects

```c
void world_optimized_collide(World* w, AccessGrid* grid) {
	// Populate the access grid with all the particles
	access_grid_clear(grid);

	for (int i = 0; i < w->size; i++) {
		Vector2 location = w->objects[i].position;
		int cellx = ((location.x - grid->start_x)/grid->cellsize);
		int celly = ((location.y - grid->start_y)/grid->cellsize);
		if (cellx >= 0 && cellx < grid->x_size && celly >= 0 && celly < grid->y_size )
			access_grid_append(grid, cellx, celly, i);
	}
	
	// For every cell in the grid, check collisons with neigboring cells
	for (int i = 0; i < w->size; i++) {	
		Vector2 location = w->objects[i].position;
		int x = (location.x - grid->start_x)/grid->cellsize;
		int y = (location.y - grid->start_y)/grid->cellsize;
		collide_with_cell(w, grid, x - 1, y - 1, i);
		collide_with_cell(w, grid, x - 1, y, i);
		collide_with_cell(w, grid, x - 1, y + 1, i);
		collide_with_cell(w, grid, x, y - 1, i);
		collide_with_cell(w, grid, x, y, i);
		collide_with_cell(w, grid, x, y + 1, i);
		collide_with_cell(w, grid, x + 1, y - 1, i);
		collide_with_cell(w, grid, x + 1, y, i);
		collide_with_cell(w, grid, x + 1, y + 1, i);
	}
}
```

This implementation has a rather big problem: it only works when all objects have a radius of more than 1/2 of a cell.
A solution to this is to compute all the cells occupied by a particle when adding it, then do the same to find all cells that have to be checked for collisions.
This actually also provides better performance for smaller particles than the past one, because most of the time it only checks for collisions in a single cell, not 9.

```c
// An optimized collision solver
// max_x, min_x, max_y, min_y are the dimentrions for any particles
// Cell size should be more then twice largest radius in the simulation, but violating this will no longer break things.
void world_optimized_collide(World* w, AccessGrid* grid) {
        // Populate the access grid with all the particles
        access_grid_clear(grid);

        for (int i = 0; i < w->size; i++) {
                Vector2 location = w->objects[i].position;
                float radius = w->objects[i].radius;

                int grid_x_start =      ((int)(location.x - radius - grid->start_x)/grid->cellsize);
                int grid_x_end =        ((int)(location.x + radius - grid->start_x)/grid->cellsize);
                int grid_y_start =      ((int)(location.y - radius - grid->start_y)/grid->cellsize);
                int grid_y_end =        ((int)(location.y + radius - grid->start_y)/grid->cellsize);

                for (int cellx = grid_x_start; cellx <= grid_x_end; cellx++) {
                        for (int celly = grid_y_start; celly <= grid_y_end; celly++) {
                                if (cellx >= 0 && cellx < grid->x_size && celly >= 0 && celly < grid->y_size ) {
                                        access_grid_append(grid, cellx, celly, i);
                                }
                        }
                }
        }

        for (int i = 0; i < w->size; i++) {
                Vector2 location = w->objects[i].position;
                float radius = w->objects[i].radius;

                int grid_x_start =      ((int)(location.x - radius - grid->start_x)/grid->cellsize);
                int grid_x_end =        ((int)(location.x + radius - grid->start_x)/grid->cellsize);
                int grid_y_start =      ((int)(location.y - radius - grid->start_y)/grid->cellsize);
                int grid_y_end =        ((int)(location.y + radius - grid->start_y)/grid->cellsize);

                for (int check_x = grid_x_start; check_x <= grid_x_end; check_x++) {
                        for (int check_y = grid_y_start; check_y <= grid_y_end; check_y++) {
                                collide_with_cell(w, grid, check_x, check_y, i);
                        }
                }
        }
}
```

[Full code for the optimized solver available on GitHub, under the `physics_optimized.h` file](https://github.com/10maurycy10/physics-verlet/)

This still has a few flaws, it preforms redundant checks on large particles, and as a particle can move from one cell to another during the collision checking, it can miss a few, resulting in slightly worse stability.
Despite this, because it does orders of magnitude less work when the cell size is set right (~3 times the largest radius seems to be optimal, but highly uneven size distributions can change this), it is much faster.

At 5000 objects, it is ~100 times faster than the unoptimized version, and the difference only get bigger with larger simulations.
Better yet, compiler optimizations actually improve things here, providing another ~3 times performance boost.

All this achieved without using optimization techniques like SIMD, multithreading, offloading the the GPU, branchless programming, rewriting in assembly, etc.
By simply finding a clever way to do less work, the code becomes hundreds (or with very large simulations thousands or tens of thousands) of times faster.
SIMD, multithreading, GPU offload, etc can make the work go a bit faster, but it cant fix a fundamentally slow algorithm, or one with poor time complexity (gets slower really fast).
