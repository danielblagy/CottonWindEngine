#pragma once

#include <unordered_map>
#include <sstream>

#include "../vendor/glm/glm.hpp"

#include "../util/file.h"
#include "../util/logger.h"


namespace cotwin
{
	struct Tilemap
	{
		const char* texture_table_str;
		const char* tiles_str;
		glm::ivec2 tiles_count;
		int tile_size;

		Tilemap(
			const char* s_texture_table_str,
			const char* s_tiles_str,
			const glm::ivec2& s_tiles_count,
			int s_tile_size
		)
			: texture_table_str(s_texture_table_str), tiles_str(s_tiles_str), tiles_count(s_tiles_count), tile_size(s_tile_size)
		{}
	};

	struct CollisionMap
	{
		const char* map;

		CollisionMap(const char* collision_map)
			: map(collision_map)
		{}
	};
	
	class Level
	{
		Tilemap tilemap;
		CollisionMap collision_map;
	};
	
	class LevelManager
	{
	private:
		std::unordered_map<std::string, Level> level_resources;

	public:
		static LevelManager& get_instance()
		{
			static LevelManager instance;
			return instance;
		}

		static Level& get_level(const char* filepath)
		{
			return get_instance().get_level_internal(filepath);
		}

	private:
		Level& load_level(const char* filepath)
		{
			std::vector<std::string> tokens;
			std::istringstream token_stream(read_file(filepath));
			std::string token;
			while (std::getline(token_stream, token, '\n'))
			{
				tokens.push_back(token);
			}
			
			

			level_resources[std::string(filepath)] = level;
			return level_resources[filepath];
		}

		Level& get_level_internal(const char* filepath)
		{
			if (level_resources.count(filepath))
			{
				return level_resources[filepath];
			}
			else
			{
				return load_level(filepath);
			}
		}

		LevelManager()
		{}

		~LevelManager()
		{}

		// explicitly disallow copying
		LevelManager(const LevelManager&) = delete;
		LevelManager& operator= (const LevelManager&) = delete;
	};
}