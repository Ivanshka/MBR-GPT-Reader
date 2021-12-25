#pragma once
#include "pch.h"

class ConsoleMenu;
class ConsoleMenuItem
{
public:
	ConsoleMenuItem(std::wstring itemText, void (*actionToRun)());
	ConsoleMenuItem(std::wstring itemText, ConsoleMenu* menuToShow);
	~ConsoleMenuItem();
	std::wstring getText();
	void click();

private:
	void(*action)();
	std::wstring itemText;
	ConsoleMenu* newMenu;
};


