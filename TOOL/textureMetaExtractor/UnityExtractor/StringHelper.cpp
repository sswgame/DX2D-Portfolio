#include "pch.h"
#include "StringHelper.h"

namespace
{
	constexpr std::string_view WHITESPACE = " \n\r\t\f\v";
}

std::string StringHelper::TrimStart(const std::string_view str)
{
	const size_t start = str.find_first_not_of(WHITESPACE);
	return (start == std::string::npos) ? std::string{} : str.substr(start).data();
}

std::string StringHelper::TrimEnd(const std::string_view str)
{
	const size_t end = str.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? std::string{} : str.substr(0, end + 1).data();
}

std::string StringHelper::Trim(const std::string_view str)
{
	return TrimEnd(TrimStart(str));
}
