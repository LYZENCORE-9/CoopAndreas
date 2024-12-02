#pragma once

#include <filesystem>
#include <iostream>
#include <iterator>
#include <system_error>
#include <vector>
#include <cstdlib>
#include <filesystem>

#include "../thirdparty-libraries/iniparser.hpp"
#include "../thirdparty-libraries/mini.h"

namespace ConfigDatabase
{
	inline unsigned short configport;

	void Init(const std::string &filename);
	bool Create(const std::string &filename);
	bool Load(const std::string &filename);
}

