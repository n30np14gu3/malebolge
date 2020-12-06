#pragma once
#include <memory>

class Hooks
{
public:
	Hooks(HMODULE _module);
};

inline std::unique_ptr<Hooks> hooks;