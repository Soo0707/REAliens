#include "assetManager.hpp"

#include <map>
#include <filesystem>
#include <string>
#include <algorithm>

#include "raylib.h"
#include "tmx.h"

AssetManager::AssetManager()
{
	AssetManager::LoadStaticTextures();
	AssetManager::LoadEntityTextures();
	AssetManager::LoadMapTextures();

	this->Ground = LoadTexture("assets/map/ground.png");
}

AssetManager::~AssetManager()
{
	AssetManager::UnloadStaticTextures();
	AssetManager::UnloadEntityTextures();
	AssetManager::UnloadMapTextures();
}

void AssetManager::LoadStaticTextures()
{
	std::filesystem::path path = "assets/static_textures";

	if (!std::filesystem::exists(path))
		exit(1);

	for (auto const &item : std::filesystem::directory_iterator(path))
	{
		StaticTextureKey CurrentTexture = GetStaticTextureKeyFromString(std::filesystem::relative(item, path).string());

		if (CurrentTexture != StaticTextureKey::None)
		{
			this->StaticTextures[CurrentTexture] = LoadTexture(item.path().string().c_str());
		}
	}
}

void AssetManager::UnloadStaticTextures()
{
	for (auto const &pair : this->StaticTextures)
	{
		UnloadTexture(this->StaticTextures[pair.first]);
	}
}


StaticTextureKey AssetManager::GetStaticTextureKeyFromString(std::string filename)
{
	static const std::map<std::string, StaticTextureKey> Lookup =
	{
		{ "circle.png", StaticTextureKey::Circle },
		{ "lazer.png", StaticTextureKey::Lazer },
		{ "projectile.png", StaticTextureKey::Projectile },
		{ "xp.png", StaticTextureKey::Xp }
	};

	return (Lookup.count(filename)) ? Lookup.at(filename) : StaticTextureKey::None;
}

void AssetManager::LoadEntityTextures()
{
	std::filesystem::path base_path = "assets/entity_textures";
	
	if (!std::filesystem::exists(base_path))
		exit(1);

	for (auto const &directory : std::filesystem::directory_iterator(base_path))
	{
		EntityTextureKey Entity = GetEntityTextureKeyFromString(
				std::filesystem::relative(directory, base_path).string()
		);

		if (Entity != EntityTextureKey::None)
		{
			this->EntityTextures[Entity] = std::vector<Texture2D>{};

			std::vector<std::filesystem::path> tmp;
			
			for (auto const &item : std::filesystem::directory_iterator(directory))
			{
				tmp.push_back(item.path());
			}
			
			std::sort(tmp.begin(), tmp.end());
			
			for (auto const &item : tmp)
			{
				this->EntityTextures[Entity].push_back(LoadTexture(item.string().c_str()));
			}
		}
	}
}

void AssetManager::UnloadEntityTextures()
{
	for (auto const &pair : this->EntityTextures)
	{
		for (auto const &item : this->EntityTextures[pair.first])
		{
			UnloadTexture(item);
		}
	}
}

EntityTextureKey AssetManager::GetEntityTextureKeyFromString(std::string folder_name)
{
	static const std::map<std::string, EntityTextureKey> Lookup =
	{
		{ "player_north", EntityTextureKey::PlayerNorth },
		{ "player_south", EntityTextureKey::PlayerSouth },
		{ "player_east", EntityTextureKey::PlayerEast },
		{ "player_west", EntityTextureKey::PlayerWest },
		{ "australian", EntityTextureKey::Australian },
		{ "big_man", EntityTextureKey::BigMan },
		{ "beer", EntityTextureKey::Beer },
		{ "bomber", EntityTextureKey::Bomber },
		{ "bomber_explosion", EntityTextureKey::BomberExplosion },
		{ "drunkard", EntityTextureKey::Drunkard },
		{ "pleb", EntityTextureKey::Pleb },
		{ "poison", EntityTextureKey::Poison},
		{ "trapper", EntityTextureKey::Trapper}
	};

	return (Lookup.count(folder_name)) ? Lookup.at(folder_name) : EntityTextureKey::None;
}

void AssetManager::LoadMapTextures()
{
	tmx_map *map = tmx_load("assets/map/map.tmx");
	Image tileset = LoadImage("assets/map/unified_tileset.png");
	
	if (map == nullptr)
		exit(1);

	tmx_layer *walls_layer = tmx_find_layer_by_name(map, "Walls");
	//tmx_layer *props_layer = tmx_find_layer_by_name(map, "Props");
	//tmx_layer *spawners_layer = tmx_find_layer_by_name(map, "Spawners");

	if (walls_layer == nullptr ) //|| props_layer == nullptr || spawners_layer == nullptr) 
		exit(1);

	if (map->ts_head == nullptr)
		exit(1);

	AssetManager::LoadMapTexturesHelper(map, walls_layer, &tileset);
	//AssetManager::LoadMapTexturesHelper(map, props_layer, &tileset);
	//AssetManager::LoadMapTexturesHelper(map, spawners_layer, &tileset);

	tmx_map_free(map);
	UnloadImage(tileset);
}

void AssetManager::UnloadMapTextures()
{
	for (auto const &pair : this->MapTextures)
	{
		UnloadTexture(this->MapTextures[pair.first]);
	}
}

void AssetManager::LoadMapTexturesHelper(tmx_map* map, tmx_layer* layer, Image* tileset)
{
	int32_t firstGID = map->ts_head->firstgid;
	int tileset_columns = tileset->width / TILESIZE;

	for (int cell_y = 0; cell_y < map->height; cell_y++)
	{
		for (int cell_x = 0; cell_x < map->width; cell_x++)
		{
			int32_t cell = layer->content.gids[cell_y * map->width + cell_x];
			int32_t GID = cell & TMX_FLIP_BITS_REMOVAL;

			if (GID == 0)
				continue;

			int32_t horizontal_flip = cell & TMX_FLIPPED_HORIZONTALLY;
			int32_t vertical_flip = cell & TMX_FLIPPED_VERTICALLY;
			int32_t diagonal_flip = cell & TMX_FLIPPED_DIAGONALLY;

			if (this->MapTextures.count(cell))
				continue;

			int x = ((GID - firstGID) % tileset_columns) * TILESIZE;
			int y = ((GID - firstGID) / tileset_columns) * TILESIZE;

			Image tile_texture = ImageFromImage(*tileset, {(float) x, (float) y, TILESIZE, TILESIZE});

			if (diagonal_flip)
			{
				ImageRotateCW(&tile_texture);
				ImageFlipHorizontal(&tile_texture);
			}

			if (horizontal_flip)
				ImageFlipHorizontal(&tile_texture);

			if (vertical_flip)
				ImageFlipVertical(&tile_texture);

			this->MapTextures[cell] = LoadTextureFromImage(tile_texture);
		}
	}
}
