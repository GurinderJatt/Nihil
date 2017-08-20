/*
Copyright (c) 2017 InversePalindrome
Nihil - PhysicsComponent.cpp
InversePalindrome.com
*/


#include "PhysicsComponent.hpp"

#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>


PhysicsComponent::PhysicsComponent(b2World& world, const b2Vec2& bodySize, b2BodyType physicalType, ObjectType objectType, std::int8_t collisionGroup) :
	PhysicsComponent(world, bodySize, 0.f, 0.f, physicalType, objectType, collisionGroup)
{
}

PhysicsComponent::PhysicsComponent(b2World& world, const b2Vec2& bodySize, float maxVelocity,
	float accelerationRate, b2BodyType physicalType, ObjectType objectType, std::int8_t collisionGroup)  :
	body(nullptr),
	maxVelocity(maxVelocity),
	accelerationRate(accelerationRate),
	objectType(objectType)
{
	b2BodyDef bodyDefinition;
	bodyDefinition.type = physicalType;

	bodyDefinition.fixedRotation = true;
	
	b2PolygonShape fixtureShape;
	fixtureShape.SetAsBox(bodySize.x, bodySize.y);
	
 	b2FixtureDef fixture;
	fixture.shape = &fixtureShape;
	fixture.density = 1.f;
	fixture.friction = 0.3f;
	fixture.filter.groupIndex = collisionGroup;

	body = world.CreateBody(&bodyDefinition);
	body->CreateFixture(&fixture);
}

b2Body& PhysicsComponent::getBody()
{
	return *this->body;
}

b2Vec2 PhysicsComponent::getPosition() const
{
	return this->body->GetPosition();
}

b2Vec2 PhysicsComponent::getVelocity() const
{
	return this->body->GetLinearVelocity();
}

b2BodyType PhysicsComponent::getType() const
{
	return this->body->GetType();
}

float PhysicsComponent::getMass() const
{
	return this->body->GetMass();
}

float PhysicsComponent::getMaxVelocity() const
{
	return this->maxVelocity;
}

float PhysicsComponent::getAccelerationRate() const
{
	return this->accelerationRate;
}

void PhysicsComponent::setPosition(const b2Vec2& position)
{
	this->body->SetTransform(position, this->body->GetAngle());
}

void PhysicsComponent::setMaxVelocity(float maxVelocity)
{
	this->maxVelocity = maxVelocity;
}

void PhysicsComponent::setAccelerationRate(float accelerationRate)
{
	this->accelerationRate = accelerationRate;
}

void PhysicsComponent::applyImpulse(const b2Vec2& impulse)
{
	this->body->ApplyLinearImpulseToCenter(impulse, true);
}

ObjectType PhysicsComponent::getObjectType() const
{
	return this->objectType;
}