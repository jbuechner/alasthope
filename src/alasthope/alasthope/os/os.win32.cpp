#pragma hdrstop

#include "os.h"

#include <tuple>
#include <array>

#include <windows.h>

#include <nowide/convert.hpp>

namespace
{
	template <size_t buffer_size>
	std::tuple<bool, std::wstring> get_module_file_name()
	{
		HMODULE handle = GetModuleHandle(nullptr);
		_ASSERT(handle);
		if (!handle)
		{
			throw std::exception{ "unable to retrieve process module handle." };
		}

		std::array<std::wstring::value_type, buffer_size> buffer{};
		DWORD byteCount = GetModuleFileName(handle, &buffer[0], static_cast<DWORD>(buffer.size()));
		if (byteCount < 0)
		{
			throw std::exception{ "unable to retrieve module file name." };
		}


		bool sufficientBufferSpaceAvailable{ GetLastError() != ERROR_INSUFFICIENT_BUFFER };
		return std::make_tuple(sufficientBufferSpaceAvailable, std::wstring{ buffer.cbegin(), buffer.cend() });
	}
}

namespace os
{
	std::filesystem::path get_process_module_path()
	{
		std::wstring u16ModuleFileName;
		bool sufficientBufferSpaceAvailable;

		std::tie(sufficientBufferSpaceAvailable, u16ModuleFileName) = get_module_file_name<MAX_PATH>();
		if (!sufficientBufferSpaceAvailable)
		{
			std::tie(sufficientBufferSpaceAvailable, u16ModuleFileName) = get_module_file_name<std::numeric_limits<int16_t>::max()>();
		}

		auto u8ModuleFileName = nowide::narrow(u16ModuleFileName);
		u16ModuleFileName.clear();

		std::filesystem::path moduleFilePath{ u8ModuleFileName };
		return moduleFilePath.parent_path();
	}
}