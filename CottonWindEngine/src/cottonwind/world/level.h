#pragma once

#include <unordered_map>
#include <sstream>

#include "../vendor/glm/glm.hpp"

#include "../util/file.h"
#include "../util/logger.h"

#include "../graphics/texture.h"


namespace cotwin
{
	struct Tilemap
	{
		Texture tileset;
		std::string texture_table_str;
		std::string tiles_str;
		glm::ivec2 tiles_count;

		Tilemap()
		{}
		
		Tilemap(
			Texture s_tileset,
			std::string s_texture_table_str,
			std::string s_tiles_str,
			const glm::ivec2& s_tiles_count
		)
			: tileset(s_tileset), texture_table_str(s_texture_table_str), tiles_str(s_tiles_str), tiles_count(s_tiles_count)
		{}
	};

	struct CollisionMap
	{
		std::string map;

		CollisionMap()
		{}
		
		CollisionMap(std::string collision_map)
			: map(collision_map)
		{}
	};
	
	struct Level
	{
	private:
		Tilemap tilemap;
		CollisionMap collision_map;

	public:
		Level()
		{}
		
		Level(
			Texture s_tileset,
			std::string texture_table_str,
			std::string tiles_str,
			const glm::ivec2& tiles_count,
			int tile_size,
			std::string map
		)
			: tilemap(s_tileset, texture_table_str, tiles_str, tiles_count), collision_map(map)
		{}

		Tilemap get_tilemap()
		{
			return tilemap;
		}

		CollisionMap get_collision_map()
		{
			return collision_map;
		}

		friend class LevelManager;
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

			const char* tileset_filepath = tokens[0].c_str();
			int tilemap_width = std::stoi(tokens[1]);
			int tilemap_height = std::stoi(tokens[2]);

			Texture& tileset = TextureManager::get_texture(tileset_filepath);
			
			if (!tilemap_width || !tilemap_height)
			{
				Logger::Error("Failed to load level %s, provided tilemap dimensions are not valid", filepath);
				Level error_level;
				return error_level;
			}

			if ((tilemap_height >= (tokens.size() - 3)) || (tokens[3].length() != tilemap_width))
			{
				Logger::Error("Failed to load level %s, the provided tiles string doesn't correspond to the dimensions", filepath);
				Level error_level;
				return error_level;
			}

			std::string tiles_str;
			tiles_str.reserve(tilemap_width * tilemap_height);
			tiles_str = "";
			
			int i;
			for (i = 3; i < tilemap_height + 3; i++)
			{
				tiles_str += tokens[i];
			}

			Level level;

			if (tokens[i] == "collision")
			{
				i++;
				
				if (((tilemap_height) > (tokens.size() - 3 - tilemap_height)) || (tokens[i].length() != tilemap_width))
				{
					Logger::Error("Failed to load level %s, the provided collision map string doesn't correspond to the dimensions", filepath);
					Level error_level;
					return error_level;
				}

				std::string collision_str;
				collision_str.reserve(tilemap_width * tilemap_height);
				collision_str = "";

				for (; i < tilemap_height * 2 + 4; i++)
				{
					collision_str += tokens[i];
				}

				level.collision_map = CollisionMap(collision_str.c_str());
			}
			
			std::string texture_table;
			//texture_table.reserve(tokens.size() - i);
			texture_table = "";
			for (; i < tokens.size(); i++)
				// TODO
				texture_table += tokens[i] + "\n";

			level.tilemap = Tilemap(tileset, texture_table, tiles_str, { tilemap_width, tilemap_height });
			
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