#pragma once

#include "SceneNode.hpp"

#include <SFML/Graphics/Sprite.hpp>


class SpriteNode : public SceneNode
{
	public:
		SpriteNode(const sf::Texture& texture);
		SpriteNode(const sf::Texture& texture, const sf::IntRect& textureRect);


	private:
		virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;


	private:
		sf::Sprite mSprite;
};