#include "../Headers/User.hpp"

struct ButtonPresser
{
	ButtonPresser(KeyCombination keys)
	: mKeys(keys) { }

	void operator() (Button &button, sf::Time) const
	{
		if (mKeys == button.getKey())
            button.pressButton();
	}

	KeyCombination mKeys;
};

User::User()
{
	// Set initial key bindings
	// mKeyBinding[Settings::CombinationForEditMode] = EditMode;
	// mKeyBinding[Settings::CombinationToIncrease] = IncreaseKeys;
	// mKeyBinding[Settings::CombinationToDecrease] = DecreaseKeys;
	mKeyBinding[Settings::CombinationToClear] = ResetStatistics;
    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
        mKeyBinding[Settings::mKeys[i]] = static_cast<Action>(i + Button0);

	// Set initial action bindings
	initializeActions();

	// Assign all categories
	// mActionBinding.find(EditMode)->second.category = Category::EditMode;
    // mActionBinding.find(IncreaseKeys)->second.category = Category::Button;
    // mActionBinding.find(DecreaseKeys)->second.category = Category::Button;
	mActionBinding.find(ResetStatistics)->second.category = Category::Statistics;
    for (size_t i = Button0; i <= Button9; ++i)
	{
        mActionBinding.find(static_cast<Action>(i))->second.category = 
			Category::Button ;
	}
}

void User::handleEvent(const sf::Event& event, CommandQueue& commands)
{
	if (event.type == sf::Event::KeyPressed)
	{
		Keys key;
		if (event.type == sf::Event::KeyPressed)
		{
			key = static_cast<Keys>(event.key.code);
		}
		else
		{
			key = static_cast<Keys>(event.mouseButton.button);
		}
		// Check if pressed key appears in key binding, trigger command if so
		auto found = mKeyBinding.find(key);
		if (found != mKeyBinding.end())
			commands.push(mActionBinding[found->second]);
	}
}

void User::handleRealtimeInput(CommandQueue& commands)
{
	// Traverse all assigned keys and check if they are pressed
	for(auto pair : mKeyBinding)
	{
		// If key is pressed, lookup action and trigger corresponding command
		if (KeyCombination::isCombinationPressed(pair.first))
			commands.push(mActionBinding[pair.second]);
	}
}

// sf::Keyboard::Key User::getAssignedKey(Action action) const
// {
// 	for(auto pair : mKeyBinding)
// 	{
// 		if (pair.second == action)
// 			return pair.first;
// 	}

// 	return sf::Keyboard::Unknown;
// }

void User::initializeActions()
{
	// mActionBinding[ResetStatistics].action = 
    for (size_t i = Button0; i <= Button9; ++i)
    {
        Action action = static_cast<Action>(i);
        auto &actionPair = mActionBinding[action];
        
        for (auto &bindPair : mKeyBinding)
		{
            if (bindPair.second == action)
                actionPair.action = derivedAction<Button>(ButtonPresser(bindPair.first));
		}
    }
}