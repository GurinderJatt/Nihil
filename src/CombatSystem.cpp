/*
Copyright (c) 2017 InversePalindrome
Nihil - CombatSystem.cpp
InversePalindrome.com
*/


#include "CombatSystem.hpp"
#include "MathUtility.hpp"
#include "UnitConverter.hpp"
#include "FilePaths.hpp"


CombatSystem::CombatSystem(Entities& entities, Events& events, ComponentParser& componentParser) :
	System(entities, events),
	componentParser(componentParser)
{
	events.subscribe<entityplus::component_added<Entity, HealthComponent>>([&events](const auto& event)
	{
		if (event.entity.has_component<ControllableComponent>())
		{
			events.broadcast(DisplayHealthBar{ event.component });
		}
	});

	events.subscribe<entityplus::component_added<Entity, RangeAttackComponent>>([this](const auto& event) { addReloadTimer(event.entity); });
	events.subscribe<CombatOcurred>([this](const auto& event) { handleCombat(event.attacker, event.victim); });
	events.subscribe<BombExploded>([this](const auto& event) { handleExplosion(event.bomb, event.explosion); });
	events.subscribe<ShootProjectile>([this](const auto& event) { shootProjectile(event.shooter, event.projectileID, event.targetPosition ); });
	events.subscribe<ActivateBomb>([this](const auto& event) { addExplosion(event.bomb); });
	events.subscribe<ApplyKnockback>([this](const auto& event) { applyKnockback(event.attacker, event.victim); });
}

void CombatSystem::update(float deltaTime)
{
	this->callbacks.update();
	this->callbacks.clearCallbacks();
}

void CombatSystem::handleCombat(Entity attacker, Entity victim)
{
	std::int32_t damagePoints = 0;

	if (attacker.has_component<MeleeAttackComponent>())
	{
		damagePoints = attacker.get_component<MeleeAttackComponent>().getDamagePoints();
	}
	else if (attacker.has_component<BulletComponent>())
	{
		damagePoints = attacker.get_component<BulletComponent>().getDamagePoints();
	}
	else if (attacker.has_component<BombComponent>())
	{
		damagePoints = attacker.get_component<BombComponent>().getDamagePoints();
	}

	if (victim.has_component<HealthComponent>())
	{
		auto& health = victim.get_component<HealthComponent>();

		if (health.getHitpoints() > 0)
		{
			health.setHitpoints(health.getHitpoints() - damagePoints);
		}
		if (victim.has_component<ControllableComponent>())
		{
			this->events.broadcast(DisplayHealthBar{ health });
		}
		if (health.getHitpoints() <= 0)
	    {
			if (victim.has_component<DropComponent>())
			{
				this->events.broadcast(DroppedItem{ victim });
			}

			this->events.broadcast(DestroyEntity{ victim });
		}
	}
}

void CombatSystem::handleExplosion(Entity bomb, Entity explosion)
{
    const float explosionTime = 0.5f;
	
	this->callbacks.addCallbackTimer([this, explosion, bomb]() mutable
	{
		this->events.broadcast(DestroyEntity{ bomb });
		this->events.broadcast(DestroyEntity{ explosion });
	}, explosionTime);
}

void CombatSystem::shootProjectile(Entity shooter, const std::string& projectileID, const sf::Vector2f& targetPosition)
{
	auto projectileEntity = this->componentParser.parseEntity(-1, projectileID + ".txt");

	this->events.broadcast(ManageCollision{ shooter, projectileEntity, false });

	if (shooter.has_component<ParentComponent>() && projectileEntity.has_component<ChildComponent>())
	{
		this->events.broadcast(CreateTransform{ projectileEntity, shooter, this->getProjectileOffset(shooter) });

		if (shooter.has_tag<Turret>())
		{
			this->events.broadcast(PlayAnimation{ shooter, { EntityState::Attacking, Direction::Right }, false });
		}
	}
	if (shooter.has_component<PhysicsComponent>() && projectileEntity.has_component<PhysicsComponent>() && projectileEntity.has_component<SpriteComponent>())
	{
		const auto& shooterPhysics = shooter.get_component<PhysicsComponent>();
		auto& projectilePhysics = projectileEntity.get_component<PhysicsComponent>();
		auto& projectileSprite = projectileEntity.get_component<SpriteComponent>();

		if (projectileEntity.has_component<BulletComponent>())
		{
			this->shootBullet(shooter.get_component<PhysicsComponent>(), projectileEntity.get_component<BulletComponent>(), projectilePhysics, projectileSprite);
		}
		else if (projectileEntity.has_component<BombComponent>())
		{
			this->shootBomb(shooter.get_component<PhysicsComponent>(), projectileEntity.get_component<BombComponent>(), projectilePhysics, projectileSprite, targetPosition);
		}
	}
}

void CombatSystem::shootBullet(const PhysicsComponent& shooterPhysics, BulletComponent& bulletComponent, PhysicsComponent& projectilePhysics, SpriteComponent& projectileSprite)
{
	switch (shooterPhysics.getDirection())
	{
	case Direction::Left:
		projectilePhysics.applyForce({ -bulletComponent.getForce(), 0.f });
		projectileSprite.setRotation(180.f);
		break;
	case Direction::Right:
		projectilePhysics.applyForce({ bulletComponent.getForce(), 0.f });
		projectileSprite.setRotation(0.f);
		break;
	}

	this->events.broadcast(PlaySound{ bulletComponent.getSoundID(), false });
}

void CombatSystem::shootBomb(const PhysicsComponent& shooterPhysics, BombComponent& bombComponent, PhysicsComponent& projectilePhysics, SpriteComponent& spriteComponent, const sf::Vector2f& targetPosition)
{
	projectilePhysics.setPosition({ projectilePhysics.getPosition().x, projectilePhysics.getPosition().y + projectilePhysics.getBodySize().y + 0.1f });

	const auto xDistance = projectilePhysics.getPosition().x - UnitConverter::pixelsToMeters(targetPosition.x);
	const float angle = 45.f;

	const auto velocity = Utility::calculateRequiredVelocity(9.8f, std::abs(xDistance), angle, std::abs(projectilePhysics.getPosition().y));

	if (!std::isnan(velocity))
	{
		if (shooterPhysics.getPosition().x > UnitConverter::pixelsToMeters(targetPosition.x))
		{
			projectilePhysics.setVelocity({ -velocity * std::cos(angle), velocity * std::sin(angle) });
		}
		else
		{
			projectilePhysics.setVelocity({ velocity * std::cos(angle), velocity * std::sin(angle) });
		}
	}
}

void CombatSystem::addReloadTimer(Entity entity)
{
	if (entity.has_component<TimerComponent>())
	{
		auto& timer = entity.get_component<TimerComponent>();
		
		timer.addTimer("Reload", entity.get_component<RangeAttackComponent>().getReloadTime());
		timer.restartTimer("Reload");
	}
}

void CombatSystem::addExplosion(Entity bomb)
{
	if (bomb.has_component<BombComponent>() && !bomb.get_component<BombComponent>().hasActivated())
	{
		bomb.get_component<BombComponent>().setActivationStatus(true);
		
		this->callbacks.addCallbackTimer(
			[this, bomb]() mutable
		{
			if (bomb.sync())
			{
				auto& bombComponent = bomb.get_component<BombComponent>();

				auto explosion = this->componentParser.parseEntity(-1, bombComponent.getExplosionID() + ".txt");

				this->events.broadcast(CreateTransform{ explosion, bomb, {0.f, 0.f} });
				this->events.broadcast(BombExploded{ bomb, explosion });
				this->events.broadcast(PlaySound{ bombComponent.getSoundID(), false });
			}
		}, bomb.get_component<BombComponent>().getExplosionTime());
	}
}

void CombatSystem::applyKnockback(Entity attacker, Entity victim)
{
	if (attacker.has_component<PhysicsComponent>() && attacker.has_component<MeleeAttackComponent>() &&  victim.has_component<PhysicsComponent>())
	{
		auto& victimPhysics = victim.get_component<PhysicsComponent>();

		auto attackingDirection = attacker.get_component<PhysicsComponent>().getDirection();
		auto knockback = attacker.get_component<MeleeAttackComponent>().getKnockback();

		switch (attackingDirection)
		{
		case Direction::Left:
			victimPhysics.applyImpulse({ -knockback, 0.f });
			break;
		case Direction::Right:
			victimPhysics.applyImpulse({ knockback, 0.f });
			break;
		}
	}
}

sf::Vector2f CombatSystem::getProjectileOffset(Entity entity)
{
	if (entity.has_component<PhysicsComponent>())
	{
		auto& physics = entity.get_component<PhysicsComponent>();

		auto direction = physics.getDirection();
		const auto& size = physics.getBodySize();

		if (direction == Direction::Left)
		{
			return { UnitConverter::metersToPixels(-size.x), 0.f };
		}
		else if (direction == Direction::Right)
		{
			return {UnitConverter::metersToPixels(size.x), 0.f };
		}
		else if (direction == Direction::Up)
		{
			return { 0.f, UnitConverter::metersToPixels(size.y) };
		}
		else if (direction == Direction::Down)
		{
			return { 0.f, UnitConverter::metersToPixels(-size.y) }; 
		}
	}

	return {};
}