/*
Copyright (c) 2017 InversePalindrome
Nihil - Item.hpp
InversePalindrome.com
*/


#pragma once

#include <cstddef>
#include <unordered_map>


enum class Item : std::size_t
{
    Coin, OrangeGem, RedGem, PurpleGem, BlueGem, YellowGem, WhiteGem, SpeedBoost, JumpBoost, Laser, Heart, Character1, Character2
};

using Items = std::unordered_map<Item, std::size_t>;