/*
Copyright (c) 2017 InversePalindrome
Nihil - Map.hpp
InversePalindrome.com
*/


#pragma once

#include "Layer.hpp"
#include "Player.hpp"
#include "CollisionData.hpp"
#include "ResourceManager.hpp"
#include "ComponentSerializer.hpp"

#include <tmxlite/Map.hpp>
#include <tmxlite/ImageLayer.hpp>

#include <Box2D/Dynamics/b2World.h>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Drawable.hpp>

#include <vector>
#include <memory>
#include <unordered_map>


class Map : public sf::Drawable
{
public:
	Map(const sf::Vector2f& chunkSize, Player& player, b2World& world, ComponentSerializer& componentSerializer, ResourceManager& resourceManager, CollisionsData& collisionsData);

	void load(const std::string& filePath);
	
	sf::FloatRect getBounds() const;
	std::string getCurrentFilePath() const;

private:
	tmx::Map map;
	sf::Sprite background;
	sf::Vector2f chunkSize;
	sf::FloatRect bounds;
	std::string filePath;
	std::vector<std::unique_ptr<Layer>> layers;

	Player& player;
	b2World& world;
	ComponentSerializer& componentSerializer;
	ResourceManager& resourceManager;
	CollisionsData& collisionsData;

	void parseMap();

	void addImage(tmx::ImageLayer* imageLayer);
	void addObjects(tmx::ObjectGroup* objectLayer);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
