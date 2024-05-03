#include "Physics.h"
#include "Input.h"
#include "app.h"
#include "Log.h"
#include "math.h"
#include "SDL/include/SDL_keycode.h"
#include "Defs.h"
#include "Log.h"
#include "Render.h"
#include "Player.h"
#include "Window.h"
#include "Box2D/Box2D/Box2D.h"

// Tell the compiler to reference the compiled Box2D libraries
#ifdef _DEBUG
#pragma comment( lib, "../Game/Source/External/Box2D/libx86/DebugLib/Box2D.lib" )
#else
#pragma comment( lib, "../Game/Source/External/Box2D/libx86/ReleaseLib/Box2D.lib" )
#endif

Physics::Physics(bool isActive) : Module(isActive)
{
	// Initialise all the internal class variables, at least to NULL pointer
	
}

// Destructor
Physics::~Physics()
{
	// You should do some memory cleaning here, if required
}

bool Physics::Start()
{
	LOG("Box2D sucks");

	

	return true;
}

// 
bool Physics::PreUpdate()
{
	bool ret = true;

	for (ListItem<Collider*>* it = colliders.start; it != NULL; it = it->next) {

		for (ListItem<Collider*>* it2 = colliders.start; it2 != NULL; it2 = it2->next) {

			if (it != it2) {

				if (it->data->Intersects(&it2->data->data)) {


					it->data->listener->OnCollision(it->data, it2->data);

				}

			}

		}

	}


	return ret;
}

Collider* Physics::AddCollider(SDL_Rect r, ColliderType type, Entity* Listener, ColliderShape myShape) {


	Collider* col = new Collider(r, type, Listener, myShape);
	colliders.Add(col);

	return col;
}

//PhysBody* Physics::CreateRectangle(int x, int y, int width, int height, bodyType type)
//{
//	b2BodyDef body;
//
//	if (type == DYNAMIC) body.type = b2_dynamicBody;
//	if (type == STATIC) body.type = b2_staticBody;
//	if (type == KINEMATIC) body.type = b2_kinematicBody;
//
//	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
//
//	b2Body* b = world->CreateBody(&body);
//	b2PolygonShape box;
//	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);
//
//	b2FixtureDef fixture;
//	fixture.shape = &box;
//	fixture.density = 1.0f;
//	b->ResetMassData();
//
//	b->CreateFixture(&fixture);
//
//	PhysBody* pbody = new PhysBody();
//	pbody->body = b;
//	b->SetUserData(pbody);
//	pbody->width = width * 0.5f;
//	pbody->height = height * 0.5f;
//
//	return pbody;
//}

//PhysBody* Physics::CreateCircle(int x, int y, int radious, bodyType type)
//{
//	// Create BODY at position x,y
//	b2BodyDef body;
//
//	if (type == DYNAMIC) body.type = b2_dynamicBody;
//	if (type == STATIC) body.type = b2_staticBody;
//	if (type == KINEMATIC) body.type = b2_kinematicBody;
//
//	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
//
//	// Add BODY to the world
//	b2Body* b = world->CreateBody(&body);
//
//	// Create SHAPE
//	b2CircleShape circle;
//	circle.m_radius = PIXEL_TO_METERS(radious);
//
//	// Create FIXTURE
//	b2FixtureDef fixture;
//	fixture.shape = &circle;
//	fixture.density = 1.0f;
//	b->ResetMassData();
//
//	// Add fixture to the BODY
//	b->CreateFixture(&fixture);
//
//	// Create our custom PhysBody class
//	PhysBody* pbody = new PhysBody();
//	pbody->body = b;
//	b->SetUserData(pbody);
//	pbody->width = radious * 0.5f;
//	pbody->height = radious * 0.5f;
//
//	// Return our PhysBody class
//	return pbody;
//}

//PhysBody* Physics::CreateRectangleSensor(int x, int y, int width, int height, bodyType type)
//{
//	// Create BODY at position x,y
//	b2BodyDef body;
//	if (type == DYNAMIC) body.type = b2_dynamicBody;
//	if (type == STATIC) body.type = b2_staticBody;
//	if (type == KINEMATIC) body.type = b2_kinematicBody;
//	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
//
//	// Add BODY to the world
//	b2Body* b = world->CreateBody(&body);
//
//	// Create SHAPE
//	b2PolygonShape box;
//	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);
//
//	// Create FIXTURE
//	b2FixtureDef fixture;
//	fixture.shape = &box;
//	fixture.density = 1.0f;
//	fixture.isSensor = true;
//
//	// Add fixture to the BODY
//	b->CreateFixture(&fixture);
//
//	// Create our custom PhysBody class
//	PhysBody* pbody = new PhysBody();
//	pbody->body = b;
//	b->SetUserData(pbody);
//	pbody->width = width;
//	pbody->height = height;
//
//	// Return our PhysBody class
//	return pbody;
//}

//PhysBody* Physics::CreateChain(int x, int y, int* points, int size, bodyType type)
//{
//	// Create BODY at position x,y
//	b2BodyDef body;
//	if (type == DYNAMIC) body.type = b2_dynamicBody;
//	if (type == STATIC) body.type = b2_staticBody;
//	if (type == KINEMATIC) body.type = b2_kinematicBody;
//	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
//
//	// Add BODY to the world
//	b2Body* b = world->CreateBody(&body);
//
//	// Create SHAPE
//	b2ChainShape shape;
//	b2Vec2* p = new b2Vec2[size / 2];
//	for (uint i = 0; i < size / 2; ++i)
//	{
//		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
//		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
//	}
//	shape.CreateLoop(p, size / 2);
//
//	// Create FIXTURE
//	b2FixtureDef fixture;
//	fixture.shape = &shape;
//
//	// Add fixture to the BODY
//	b->CreateFixture(&fixture);
//
//	// Clean-up temp array
//	delete p;
//
//	// Create our custom PhysBody class
//	PhysBody* pbody = new PhysBody();
//	pbody->body = b;
//	b->SetUserData(pbody);
//	pbody->width = pbody->height = 0;
//
//	// Return our PhysBody class
//	return pbody;
//}

// 
bool Physics::PostUpdate()
{
	bool ret = true;

	

	return ret;
}

// Called before quitting
bool Physics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	

	return true;
}

// Callback function to collisions with Box2D
//void Physics::BeginContact(b2Contact* contact)
//{
//	// Call the OnCollision listener function to bodies A and B, passing as inputs our custom PhysBody classes
//	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
//	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();
//
//	if (physA && physA->listener != NULL)
//		physA->listener->OnCollision(physA, physB);
//
//	if (physB && physB->listener != NULL)
//		physB->listener->OnCollision(physB, physA);
//}
//
////--------------- PhysBody
//
//void PhysBody::GetPosition(int& x, int& y) const
//{
//	b2Vec2 pos = body->GetPosition();
//	x = METERS_TO_PIXELS(pos.x) - (width);
//	y = METERS_TO_PIXELS(pos.y) - (height);
//}
//
//float PhysBody::GetRotation() const
//{
//	return RADTODEG * body->GetAngle();
//}
//
//bool PhysBody::Contains(int x, int y) const
//{
//	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
//
//	const b2Fixture* fixture = body->GetFixtureList();
//
//	while (fixture != NULL)
//	{
//		if (fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
//			return true;
//		fixture = fixture->GetNext();
//	}
//
//	return false;
//}

//int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
//{
//	int ret = -1;
//
//	b2RayCastInput input;
//	b2RayCastOutput output;
//
//	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
//	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
//	input.maxFraction = 1.0f;
//
//	const b2Fixture* fixture = body->GetFixtureList();
//
//	while (fixture != NULL)
//	{
//		if (fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
//		{
//			// do we want the normal ?
//
//			float fx = x2 - x1;
//			float fy = y2 - y1;
//			float dist = sqrtf((fx * fx) + (fy * fy));
//
//			normal_x = output.normal.x;
//			normal_y = output.normal.y;
//
//			return output.fraction * dist;
//		}
//		fixture = fixture->GetNext();
//	}
//
//	return ret;
//}