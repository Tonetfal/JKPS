#pragma once

#include <map>
#include <string>
#include <memory>
#include <stdexcept>
#include <cassert>


template <typename Resource, typename Identifier>
class ResourceHolder
{
	public:
		void loadFromFile(Identifier id, const std::string &filename);
		void loadFromStream(Identifier id, const void *data, std::size_t size);

		template <typename Parameter>
		void loadFromFile(Identifier id, const std::string &filename, const Parameter &secondParam);

		Resource& get(Identifier id);
		const Resource &get(Identifier id) const;


	private:
		void insertResource(Identifier id, std::unique_ptr<Resource> resource);


	private:
		std::map<Identifier, std::unique_ptr<Resource>>	mResourceMap;
};

#include "ResourceHolder.inl"