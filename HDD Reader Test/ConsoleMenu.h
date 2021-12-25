#pragma once
#include "pch.h"

//#include "ConsoleMenuItem.h"
class ConsoleMenuItem;
class ConsoleMenu
{
public:
	ConsoleMenu();
	ConsoleMenu(std::wstring exitText);
	~ConsoleMenu();
	void show();
	void addMenuItem(ConsoleMenuItem* item);
	void showExitItem(bool toShow);
	void setExitText(std::wstring exitText);

private:
	std::list<ConsoleMenuItem>* items;
	bool _showExitItem;
	int itemCount;
	std::wstring exitText;

	int selectedTextColor = 2;
	int textColor = 7;
	int selectedBackgroundColor = 1;
	int backgroundColor = 0;
	void clearScreen();
};

