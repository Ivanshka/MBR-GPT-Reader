#include "pch.h"

void SetConsoleColors(int textColor, int backgroundColor);

void zeroFunc() {
    wprintf(L"Zero function -_- wtf?");
};

ConsoleMenu::ConsoleMenu()
{
	items = new std::list<ConsoleMenuItem>();
    itemCount = 0;
    _showExitItem = false;
}

ConsoleMenu::ConsoleMenu(std::wstring exitText)
{
    items = new std::list<ConsoleMenuItem>();
    itemCount = 0;
    _showExitItem = true;
    this->exitText = exitText;
}

ConsoleMenu::~ConsoleMenu()
{
	delete items;
}

void ConsoleMenu::show()
{
    ConsoleMenuItem* exitItem = nullptr;

    if (_showExitItem) {
        exitItem = new ConsoleMenuItem(exitText, zeroFunc);
        addMenuItem(exitItem);
    }

    int selected = 0;

    while (true) {

        std::list<ConsoleMenuItem>::iterator iter = items->begin();

        for (int i = 0; iter != items->end(); ++iter, ++i)
        {
            std::wstring itemText = iter->getText();

            if (i == selected) {
                SetConsoleColors(selectedTextColor, selectedBackgroundColor);
                wprintf(L"%d) %s", i + 1, itemText.c_str());
                SetConsoleColors(textColor, backgroundColor);
            }
            else
            {
                wprintf(L"%d) %s", i + 1, itemText.c_str());
            }
        }

        int ch = _getch();
        if (ch == 72)
            --selected;
        if (ch == 80)
            ++selected;
        if (ch == 13) {
            if (_showExitItem && (selected == itemCount - 1))
                break;
            else {
                iter = items->begin();
                std::advance(iter, selected);
                iter->click();
            }
        }

        if (selected == itemCount)
            selected = itemCount - 1;
        else if (selected < 0)
            selected = 0;

        SetConsoleColors(textColor, backgroundColor);
        clearScreen();
    }

    if (_showExitItem) {
        items->pop_back();
    }
}

void ConsoleMenu::addMenuItem(ConsoleMenuItem* item)
{
	items->push_back(*item);
    ++itemCount;
}

void ConsoleMenu::showExitItem(bool toShow)
{
    if (this->_showExitItem == toShow)
        return;
	_showExitItem = toShow;
}

void ConsoleMenu::setExitText(std::wstring exitText)
{
    this->exitText = exitText;
}

void SetConsoleColors(int textColor, int backgroundColor) {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE); //получаем наше окно, в котором нужно изменить цвет
    SetConsoleTextAttribute(consoleHandle, (((2 << backgroundColor) | textColor)));
}

void ConsoleMenu::clearScreen() {
    char fill = ' ';
    COORD tl = { 0,0 };
    CONSOLE_SCREEN_BUFFER_INFO s;
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(console, &s);
    DWORD written, cells = s.dwSize.X * s.dwSize.Y;
    FillConsoleOutputCharacter(console, fill, cells, tl, &written);
    FillConsoleOutputAttribute(console, s.wAttributes, cells, tl, &written);
    SetConsoleCursorPosition(console, tl);
}

