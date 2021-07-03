#pragma once


// Entity/scene node category, used to dispatch commands
namespace Category
{
	enum Type
	{
		None 		= 0,
		Scene 		= 1 << 0,
		Button 		= 1 << 1,
		Statistics 	= 1 << 2,
		EditMode 	= 1 << 3,
		Calculator  = 1 << 4,
	};
}