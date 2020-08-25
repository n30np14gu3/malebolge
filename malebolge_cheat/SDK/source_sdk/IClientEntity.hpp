#pragma once
#include "../Vector.hpp"
#include "../VirtualMethod.hpp"

class Collideable {
public:
    VIRTUAL_METHOD(const Vector&, obbMins, 1, (), (this))
	VIRTUAL_METHOD(const Vector&, obbMaxs, 2, (), (this))
};

class IClientEntity
{
	VIRTUAL_METHOD(Collideable*, getCollideable, 3, (), (this))
	VIRTUAL_METHOD(const Vector&, getAbsOrigin, 10, (), (this))
	VIRTUAL_METHOD(int, health, 121, (), (this))
	VIRTUAL_METHOD(bool, isAlive, 155, (), (this))
	VIRTUAL_METHOD(Vector, getAimPunch, 345, (), (this))


	Vector getOrigin()
	{
		return *reinterpret_cast<Vector*>(this + 0x0);
	}

	bool isDefusing()
	{
		return *reinterpret_cast<bool*>(this + 0x0);
	}

	int armor()
	{
		return  *reinterpret_cast<int*>(this + 0x0);
	}
};