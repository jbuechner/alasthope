#pragma once

#include <mutex>
#include <unordered_map>

#include "alasthope/path_utils.h"
#include "alasthope/engine/aloo/font.h"
#include "alasthope/engine/aloo/font_manager.h"

namespace engine
{
	namespace aloo
	{
		namespace details
		{
			class font_manager_impl : public font_manager
			{
			private:
				class font_item_cache
				{
				public:
					font_item_cache(std::filesystem::path const& path)
						: _path{ path }
					{
					}

					font_item_cache(font_item_cache&& other)
						: _path{ std::move(other._path) }, _cache{ std::move(other._cache) }
					{
					}

					std::shared_ptr<font> const get_font(uint16_t const& sizeInPixel)
					{
						std::lock_guard<std::mutex> lock{ _mtx };

						auto const iter = _cache.find(sizeInPixel);
						if (iter != _cache.cend())
						{
							auto font = iter->second.lock();
							if (font)
							{
								return font;
							}
						}

						// could be optimised by risking a construction race inside an unlocked region,
						// should be faste because IO is the slowest part inside the lock, but on the other hand
						// allegro has to allocate the texture multiple times inside the construction race ...

						auto font = create_font(_path, sizeInPixel);
						_cache.insert_or_assign(sizeInPixel, font);
						return font;
					}
				private:
					std::filesystem::path const _path;
					std::mutex _mtx{};
					std::unordered_map<uint16_t, std::weak_ptr<font>> _cache{};
				};

				std::shared_ptr<font> const get_font_internal(std::filesystem::path const& path, uint16_t const& sizeInPixel)
				{
					std::lock_guard<std::mutex> lock{ _mtx };

					auto localPath = make_local_path(path);

					if (localPath.empty())
					{
						throw std::exception{ "file is outside of the application directory." };
					}

					std::string key{ localPath.filename().u8string() };
					auto const iter = _cache.find(key);
					if (iter != _cache.cend())
					{
						auto& item_cache = iter->second;
						return item_cache->get_font(sizeInPixel);
					}

					auto entry = _cache.insert_or_assign(key, std::make_unique<font_item_cache>(localPath));
					auto& item_cache = entry.first->second;
					return item_cache->get_font(sizeInPixel);
				}

				std::mutex _mtx{};
				std::unordered_map<std::string, std::unique_ptr<font_item_cache>> _cache{};
			};
		}
	}
}