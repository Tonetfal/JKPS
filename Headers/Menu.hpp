#pragma once

#include "ParameterLine.hpp"
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "LogicalParameter.hpp"
#include "Settings.hpp"
#include "ChangedParametersQueue.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <vector>


class Button;

class Menu 
{
    public:
        using ParameterLinesContainer = std::map<ParameterLine::ID, std::shared_ptr<ParameterLine>>;
        using ParametersContainer = std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>>;

    public:
        Menu();

        void processInput();
        void update();
        void render();

        void openWindow();
        void closeWindow();
        bool isOpen() const;

        LogicalParameter &getParameter(LogicalParameter::ID id);
        ChangedParametersQueue &getChangedParametersQueue();

        void saveConfig(const std::vector<std::unique_ptr<Button>> &mKeys);
        void requestFocus();
        void requestReloadAssets();
        bool resetReloadAssetsRequest();


    private:
        void handleEvent();
        void handleRealtimeInput();

        void loadFonts();
        void loadTextures();

        void selectTab(unsigned idx);
        void selectAdvKeyPressVisKey(unsigned idx);
        void selectAdvBtnTextSepValKeys(unsigned idx);
        void selectAdvBtnTextKeys(unsigned idx);

        void initCollectionNames();
        void buildMenuTabs();
        void buildParametersMap();
        void buildParameterLines();
        void buildAdvKeys();
        
        void positionMenuLines();

        void moveSliderBarButtons(float offset);
        void moveSliderBarMouse(sf::Vector2i mousePos);
        void returnViewInBounds();

        void updateSaveStatsStrings();

    public:
        class KeyBlock : public sf::Drawable, public sf::Transformable, private sf::NonCopyable
        {
            public:
                using Ptr = std::unique_ptr<GfxParameter>;
                using Container = std::vector<Ptr>;


            public:
                KeyBlock(ParameterLinesContainer &parameterLines, ParameterLine::ID placeHolder, size_t parametersNumber);

                GfxParameter &current();
                void push(Ptr ptr);
                void select(unsigned idx);

                void setContainingTab(unsigned idx);
                unsigned getContainingTab() const;

                void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
                void handleEvent(sf::Event event, sf::Vector2f absCursorPos);


            private:
                ParameterLinesContainer &mParameterLines;
                ParameterLine::ID mFirstParameterLineId;
                size_t mParametersNumber;

                Container mContainer;
                unsigned mSelectedKeyIdx;
                unsigned mContainingTabIdx;
        };

        enum class AdvancedKeys
        {
            StatText,
            BtnTextSepVal,
            BtnText,
            GfxBtn,
            KeyPressVis
        };


    private:
        sf::RenderWindow mWindow;

        FontHolder mFonts;
        TextureHolder mTextures;

        sf::View mView;
        const float mScrollSpeed;
        float mHighViewBounds;

        sf::RectangleShape mSliderBar;
        const sf::Color mSliderBarDefaultColor;
        const sf::Color mSliderBarAimedColor;
        const sf::Color mSliderBarPressedColor;

        unsigned mSelectedTab;
        std::vector<std::unique_ptr<GfxParameter>> mTabs;
        std::map<unsigned, std::vector<ParameterLine::ID>> mTabParameters;
        unsigned mSelectedAdvKeyPressVisKey;
        std::map<AdvancedKeys, std::unique_ptr<KeyBlock>> mKeyBlocks;
        std::vector<float> mBounds;
        sf::RectangleShape mTabsBackground;

        ParametersContainer mParameters;
        ParameterLinesContainer mParameterLines;
        std::vector<std::string> mCollectionNames;
        ChangedParametersQueue mChangedParametersQueue;

        static bool paramValWasChanged;
};