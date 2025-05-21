#include "../Headers/Application.hpp"
#include "../Headers/Menu.hpp"
#include "../Headers/LogicalParameter.hpp"
#include "../Headers/ChangedParametersQueue.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/DefaultFiles.hpp"
#include "../Headers/Menu.hpp"
#include "../Headers/ConfigHelper.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Transformable.hpp>


const sf::Time Application::TimePerHookUpdate = sf::seconds(1.f / 60.f);

Application::Application()
{
    loadTextures();
    loadFonts();

    buildButtons();
    buildStatistics();

    openWindow();
    loadIcon();

    auto keySelector = std::make_unique<GfxButtonSelector>();
    mGfxButtonSelector = std::move(keySelector);

    auto buttonPositioner = std::make_unique<ButtonPositioner>(&mButtons);
    mButtonsPositioner = std::move(buttonPositioner);
    (*mButtonsPositioner)();

    auto statPositioner = std::make_unique<StatisticsPositioner>(&mStatistics);
    mStatisticsPositioner = std::move(statPositioner);
    (*mStatisticsPositioner)();

    auto bg = std::make_unique<Background>(mTextures, mWindow);
    mBackground = std::move(bg);

    auto kpsWindow = std::make_unique<KPSWindow>(mFonts);
    mKPSWindow = std::move(kpsWindow);

    auto graph = std::make_unique<KeysPerSecondGraph>();
    mGraph = std::move(graph);

    mMenu.saveConfig(mButtons);
}

void Application::run()
{
    sf::Clock clock;
	auto timeSinceLastEventUpdate = sf::Time::Zero;
	auto timeSinceLastHooksUpdate = sf::Time::Zero;

    while (mWindow.isOpen())
    {
        auto dt = clock.restart();
		timeSinceLastEventUpdate += dt;
		timeSinceLastHooksUpdate += dt;

		while (true)
		{
			int updateType = UpdateType::None;
			if (timeSinceLastHooksUpdate > TimePerHookUpdate)
			{
				timeSinceLastHooksUpdate -= TimePerHookUpdate;
				updateType |= UpdateType::Hooks;
			}

			int frames = getRenderUpdateFrequency();
			sf::Time TimePerEventUpdate = sf::seconds(1.f / static_cast<float>(frames));
			if (timeSinceLastEventUpdate > TimePerEventUpdate)
			{
				timeSinceLastEventUpdate -= TimePerEventUpdate;
				updateType |= UpdateType::Event;
			}

			if (updateType == UpdateType::None)
			{
				break;
			}

			processInput(static_cast<UpdateType>(updateType));
			update(static_cast<UpdateType>(updateType));
		}

        render();
    }
}

void Application::processInput(UpdateType type)
{
	if (type & UpdateType::Event)
	{
		// Open/close other windows, add/rm keys
		handleEvent();

		// Update changed parameters
		if (mMenu.isOpen())
			unloadChangesQueue();

		// Update assets if there is a request
		if (ParameterLine::resetRefreshState())
			resetAssets();
	}

	if (type & UpdateType::Hooks)
	{
		// Take buttons realtime input
		for (auto &button : mButtons)
			button->processInput();
	}

	if (type & UpdateType::Event)
	{
		if (!Settings::WindowTitleBar)
			moveWindow();

		// Make separate windows handle own events
		if (mMenu.isOpen())
			mMenu.processInput();
		if (mGfxButtonSelector->isOpen())
			mGfxButtonSelector->handleOwnInput();
		if (mKPSWindow->isOpen())
			mKPSWindow->handleOwnEvent();
		if (mGraph->isOpen())
			mGraph->handleOwnEvent();
	}
}

void Application::handleEvent()
{
    sf::Event event;
    while (mWindow.pollEvent(event))
    {
        if (event.type == sf::Event::MouseButtonPressed)
        {
            const auto button = event.mouseButton.button;
            if (button == sf::Mouse::Right)
            {
                auto idx = 0u;
                if (isPressPerformedOnButton(idx))
                {
                    mGfxButtonSelector->setKey(mButtons[idx]->getLogKey());
                    mGfxButtonSelector->openWindow();
                }
            }
        }
        
        if (event.type == sf::Event::KeyPressed)
        {
            const auto key = event.key;
            if (key.control)
            {
                auto btnAmtChanged = false;
                if (key.code == Settings::KeyToIncreaseKeys || key.code == Settings::AltKeyToIncreaseKeys)
                {
                    addButton(*new LogKey("A", "A", new sf::Keyboard::Key(sf::Keyboard::A), nullptr));
                    btnAmtChanged = true;
                }

                if (key.code == Settings::KeyToIncreaseButtons)
                {
                    addButton(*new LogKey("M Left", "M Left", nullptr, new sf::Mouse::Button(sf::Mouse::Left)));
                    btnAmtChanged = true;
                }

                if (key.code == Settings::KeyToDecreaseKeys || key.code == Settings::AltKeyToDecreaseKeys || key.code == Settings::KeyToDecreaseButtons)
                {
                    removeButton();
                    btnAmtChanged = true;
                }

                if (btnAmtChanged)
                {
                    (*mButtonsPositioner)();
                    (*mStatisticsPositioner)();
                    resizeWindow();
                    mBackground->rescale();
                }

                if (key.code == Settings::KeyToOpenKPSWindow)
                {
                    if (mKPSWindow->isOpen())
                        mKPSWindow->closeWindow();
                    else
                        mKPSWindow->openWindow();
                }

                if (key.code == Settings::KeyToOpenMenuWindow)
                {
                    if (mMenu.isOpen())
                        mMenu.closeWindow();
                    else
                        mMenu.openWindow();
                }

                // if (key == Settings::KeyToOpenGraphWindow)
                // {
                //     if (mGraph->isOpen())
                //         mGraph->closeWindow();
                //     else
                //         mGraph->openWindow();
                // }

                if (key.code == Settings::KeyToReset)
                {
                    for (auto &button : mButtons)
                        button->reset();
                }

                if (key.code == Settings::KeyExit)
                {
                    mMenu.saveConfig(mButtons);
                    mWindow.close();
                    return;
                }
            }
        }

        if (event.type == sf::Event::Closed)
        {
            mMenu.saveConfig(mButtons);
            mWindow.close();
            return;
        }
    }
}

void Application::update(UpdateType type)
{
	if (type & UpdateType::Event)
	{
		for (auto &button : mButtons)
			button->update();
		for (auto &line : mStatistics)
			line->update();

		if (mMenu.isOpen())
			mMenu.update();

		if (mKPSWindow->isOpen())
			mKPSWindow->update();

		// if (mGraph->isOpen())
		//     mGraph->update();
	}

	if (type & UpdateType::Hooks)
	{
		Button::moveIndex();
		for (auto &button : mButtons)
			button->accumulateBeatsPerMinute();
	}
}

void Application::render()
{
    mWindow.clear();

    mWindow.draw(*mBackground);

    for (const auto &elem : mButtons)
        mWindow.draw(*elem);
    for (const auto &elem : mStatistics)
        mWindow.draw(*elem);

    if (mMenu.isOpen())
        mMenu.render();
    if (mGfxButtonSelector->isOpen())
        mGfxButtonSelector->render();
    if (mKPSWindow->isOpen())
        mKPSWindow->render();
    if (mGraph->isOpen())
        mGraph->render();
    
    mWindow.display();
}

void Application::unloadChangesQueue()
{
    auto &queue = mMenu.getChangedParametersQueue();
    while (!queue.isEmpty())
    {
        auto pair = queue.pop();

        if (GfxStatisticsLine::parameterIdMatches(pair.first))
        {
            for (auto &line : mStatistics)
                line->updateParameters();
            (*mStatisticsPositioner)();
        }

        if (Button::parameterIdMatches(pair.first))
        {
            unsigned idx = 0;
            for (auto &button : mButtons)
            {
                button->updateParameters();
                ++idx;
            }
            (*mButtonsPositioner)();
        }

        if (KPSWindow::parameterIdMatches(pair.first))
        {
            mKPSWindow->updateParameters();
        }

        if (parameterIdMatches(pair.first))
        {
            mWindow.setSize(sf::Vector2u(getWindowWidth(), getWindowHeight()));
            mWindow.setView(sf::View(sf::FloatRect(0.f, 0.f, mWindow.getSize().x, mWindow.getSize().y)));
            mMenu.requestFocus();
        }

        if (pair.first == LogicalParameter::ID::MainWndwTitleBar)
        {
            openWindow();
        }

		if (pair.first == LogicalParameter::ID::RenderUpdateFrequency)
		{
			mWindow.setFramerateLimit(getRenderUpdateFrequency());
		}

        mBackground->rescale();
    }
}

void Application::resetAssets()
{
    mFonts.clear();
    mTextures.clear();

    loadTextures();
    loadFonts();

    auto idx = 0u;
    for (auto &button : mButtons)
    {
        button->updateAssets();
        button->setPosition(Button::getWidth(idx), Button::getHeight(idx));
        ++idx;
    }

    for (auto &line : mStatistics)
        line->updateAsset();

    mBackground->updateAssets();
    (*mButtonsPositioner)();
    (*mStatisticsPositioner)();

    mMenu.saveConfig(mButtons);
}

void Application::loadTextures()
{
    if (!mTextures.loadFromFile(Textures::Button, Settings::GfxButtonTexturePath))
        mTextures.loadFromMemory(Textures::Button, Settings::DefaultButtonTexture, 2700);

    if (!mTextures.loadFromFile(Textures::Animation, Settings::AnimationTexturePath))
        mTextures.loadFromMemory(Textures::Animation, Settings::DefaultAnimationTexture, 15800);

    // mTextures.loadFromMemory(Textures::KeyPressVis, Settings::KeyPressVisTexture, 4200);

    Settings::isGreenscreenSet = Settings::BackgroundTexturePath == "GreenscreenBG.png";
    if (Settings::isGreenscreenSet)
        mTextures.loadFromMemory(Textures::Background, Settings::DefaultGreenscreenBackgroundTexture, 596);
    else
    {
        if (!mTextures.loadFromFile(Textures::Background, Settings::BackgroundTexturePath))
            mTextures.loadFromMemory(Textures::Background, Settings::DefaultBackgroundTexture, 2700);
    }
}

void Application::loadFonts()
{
    if (!mFonts.loadFromFile(Fonts::ButtonValue, Settings::ButtonTextFontPath))
        mFonts.loadFromMemory(Fonts::ButtonValue, Settings::KeyCountersDefaultFont, 581700);

    if (!mFonts.loadFromFile(Fonts::Statistics, Settings::StatisticsTextFontPath))
        mFonts.loadFromMemory(Fonts::Statistics, Settings::StatisticsDefaultFont, 581700);

    if (!mFonts.loadFromFile(Fonts::KPSText, Settings::KPSWindowTextFontPath))
        mFonts.loadFromMemory(Fonts::KPSText, Settings::DefaultKPSWindowFont, 459300);

    if (!mFonts.loadFromFile(Fonts::KPSNumber, Settings::KPSWindowNumberFontPath))
        mFonts.loadFromMemory(Fonts::KPSNumber, Settings::DefaultKPSWindowFont, 459300);
}

void Application::loadIcon()
{
    sf::Image icon;
    icon.loadFromMemory(IconTexture, 53200);
    mWindow.setIcon(256, 256, icon.getPixelsPtr());
}

void Application::buildStatistics()
{
    using Ptr = std::unique_ptr<GfxStatisticsLine>;
    auto linePtr = Ptr();
    auto id = static_cast<unsigned>(GfxStatisticsLine::StatisticsID::KPS);
    
    linePtr = Ptr(new GfxStatisticsLine(mFonts, Settings::ShowStatisticsKPS, static_cast<GfxStatisticsLine::StatisticsID>(id)));
    mStatistics[id] = std::move(linePtr);
    ++id;

    linePtr = Ptr(new GfxStatisticsLine(mFonts, Settings::ShowStatisticsTotal, static_cast<GfxStatisticsLine::StatisticsID>(id)));
    mStatistics[id] = std::move(linePtr);
    ++id;

    linePtr = Ptr(new GfxStatisticsLine(mFonts, Settings::ShowStatisticsBPM, static_cast<GfxStatisticsLine::StatisticsID>(id)));
    mStatistics[id] = std::move(linePtr);
    ++id;
}

void Application::buildButtons()
{
    auto logKeyQueue = ConfigHelper::oldGetLogKeys();
    auto logKeyBtnsQueue = ConfigHelper::oldGetLogButtons();

    while (logKeyBtnsQueue.size())
    {
        logKeyQueue.push(logKeyBtnsQueue.front());
        logKeyBtnsQueue.pop();
    }
    if (logKeyQueue.empty())
        logKeyQueue = ConfigHelper::getLogKeys();

    for (auto i = 0ul; !logKeyQueue.empty(); ++i)
    {
        addButton(logKeyQueue.front());
        logKeyQueue.pop();
    }
    
    // TODO remove *new LogKey, use smart ptrs instead
    if (mButtons.empty())
    {
        addButton(*new LogKey("Z", "Z", new sf::Keyboard::Key(sf::Keyboard::Z), nullptr));
        addButton(*new LogKey("X", "X", new sf::Keyboard::Key(sf::Keyboard::X), nullptr));
    }
}

bool Application::isPressPerformedOnButton(unsigned &btnIdx) const
{
    const auto size = Button::size();
    for (auto i = 0ul; i < size; ++i)
    {
        if (isMouseInRange(i))
        {
            btnIdx = i;
            return true;
        }
    }
    return false;
}

bool Application::isMouseInRange(unsigned idx) const
{
    const auto mousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(mWindow));
    const auto textureSize = static_cast<sf::Vector2f>(Settings::GfxButtonTextureSize);
    const auto &button = *mButtons[idx];
    const auto buttonPosition = button.getPosition() - textureSize / 2.f;
    const auto buttonRectangle = sf::FloatRect(buttonPosition, textureSize);

    return buttonRectangle.contains(mousePosition);
}

void Application::addButton(LogKey &logKey)
{
    mButtons.emplace_back(std::make_unique<Button>(logKey, mTextures, mFonts));
}

void Application::removeButton()
{
    mButtons.pop_back();
}

void Application::openWindow()
{
    sf::Uint32 style;
#ifdef _WIN32
    style = Settings::WindowTitleBar ? sf::Style::Close : sf::Style::None;
#elif linux
    style = Settings::WindowTitleBar ? sf::Style::Default : sf::Style::None;
#else
#error Unsupported compiler
#endif

    if (mWindow.isOpen())
        mWindow.close();
    mWindow.create(sf::VideoMode(getWindowWidth(), getWindowHeight()), "JKPS", style);
    mWindow.setKeyRepeatEnabled(false);
    mWindow.setFramerateLimit(getRenderUpdateFrequency());
#ifdef linux
    if (style == sf::Style::None)
    {
        auto desktop = sf::VideoMode::getDesktopMode();
        auto windowSize = static_cast<sf::Vector2i>(mWindow.getSize());
        mWindow.setPosition(sf::Vector2i(
            desktop.width  / 2 - windowSize.x / 2, 
            desktop.height / 2 - windowSize.y / 2));
    }
#endif
}

void Application::resizeWindow()
{
    const auto size = sf::Vector2u(getWindowWidth(), getWindowHeight());
    mWindow.setSize(size);

    auto windowSize = static_cast<sf::Vector2f>(mWindow.getSize());
    auto view = sf::View(sf::FloatRect(0, 0, windowSize.x, windowSize.y));
    mWindow.setView(view);
}

void Application::moveWindow()
{
    static auto mLastMousePosition = sf::Vector2i();
    if (mWindow.hasFocus() && sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        mWindow.setPosition(mWindow.getPosition() + 
            sf::Mouse::getPosition() - mLastMousePosition);
    }
    mLastMousePosition = sf::Mouse::getPosition();
}

unsigned Application::getWindowWidth()
{
    const auto btnAmt = static_cast<int>(Button::size());
    const auto width = static_cast<int>(
        Settings::GfxButtonTextureSize.x * btnAmt + 
        (btnAmt - 1) * Settings::GfxButtonDistance + 
        Settings::WindowBonusSizeLeft + Settings::WindowBonusSizeRight);
    
    return std::max(5, width);
}

unsigned Application::getWindowHeight()
{
    const auto height = static_cast<int>(
        Settings::GfxButtonTextureSize.y + Settings::WindowBonusSizeTop + 
        Settings::WindowBonusSizeBottom);
    
    return std::max(5, height);
}

sf::IntRect Application::getWindowRect()
{
    return { { }, sf::Vector2i(getWindowWidth(), getWindowHeight()) };
}

bool Application::parameterIdMatches(LogicalParameter::ID id)
{
    return
        id == LogicalParameter::ID::BtnGfxTxtrSz ||  
        id == LogicalParameter::ID::BtnTextChSz ||
        id == LogicalParameter::ID::BtnGfxDist  ||
        id == LogicalParameter::ID::MainWndwTop ||
        id == LogicalParameter::ID::MainWndwBot ||
        id == LogicalParameter::ID::MainWndwLft ||
        id == LogicalParameter::ID::MainWndwRght;
}

int Application::getRenderUpdateFrequency() const
{
	return Settings::RenderUpdateFrequency;
}
