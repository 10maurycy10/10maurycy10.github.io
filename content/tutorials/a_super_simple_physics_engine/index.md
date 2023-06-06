---
title: "Making a super simple physics engine from scratch"
date: 2023-06-05T17:41:05-07:00
tags: []
draft: false
math: true
---

It is very easy to create a simple physics engine that works decently well with very little code.
I wont be covering rendering, but it is fairly straightforward. (Code snippets will be in C, but this technique will work in any language).

The trick is to use Verlet integration, a mathematical trick to integrate (compute a solution to) Newton's laws of motion.
Verlet integration comes down to this equation:

$$ x_{n+1} = 2x_{n} - x_{n-1} + a \Delta t^2 $$

To make it easier to see what is going on here, lets rearrange the equation a bit:

$$ x_{n+1} = x_{n} + (x_{n} - x_{n-1}) + a \Delta t^2 $$

In short, instead of storing a velocity, it is computed from the current and last positions.
This has the advantage that it is possible to just change the position, and have things work as expected, without having to update the velocity, making almost everything easier.

These are some basic functions for working with 2d vectors, because C has no built in vector type.
These are 2d but should be trivially expandable to 3d.

```c
///////////////////////////////
// low level math functions. //
///////////////////////////////

typedef struct Vector2 {
        float x;
        float y;
} Vector2;

Vector2 vector_add(Vector2 v1, Vector2 v2) {
        Vector2 output = {.x = v1.x + v2.x, .y = v1.y + v2.y};
        return output;
}

Vector2 vector_sub(Vector2 v1, Vector2 v2) {
        Vector2 output = {.x = v1.x - v2.x, .y = v1.y - v2.y};
        return output;
}

Vector2 vector_mul_scaler(Vector2 v1, float s) {
        Vector2 output = {.x = v1.x * s, .y = v1.y * s};
        return output;
}

float vector_length(Vector2 v1) {
        return sqrt(v1.x * v1.x + v1.y * v1.y);
}
```

For each physics object, track the current position, last position.
Acceleration here will be used to sum up the effect of forces acting on the object every simulation step.

```c
typedef struct Body {
        float radius;
        
	Vector2 position_old;
        Vector2 position;
        Vector2 acceleration;
} Body;
```

Time for the actual Verlet integration.
The timestep is how much time passes between every simulation step. If the simulation is running at 60 updates per second, it should be 1/60 (~0.017).
The actual integration looks quite ugly becuase of C's lack of a built in vector type, so enjoy this psedocode:

```py
def update_position(body, dt):
	# Compute velocity in terms of units/timestep	
	velocity = body.position - body.position_old;
	# Save the position before updating
	body.position_old = body.position;
	# Use verlet integration to compute the new velocity.
	body.position = body.position + velocity + (body.acceleration * dt * dt);
	# Reset accelration.
	body.acceleration = (0, 0);
```

This is the actual C code for the function:

```c
// Use verlet integration to apply velocity and acceleration to the body
void update_position(Body* body, float dt) {
        // Compute velocity in terms of timestep based on position
        Vector2 velocity = vector_sub(body->position, body->position_old);
        // Save the position before updating
        body->position_old = body->position;
        // Move the object by the velocity, and the acceleration. 
        body->position = vector_add(body->position,vector_add(velocity, vector_mul_scaler(body->acceleration, dt * dt )));
        // Reset acceleration
        body->acceleration.x = 0;
        body->acceleration.y = 0;
}
```

To use this, create an object where the position, old position and acceleration are all zero, set the acceleration's y component to a negative number (why not 9.8?) and repeatedly call the `update_position` function.
This results in a nice falling object motion, where the object starts off at a standstill and quickly accelerates.

One physics object is all right, but some more would be nice.
This is a struct that holds a collection of physics object (and some rather uninteresting functions to manage memory).
Depending on your programming language, this could just become a list of objects.

```c
typedef struct World {
        Body* objects;
        int size; // How many objects are in the world at the given moment
        int capacity; // The total amount that can be stored at a given time
} World;

World world_with_capacity(int capacity) {
        World w = {
                .objects = malloc(capacity * sizeof(Body)),
                .size = 0,
                .capacity = capacity
        };
        return w;
}

// Returns 1 if sucessfull, zero if not
int insert_object(World* world, Body object) {
        if (world->capacity > world->size) {
                world->objects[world->size] = object;
                world->size++;
		return 1;
        } else {
                return 0;
        }
}

// Frees internal datastructures of a world, does not free the passed pointer.
void cleanup_world(World* w) {
        free(w->objects);
        // Set these to make sure no one tries to access any of the freed data;
        w->size = 0;
        w->capacity = 0;
        w->objects = 0;
}
```

These are trivial functions to step the simulation for all objects and to apply gravity to all objects.

```c
void update_positions(World* w, float dt) {
        for (int i = 0; i < w->size; i++) {
                update_position(&w->objects[i], dt);
        }
}

void apply_gravity(World* w) {
        for (int i = 0; i < w->size; i++) {
                w->objects[i].acceleration.y -= 9.8;
        }
}
```

Now, simply calling `update_positions` and `apply_gravity`, can simulate a whole bunch of objects at once.
Objects falling into an infinite void is fine and all, but the simulation becomes a lot more interesting if you limit the objects to a certain range.

This is where the magic of Verlet integration comes in, all that has to be done is to check if the object is outside of the bounds (here a circle with a radius if centered on the origin)
Verlet integration takes care of all the dynamics, and the object ends up nicely sliding around inside of the sphere:

```c
void collide(World* w) {
        // Bound the center of an object to 3 units away from (0, 0)
        for (int i = 0; i < w->size; i++) {
                if (vector_length(w->objects[i].position) > (3*3)) {
                        float position_scale = (3*3) / vector_length(w->objects[i].position);
                        // position = position * scale
                        w->objects[i].position = vector_mul_scaler(w->objects[i].position,position_scale);
                }
        }
}
```

It would also be nice of the objects would collide with one another.
Continuing the `collide` function, There are much faster algorithms to detect collisions then this \\(O(n^2)\\) one but this one is the most straightforward.
Assuming every object is a sphere, it just moves intersecting objects until they are no longer intersecting.

```c
void collide(World* w) {
	...
        // Check object colision, this is super inefficent.
        for (int i = 0; i < w->size; i++) {
                // This iterates up to i and not w->size to avoid rendundent checks and checking an object against itself
                for (int e = 0; e < i; e++) {
                        // The closest the objects can be without intersecting
                        float mindistance = w->objects[i].radius + w->objects[e].radius;

                        Vector2* object1 = &w->objects[i].position;
                        Vector2* object2 = &w->objects[e].position;
                        Vector2 difference = vector_sub(*object1, *object2);

                        // The current distance
                        float distance = vector_length(difference);

                        // Check for intersections
                        if (mindistance > distance) {
                                // How much each object has to be moved in order to not be intersecting
                                float delta = (mindistance - distance)/2;
                                // adjustment = normalize(difference) * deta
                                Vector2 adjustment = vector_mul_scaler(vector_mul_scaler(difference, 1.0/distance), delta);
                                // Move the objects by that distance
                                *object1 = vector_add(*object1, adjustment);
                                *object2 = vector_sub(*object2, adjustment);
                        }
                }
        }
}
```

To use it, just call `apply_gravity`, `update_positions`, and `collide` in a loop.
That's it, a working physics engine. Currently, collisions are inelastic, meaning that all kinetic energy is absorbed (no bounciness), but there is no friction.
Although at high timesteps objects sliding on a curved surface will slow down, but this is an artifact of limited integration steps.

The objects can be made bouncy by moving them more on collisions, becuase velocity is computed from position, this will have the effect of transferring more momentum.
The collision solver can also be made to take into account objects of different mass by moving them a different amount.

The collision solver can cause problems with large amounts of objects, this can be fixed by calling `collide` multiple times during a single simulation step.

[The code can be found here complete with a simple renderer based on SDL2.](https://github.com/10maurycy10/physics-verlet)

[Here is a video showing the results.](https://youtu.be/1-dzsX9fY-4)
