#pragma once
#include "ThemidaSDK.h"

#ifdef _DEBUG
#define PROTECT_VM_START_HIGH
#define PROTECT_VM_END_HIGH

#define PROTECT_VM_START_LOW
#define PROTECT_VM_END_LOW

#define PROTECT_MUTATE_START
#define PROTECT_MUTATE_END

#define PROTECT_STR_START
#define PROTECT_STR_END
#else
#define PROTECT_VM_START_HIGH		CustomVM00000139_Start()
#define PROTECT_VM_END_HIGH			CustomVM00000139_End()

#define PROTECT_VM_START_LOW		CustomVM00000103_Start()
#define PROTECT_VM_END_LOW			CustomVM00000103_End()

#define PROTECT_MUTATE_START		MutateStart()
#define PROTECT_MUTATE_END			MutateEnd()

#define PROTECT_STR_START			StrEncryptStart()
#define PROTECT_STR_END				StrEncryptEnd()
#endif 