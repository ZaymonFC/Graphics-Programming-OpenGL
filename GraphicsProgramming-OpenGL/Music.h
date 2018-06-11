#pragma once
#include <string>

class Music
{
public:
	std::string path;
	Music(std::string fileName);
	auto Play() -> void;
};

