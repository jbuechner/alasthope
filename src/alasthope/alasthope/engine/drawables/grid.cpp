#include "pch_allegro.h"
#pragma hdrstop

#include <unordered_map>
#include <vector>

#include "alasthope/engine/aloo/draw_context.h"
#include "alasthope/engine/aloo/render_texture.h"
#include "alasthope/engine/terrain_info.h"
#include "alasthope/engine/tile_info.h"
#include "alasthope/engine/rectangular_grid.h"
#include "alasthope/engine/drawables/sprite.h"

#include "grid_options.h"
#include "grid.h"

namespace
{
	using namespace engine;
	using namespace engine::aloo;
	using namespace engine::drawables;

	class terrain_sprite_info
	{
	public:
		terrain_sprite_info()
			: source{}
		{
		}

		terrain_sprite_info(glm::uvec2 const& source_)
			: source{ source_ }
		{
		}

		glm::uvec2 const source;
	};

	using terrain_sprite_info_variation_list_type = std::vector<terrain_sprite_info>;

	class terrain_sprite_info_list
	{
	public:
		terrain_sprite_info_list(terrain_sprite_info_variation_list_type const& variations_)
			: variations { std::move(variations_) }
		{
		}

		terrain_sprite_info_variation_list_type const variations;
	};

	terrain_sprite_info_list _none_sprite_info{ std::move<terrain_sprite_info_variation_list_type>({ { { 0, 0 } } }) };
	terrain_sprite_info_list _sand_sprite_info{ std::move<terrain_sprite_info_variation_list_type>({ { { 0, 32 } }, { { 0, 64 } }, { { 0, 96 } }, { { 0, 128 } } }) };

	using terrain_sprite_info_cache_type = std::unordered_map<size_t, terrain_sprite_info_list>;

	terrain_sprite_info_cache_type initialize_terrain_sprite_info_cache()
	{
		terrain_sprite_info_cache_type cache{};

		cache.emplace(0, _none_sprite_info);
		cache.emplace(1, _sand_sprite_info);

		return std::move(cache);
	}

	terrain_sprite_info const& lookup_terrain_sprite_info(size_t const& id, size_t const& variation)
	{
		static terrain_sprite_info_cache_type cache{ std::move(initialize_terrain_sprite_info_cache()) };

		auto const iter = cache.find(id);
		if (iter != cache.cend())
		{
			auto& list = iter->second;
			auto const size = list.variations.size();
			return list.variations.at(std::max(static_cast<size_t>(0), std::min(std::max(static_cast<size_t>(0), size - 1), variation)));
		}
		return _none_sprite_info.variations.at(0);
	}

	class tile_info_sprite : public engine::aloo::drawable
	{
	public:
		tile_info_sprite()
			: _terrain_sprite_info{ _none_sprite_info.variations.at(0) }
		{
		}

		void set_tile_info(std::shared_ptr<tile_info> const& tile_info)
		{
			_tile_info = tile_info;
			_terrain_sprite_info = lookup_terrain_sprite_info(tile_info->terrain().id, tile_info->terrain_variation());
			update_source_region();

			auto& coordinate = _tile_info->coordinate();
			_sprite->set_position({ coordinate.x * _size.x, coordinate.y * _size.y });
		}

		void set_spritesheet(std::shared_ptr<render_texture const> const& spritesheet)
		{
			_sprite->set_render_texture(spritesheet);
		}

		void set_size(glm::uvec2 const& size)
		{
			_size = size;
		}
	private:
		bool draw_requested_internal(draw_context& context) const override
		{
			return true;
		}

		void draw_internal(draw_context& context) override
		{
			_sprite->draw(context);
		}

		inline void update_source_region()
		{
			auto& sprite_info = _terrain_sprite_info.get();
			_sprite->set_source_region({ sprite_info.source.x, sprite_info.source.y, _size.x, _size.y });
		}

		std::reference_wrapper<terrain_sprite_info const> _terrain_sprite_info;
		bool _is_dirty{ false };
		glm::uvec2 _size{};
		std::shared_ptr<tile_info const> _tile_info{ nullptr };
		std::shared_ptr<sprite> _sprite{ create_sprite() };
	};

	class grid_internal : public grid
	{
	public:
		grid_internal(grid_options const& options, std::shared_ptr<rectangular_grid const> const& grid)
			: _grid{ grid }, _size{ grid->size() }, _spritesheet{ options.spritesheet }, _tile_size{ options.tile_size }
		{
			_sprites.resize(_size.x * _size.y);
			fill();
			_texture = create_render_texture(_size.x * _tile_size.x, _size.y * _tile_size.y, render_texture_flags::video, render_texture_format::best);
			_cursor->set_render_texture(_spritesheet);
			_cursor->set_source_region({ 32, 0, _tile_size });
			_cursor->set_visible(false);
		}
	private:
		void draw_internal(draw_context& context) override
		{
			bool draw { _is_dirty };

			if (draw)
			{
				auto render_target_guard{ std::move(_texture->make_render_target()) };

				for (auto& sprite : _sprites)
				{
					sprite.draw(context);
				}
			}

			al_draw_bitmap(reinterpret_cast<ALLEGRO_BITMAP*>(_texture->get_native_ptr()), _position.x, _position.y, 0);
			_cursor->draw(context);

			_is_dirty = false;
		}

		void set_position_internal(glm::ivec2 const& position) override
		{
			glm::uvec2 total_size{ _size.x * _tile_size.x, _size.y * _tile_size.y };
			_position = position - static_cast<glm::ivec2>((total_size / glm::uvec2{ 2, 2 }));
		}

		virtual void set_cursor_visible_internal(bool const& value) const override
		{
			_cursor->set_visible(value);
		}

		virtual void set_cursor_position_internal(glm::uvec2 const& tile_coordinate) const override
		{
			_cursor->set_position(static_cast<glm::ivec2>(tile_coordinate * _tile_size) + _position);
		}

		std::optional<glm::uvec2 const> get_coordinate_at_mouse_position_internal(glm::uvec2 const& position) const override
		{
			auto relativePosition = static_cast<glm::ivec2>(position) - _position;

			if (relativePosition.x > 0 && relativePosition.y > 0)
			{
				glm::ivec2 const tile_coordinate{ static_cast<glm::ivec2::value_type>(relativePosition.x / _tile_size.x), static_cast<glm::ivec2::value_type>(relativePosition.y / _tile_size.y) };
				if (tile_coordinate.x >= 0 && tile_coordinate.y >= 0 && tile_coordinate.x < _size.x && tile_coordinate.y < _size.y)
				{
					return tile_coordinate;
				}
			}

			return {};
		}

		inline tile_info_sprite& lookup_ref(glm::uvec2 const& coordinate)
		{
			return _sprites[coordinate.x + coordinate.y * _size.x];
		}

		void fill()
		{
			for (glm::uvec2::value_type x = 0; x < _size.x; x++)
			{
				for (glm::uvec2::value_type y = 0; y < _size.y; y++)
				{
					glm::uvec2 coordinate{ x, y };
					auto& sprite{ lookup_ref(coordinate) };
					sprite.set_size(_tile_size);
					sprite.set_tile_info(_grid->lookup(coordinate));
					sprite.set_spritesheet(_spritesheet);
				}
			}
		}

		bool _is_dirty{ true };
		std::vector<tile_info_sprite> _sprites{};
		glm::uvec2 const _size;
		glm::uvec2 const _tile_size;
		glm::ivec2 _position{};
		std::shared_ptr<sprite> const _cursor{ std::move(create_sprite()) };
		std::shared_ptr<rectangular_grid const> const _grid;
		std::shared_ptr<render_texture const> const _spritesheet;
		std::shared_ptr<render_texture> _texture{ nullptr };
	};
}

namespace engine
{
	namespace drawables
	{
		std::shared_ptr<grid> create_grid(grid_options const& options, std::shared_ptr<rectangular_grid const> const& grid)
		{
			return std::make_shared<grid_internal>(options, grid);
		}
	}
}