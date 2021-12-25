#include "pch.h"
#include "ConsoleMenuItem.h"

ConsoleMenuItem::ConsoleMenuItem(std::wstring itemText, void(*action)())
{
	this->itemText = itemText;
	this->action = action;
	this->newMenu = nullptr;
}

ConsoleMenuItem::ConsoleMenuItem(std::wstring itemText, ConsoleMenu* menuToShow)
{
	this->itemText = itemText;
	this->newMenu = menuToShow;
	this->action = nullptr;
}

ConsoleMenuItem::~ConsoleMenuItem() {

}

std::wstring ConsoleMenuItem::getText()
{
	return itemText;
}

void ConsoleMenuItem::click()
{
	if (action != nullptr)
		(*action)();
	else
		newMenu->show();
}
