#ifndef _TEXTURE_CACHE_H_
#define _TEXTURE_CACHE_H_

#include "Mesh.h"

#include "unordered_map"

class TextureCache {
public:
	static TextureCache& getInstance() {
		static TextureCache texCache;
		return texCache;
	}

	static std::unordered_map<std::string, Texture>& getCache() {
		static std::unordered_map<std::string, Texture> cache;
		return cache;
	}

private:

	TextureCache() = default;

};

#endif