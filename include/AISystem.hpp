/*
Copyright (c) 2017 InversePalindrome
Nihil - AISystem.hpp
InversePalindrome.com
*/


#pragma once

#include "System.hpp"
#include "Pathway.hpp"

#include <SFML/System/Vector2.hpp>

#include <optional>


class AISystem : public System
{
public:
    AISystem(Entities& entities, Events& events, Pathways& pathways);

    virtual void update(float deltaTime) override;

private:
    Pathways& pathways;
    Entity targetEntity;

    void updateMovement(Entity entity, PatrolComponent& patrol, const sf::Vector2f& position);

    void addProperties(Entity entity);

    void changeWaypoint(Entity entity);
    void chaseTarget(PatrolComponent& patrol, const sf::Vector2f& AIPosition, const sf::Vector2f& targetPosition);

    std::optional<Pathway> getPathway(Entity entity);
    std::optional<sf::Vector2f> getTargetPosition();

    bool isWithinRange(Entity AI, const sf::Vector2f& AIPosition, const sf::Vector2f& targetPosition, float visionRange);
    bool isFacingTarget(Entity entity);
};