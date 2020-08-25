#pragma once
#include <memory>

class Hooks
{
public:
	Hooks(HMODULE module);
};

inline std::unique_ptr<Hooks> hooks;