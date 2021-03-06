/*
Copyright (c) 2017 InversePalindrome
Nihil - Game.cpp
InversePalindrome.com
*/


#include "Game.hpp"
#include "FilePaths.hpp"

#include <fstream>
#include <sstream>


Game::Game()
{
    loadLevels();
    loadAchievements();

    std::ifstream inFile(Path::levels / "Levels.txt");

    std::size_t directionType = 0u;
    float xGravity = 0.f, yGravity = 0.f;

    inFile >> currentLevel >> directionType >> xGravity >> yGravity >> spawnpoint.x >> spawnpoint.y;
}

Game::Game(const std::string & data)
{
    loadLevels();

    std::istringstream iStream(data);
    std::string line;

    while (std::getline(iStream, line))
    {
        std::istringstream inLine(line);

        std::string category;

        inLine >> category;

        if (category == "Game")
        {
            inLine >> gameName;
        }
        else if (category == "CurrentLevel")
        {
            inLine >> currentLevel;
        }
        else if (category == "Spawnpoint")
        {
            inLine >> spawnpoint.x >> spawnpoint.y;
        }
        else if (category == "Level")
        {
            std::string levelName;
            bool isLoaded;

            inLine >> levelName >> isLoaded;

            levels[levelName].isLoaded = isLoaded;
        }
        else if (category == "Item")
        {
            std::size_t item = 0u, quantity = 0u;

            inLine >> item >> quantity;

            items.emplace(Item{ item }, quantity);
        }
        else if (category == "Achievement")
        {
            std::size_t achievementID = 0u, currentQuantity = 0u, maxQuantity = 0u;

            inLine >> achievementID >> currentQuantity >> maxQuantity;

            achievements.emplace(Achievement{ achievementID }, std::make_pair(currentQuantity, maxQuantity));
        }
    }
}

std::optional<Entity> Game::getPlayer()
{
    if (this->player.sync())
    {
        return this->player;
    }
    else
    {
        return {};
    }
}

std::string Game::getGameName() const
{
    return this->gameName;
}

std::string Game::getCurrentLevel() const
{
    return this->currentLevel;
}

DirectionType Game::getCurrenDirectionType() const
{
    return this->levels.at(this->currentLevel).directionType;
}

sf::Vector2f Game::getSpawnpoint() const
{
    return this->spawnpoint;
}

std::string Game::getLevelMusic() const
{
    return this->levels.at(this->currentLevel).musicFile;
}

Game::Levels& Game::getLevels()
{
    return this->levels;
}

Items& Game::getItems()
{
    return this->items;
}

Achievements& Game::getAchievements()
{
    return this->achievements;
}

b2Vec2 Game::getCurrentGravity() const
{
    return this->levels.at(this->currentLevel).gravity;
}

void Game::setPlayer(Entity player)
{
    this->player = player;
}

void Game::setGameName(const std::string & gameName)
{
    this->gameName = gameName;
}

void Game::setCurrentLevel(const std::string & currentLevel)
{
    this->currentLevel = currentLevel;
}

void Game::setSpawnpoint(const sf::Vector2f & spawnpoint)
{
    this->spawnpoint = spawnpoint;
}

std::ostream& operator<<(std::ostream & os, const Game & game)
{
    os << "Game " << game.gameName << '\n';
    os << "CurrentLevel " << game.currentLevel << '\n';
    os << "Spawnpoint " << game.spawnpoint.x << ' ' << game.spawnpoint.y << '\n';

    for (const auto& level : game.levels)
    {
        os << "Level " << level.first << ' ' << level.second.isLoaded << '\n';
    }

    for (const auto& item : game.items)
    {
        os << "Item " << static_cast<std::size_t>(item.first) << ' ' << item.second << '\n';
    }

    for (const auto& achievement : game.achievements)
    {
        os << "Achievement " << static_cast<std::size_t>(achievement.first) << ' ' <<
            achievement.second.first << ' ' << achievement.second.second << '\n';
    }

    return os;
}

void Game::loadLevels()
{
    std::ifstream inFile(Path::levels / "Levels.txt");
    std::string line;

    while (std::getline(inFile, line))
    {
        std::istringstream iStream(line);

        std::string levelName, musicFile;
        std::size_t directionType = 0u;
        float xGravity = 0.f, yGravity = 0.f, xSpawnpoint = 0.f, ySpawnpoint = 0.f;

        iStream >> levelName >> directionType >> xGravity >> yGravity >> xSpawnpoint >> ySpawnpoint >> musicFile;

        this->levels.emplace(levelName, Level{ DirectionType{directionType}, { xGravity, yGravity }, false, musicFile });
    }
}

void Game::loadAchievements()
{
    std::ifstream inFile(Path::miscellaneous / "AchievementsData.txt");
    std::string line;

    while (std::getline(inFile, line))
    {
        std::istringstream iStream(line);

        std::size_t achievementID = 0u, maxQuantity = 0u;

        iStream >> achievementID >> maxQuantity;

        this->achievements[Achievement{ achievementID }] = { 0u, maxQuantity };
    }
}