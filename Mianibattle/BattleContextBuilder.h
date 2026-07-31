#pragma once

#include "BattleContext.h"
#include "Character.h"

class BattleContextBuilder
{
public:

	BattleContext Build(const Character& self, const Character& target);

private:
	
	CharacterSnapshot MakeCharacterSnapshot(const Character& character);
	StatusSnapshot MakeStatusSnapshot(const Character& character);
	ActionControl MakeActionControl(const Character& character);

};