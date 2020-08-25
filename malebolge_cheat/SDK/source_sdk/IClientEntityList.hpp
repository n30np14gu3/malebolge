#pragma once
#include "../VirtualMethod.hpp"

class Entity;

class IClientEntityList {
public:
    VIRTUAL_METHOD(Entity*, getEntity, 3, (int index), (this, index))
    VIRTUAL_METHOD(Entity*, getEntityFromHandle, 4, (int handle), (this, handle))
	VIRTUAL_METHOD(int, getHighestEntityIndex, 6, (), (this))
};