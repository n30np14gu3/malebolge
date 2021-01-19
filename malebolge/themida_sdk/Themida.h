#pragma once
#ifdef _DEBUG
#define PROTECT_VM_START_CRITICAL
#define PROTECT_VM_END_CRITICAL
#define PROTECT_VM_START_HIGH
#define PROTECT_VM_END_HIGH

#define PROTECT_VM_START_LOW
#define PROTECT_VM_END_LOW

#define PROTECT_MUTATE_START
#define PROTECT_MUTATE_END

#define PROTECT_STR_START
#define PROTECT_STR_END
#else
#include "ThemidaSDK.h"
#define PROTECT_VM_START_CRITICAL	CustomVM00000113_Start()
#define PROTECT_VM_END_CRITICAL		CustomVM00000113_End()

#define PROTECT_VM_START_HIGH		CustomVM00000139_Start()
#define PROTECT_VM_END_HIGH			CustomVM00000139_End()

#define PROTECT_VM_START_LOW		CustomVM00000103_Start()
#define PROTECT_VM_END_LOW			CustomVM00000103_End()

#define PROTECT_MUTATE_START		MutateStart()
#define PROTECT_MUTATE_END			MutateEnd()

#define PROTECT_STR_START			StrEncryptStart()
#define PROTECT_STR_END				StrEncryptEnd()
#endif 