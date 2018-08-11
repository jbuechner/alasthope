#include "pch_allegro.h"
#pragma hdrstop

#include <unordered_map>
#include <vector>

#include "alasthope/engine/aloo/draw_context.h"
#include "alasthope/engine/aloo/render_texture.h"
#include "alasthope/engine/terrain_info.h"
#include "alasthope/engine/structure_info.h"
#include "alasthope/engine/tile_info.h"
#include "alasthope/engine/rectangular_grid.h"
#include "alasthope/engine/drawables/sprite.h"

#include "terrain_sprite_info.h"
#include "structure_sprite_info.h"

#include "grid_options.h"
#include "grid.h"

namespace
{
	using namespace engine;
	using namespace engine::aloo;
	using namespace engine::drawables;

	class tile_info_sprite : public engine::aloo::drawable
	{
	public:
		tile_info_sprite()
			: _terrain_sprite_info{ get_default_terrain_sprite_info() }, _structure_sprite_info{ get_default_structure_sprite_info() }
		{
		}

		void set_tile_info(std::shared_ptr<tile_info> const& tile_info)
		{
			_terrain_sprite_info = lookup_terrain_sprite_info(tile_info->terrain().id, tile_info->terrain_variation());

			auto& structure = tile_info->structure();
			_structure_sprite_info = lookup_structure_sprite_info(structure.id);
			_render_structure = structure.id > 0;

			update_source_regions();

			auto& coordinate = tile_info->coordinate();
			_position = { coordinate.x * _size.x, coordinate.y * _size.y };
		}

		void set_spritesheet(std::shared_ptr<render_texture const> const& spritesheet)
		{
			_spritesheet = spritesheet;
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
			auto* const bitmap_ptr{ reinterpret_cast<ALLEGRO_BITMAP*>(_spritesheet->get_native_ptr()) };
			al_draw_bitmap_region(bitmap_ptr, _terrain_source_region.x, _terrain_source_region.y, _terrain_source_region.z, _terrain_source_region.w, _position.x, _position.y, 0);
			if (_render_structure)
			{
				al_draw_bitmap_region(bitmap_ptr, _structure_source_region.x, _structure_source_region.y, _structure_source_region.z, _structure_source_region.w, _position.x, _position.y, 0);
			}
		}

		inline void update_source_regions()
		{
			auto& terrain_sprite_info = _terrain_sprite_info.get();
			_terrain_source_region = { terrain_sprite_info.source.x, terrain_sprite_info.source.y, _size.x, _size.y };

			auto& structure_sprite_info = _structure_sprite_info.get();
			_structure_source_region = { structure_sprite_info.source, _size.x, _size.y };
		}

		std::reference_wrapper<terrain_sprite_info const> _terrain_sprite_info;
		std::reference_wrapper<structure_sprite_info const> _structure_sprite_info;

		bool _is_dirty{ false };
		bool _render_structure{ false };
		glm::uvec2 _size{};
		glm::uvec4 _terrain_source_region{};
		glm::uvec4 _structure_source_region{};
		glm::ivec2 _position{};
		std::shared_ptr<render_texture const> _spritesheet{ nullptr };
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
				if (tile_coordinate.x >= 0 && tile_coordinate.y >= 0 && tile_coordinate.x < static_cast<glm::ivec2::value_type>(_size.x) && tile_coordinate.y < static_cast<glm::ivec2::value_type>(_size.y))
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