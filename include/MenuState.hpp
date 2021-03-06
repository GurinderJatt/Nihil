/*
Copyright (c) 2017 InversePalindrome
Nihil - MenuState.hpp
InversePalindrome.com
*/


#pragma once

#include "State.hpp"
#include "StateID.hpp"

#include <Thor/Particles/ParticleSystem.hpp>

#include <SFGUI/Button.hpp>

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>


class MenuState : public State
{
public:
    MenuState(StateMachine& stateMachine, StateData& stateData);

    virtual void handleEvent(const sf::Event& event) override;
    virtual void update(float deltaTime) override;
    virtual void draw() override;

    virtual void showWidgets(bool showStatus) override;

private:
    sf::Sprite background;
    sf::Text titleLabel;

    sfg::Button::Ptr playButton;
    sfg::Button::Ptr settingsButton;
    sfg::Button::Ptr quitButton;

    thor::ParticleSystem particleSystem;

    bool isTitleVisible;

    void transitionToState(StateID stateID);
};