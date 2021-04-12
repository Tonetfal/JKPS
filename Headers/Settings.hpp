#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <vector>
#include <fstream>
#include <iostream>

class Settings
{
    public:
                                    Settings(const std::string& FilePath);
        void                        handleEvent(sf::Event event);
        void                        draw();

        void                        setWindowReference(sf::RenderWindow& window);

        void                        changeChangeability();
        bool                        isChangeable();
        void                        setChangeabilityPosition();


    private:

        void                        setDefaultKey(size_t index);

        size_t                      controlKeyAmount(std::string& Information);
        void                        controlKeys(std::string& Information);

        // void                        setTextFont(std::string& Information);
        // void                        setColorText(std::string& Information);
        // void                        setCharacterSize(std::string& Information);

        // void                        setDistance(std::string& Information);

        // void                        setButtonImage(std::string& Information);
        // void                        setButtonImageTransparency(std::string& Information);
        // void                        setAnimationVelocity(std::string& Information);
        // void                        setAnimationColor(std::string& Information);


    public:                                    
        static std::size_t          KeyAmount;
        static std::vector<sf::Keyboard::Key> Keys;

        static sf::Color            StatisticsTextColor;
        static sf::Color            KeyCountersTextColor;
        static std::size_t          StatisticsTextCharacterSize;
        static std::size_t          KeyCountersTextCharacterSize;

        static float                Distance;
        static float                SpaceBetweenButtonsAndStatistics;

        static sf::Vector2u         DefaultButtonTextureSize;
        static sf::Color            ButtonImageTransparency;
        static std::size_t          AnimationVelocity;
        static sf::Color            AnimationColor;
        static sf::Color            AnimationOnClickTransparency;

        static sf::Keyboard::Key    KeyToIncrease;
        static sf::Keyboard::Key    KeyToDecrease;


    private:
        const size_t                minimumKeys;
        const size_t                maximumKeys;

        sf::RenderWindow*           mWindow;

        bool                        mIsChangeable;
        sf::CircleShape             mIsChangeableAlert;
        sf::Color                   mAlertColor;

        sf::Keyboard::Key           mButtonToChange;
        int                         mButtonToChangeIndex;
};