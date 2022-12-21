#pragma once
#include <string_view>

class StringHelper
{
public:
	static std::string TrimStart(std::string_view str);

	static std::string TrimEnd(std::string_view str);

	static std::string Trim(std::string_view str);
};
