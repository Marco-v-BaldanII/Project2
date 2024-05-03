#pragma once
#include "Module.h"
#include "Entity.h"

#include "Box2D/Box2D/Box2D.h"

#define METERS_TO_PIXELS(meters) ((int)((meters) * 50)) // Assuming 1 meter = 50 pixels
#define PIXELS_TO_METERS(pixels) ((float)(pixels) / 50)




enum ColliderShape {
    QUAD,
    CIRCLE
};

enum CollisionDetection {
    TELEPORT,
    ITERATIVE,
    RAYCAST
};

enum Integrators {
    EULER,
    SIMPLETIC_EULER,
    VERLET
};

enum ColliderType {

    y,
    UNKOWN
};

struct vec2 {
    float x;
    float y;

    float LengthSquared() const {
        return x * x + y * y;
    }

    float Length() const {
        return sqrt(LengthSquared());
    }

    vec2 Normalize() const {
        float length = Length();
        return { x / length, y / length };
    }
};

struct circle {
    int x;
    int y;
    int r;
};

struct ResitutionObj {
    int x, y;
    int dx, dy;
};

class Collider {
public:

    SDL_Rect data;
    ColliderType type;
    ColliderShape shape = QUAD;

    bool made_explosion = false;

    Collider(SDL_Rect r, ColliderType type, Entity* Listener, ColliderShape myShape = ColliderShape::QUAD) {
        data = r;
        this->type = type;
        listener = Listener;
        shape = myShape;
    }

    bool Intersects(SDL_Rect* r) const
    {
        if (shape == ColliderShape::QUAD) {
            if (r != nullptr && &data != nullptr) {
                if (data.w != 0 && data.h != 0 && r->w != 0 && r->h != 0) {
                    // returns true if there has been an intersection
                    return (data.x < r->x + r->w &&
                        data.x + data.w > r->x &&
                        data.y < r->y + r->h &&
                        data.h + data.y > r->y);
                }
            }


            else {
                return false;
            }
        }
    }

    bool Feet_Intersects(SDL_Rect* r) {

        SDL_Rect box_feet = { data.x, (data.y + data.h - 1), data.w, 2 };
        if (r != nullptr && &box_feet != nullptr) {

            // returns true if there has been an intersection
            return (box_feet.x < r->x + r->w &&
                box_feet.x + box_feet.w > r->x &&
                box_feet.y < r->y + r->h &&
                box_feet.h + box_feet.y > r->y);

        }


        else {
            return false;
        }
    }

    Entity* listener;

};

// Small class to return to other modules to track position and rotation of physics bodies
//class PhysBody
//{
//public:
//	PhysBody() : listener(NULL), body(NULL), ctype(ColliderType::UNKNOWN)
//	{}
//
//	~PhysBody() {}
//
//	void GetPosition(int& x, int& y) const;
//	float GetRotation() const;
//	bool Contains(int x, int y) const;
//	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;
//
//public:
//	int width, height;
//	b2Body* body;
//	Entity* listener;
//	ColliderType ctype;
//};

// Module --------------------------------------
class Physics : public Module, public b2ContactListener // TODO
{
public:

	// Constructors & Destructors
	Physics(bool isActive);
	~Physics();

	// Main module steps
	bool Start();
	bool PreUpdate();
	bool PostUpdate();
	bool CleanUp();

    Collider* AddCollider(SDL_Rect r, ColliderType type, Entity* Listener, ColliderShape myShape);

private:

	// Debug mode
	bool debug;

    List<Collider*> colliders;

};