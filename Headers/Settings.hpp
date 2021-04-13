#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "StringHelper.hpp"

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/stat.h>

class Settings
{
    public:
                                    Settings();
        void                        handleEvent(sf::Event event);
        void                        draw();

        void                        setWindowReference(sf::RenderWindow& window);

        void                        changeChangeability();
        bool                        isChangeable();
        void                        setChangeabilityPosition();


    private:

        std::string                 findParameter(std::string parameterName);

        void                        setDefaultKey(size_t index);

        void                        setupColor(sf::Color& color, const std::string information, const std::string parameterName);
        template <typename T>
        void                        setupVector(T& vector, int limitMin, int limitMax, const std::string information, const std::string parameterName);
        template <typename T>
        void                        setupDigitParameter(T& parameter, int limitMin, int limitMax, const std::string information, const std::string parameterName);
        void                        setupFilePathParameter(std::string& parameter, const std::string information, const std::string parameterName);
        void                        setupKey(std::vector<sf::Keyboard::Key>& keys, const std::string information, const std::string parameterName);

        bool                        isThereSameKey(sf::Keyboard::Key key, size_t& whichOne);

        void                        createDefaultConfig();


    public:
        static std::vector<sf::Keyboard::Key> Keys;
        static std::size_t          KeyAmount;

        static std::string          FontPath;
        static sf::Color            StatisticsTextColor;
        static sf::Color            KeyCountersTextColor;
        static std::size_t          StatisticsTextCharacterSize;
        static std::size_t          KeyCountersTextCharacterSize;

        static float                Distance;
        static float                SpaceBetweenButtonsAndStatistics;

        static std::string          ButtonTexturePath;
        static sf::Vector2u         ButtonTextureSize;
        static sf::Color            ButtonImageColor;
        static std::size_t          AnimationVelocity;
        static sf::Color            AnimationColor;
        static sf::Color            AnimationOnClickTransparency;

        static std::string          BackgroundTexturePath;
        static sf::Color            BackgroundColor;

        static sf::Keyboard::Key    KeyToIncrease;
        static sf::Keyboard::Key    KeyToDecrease;


    private:
        const std::string           configPath;

        const size_t                minimumKeys;
        const size_t                maximumKeys;

        sf::RenderWindow*           mWindow;

        bool                        mIsChangeable;
        sf::CircleShape             mIsChangeableAlert;
        sf::Color                   mAlertColor;

        sf::Keyboard::Key           mButtonToChange;
        int                         mButtonToChangeIndex;
};