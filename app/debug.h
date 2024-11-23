#pragma once

#include <boost/signals2.hpp>
#include <imgui.h>

class Debug
{
	public:
	boost::signals2::signal<void()> buildUI{};
};