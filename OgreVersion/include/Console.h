/*! Originally a MyGUI demo
	@file
	@author		Albert Semenov
	@date		08/2008
	@module

	Added to Blobby Volley 3D - 06/2009 (Benjamin Wasty)
	Changed to MyGUI 3.01 Version - 3/2010 (main change: used string type)
*/
#pragma once

#include <MyGUI.h>
#include "BaseLayout/BaseLayout.h"
#include <limits>

#ifdef max
#    undef max
#    undef min
#endif

typedef MyGUI::delegates::CDelegate2<const MyGUI::UString &, const MyGUI::UString &> CommandDelegate;

namespace formates
{
	template<typename T> inline std::string format() { return MyGUI::utility::toString("[ ", std::numeric_limits<T>::min(), " | ", std::numeric_limits<T>::max(), " ]"); }
	template<> inline std::string format<bool>() { return "[ true | false ]"; }
	template<> inline std::string format<float>() { return MyGUI::utility::toString("[ ", -std::numeric_limits<float>::max(), " | ", std::numeric_limits<float>::max(), " ]"); }
	template<> inline std::string format<double>() { return MyGUI::utility::toString("[ ", -std::numeric_limits<double>::max(), " | ", std::numeric_limits<double>::max(), " ]"); }
}

class Console : public wraps::BaseLayout
{
public:
	static Console * getInstancePtr();
	static Console & getInstance();

	Console();
	virtual ~Console();

	void addToConsole(const MyGUI::UString & _line);
	void addToConsole(const MyGUI::UString & _reason, const MyGUI::UString & _key, const MyGUI::UString & _value)
	{
		addToConsole(MyGUI::utility::toString(_reason, "'", _key, " ", _value, "'"));
	}

	void clearConsole();

	/** Method : add command.\n
		@example Add_console_command
		@code
			registerConsoleDelegate("your_command_1", MyGUI::newDelegate(your_func));
			registerConsoleDelegate("your_command_2", MyGUI::newDelegate(your_static_method));
			registerConsoleDelegate("your_command_3", MyGUI::newDelegate(your_class_ptr, &your_class_name::your_method_name));
		@endcode

		signature your method : void method(const MyGUI::UString & _key, const MyGUI::UString & _value)
	*/
	void registerConsoleDelegate(const MyGUI::UString & _command, CommandDelegate::IDelegate * _delegate);

	/** Event : Unknown command.\n
		signature : void method(const MyGUI::UString & _key, const MyGUI::UString & _value)
	*/
	CommandDelegate eventConsoleUnknowCommand;

	const MyGUI::UString & getConsoleStringCurrent() { return mStringCurrent; }
	const MyGUI::UString & getConsoleStringError() { return mStringError; }
	const MyGUI::UString & getConsoleStringSuccess() { return mStringSuccess; }
	const MyGUI::UString & getConsoleStringUnknow() { return mStringUnknow; }
	const MyGUI::UString & getConsoleStringFormat() { return mStringFormat; }

	bool isVisible() { return mMainWidget->isVisible(); }
	void setVisible(bool _visible) { mMainWidget->setVisible(_visible); }

	template <typename T> bool isAction(T & _result, const MyGUI::UString & _key, const MyGUI::UString & _value, const MyGUI::UString & _format = "")
	{
		if (_value.empty())
		{
			addToConsole(getConsoleStringCurrent(), _key, MyGUI::utility::toString(_result));
		}
		else
		{
			if (!MyGUI::utility::parseComplex(_value, _result))
			{
				addToConsole(getConsoleStringError(), _key, _value);
				addToConsole(getConsoleStringFormat(), _key, _format.empty() ? formates::format<T>() : _format);
			}
			else
			{
				addToConsole(getConsoleStringSuccess(), _key, _value);
				return true;
			}
		}
		return false;
	}

private:
	void notifyWindowButtonPressed(MyGUI::Window* _sender, const std::string& _button);

	void notifyMouseButtonClick(MyGUI::Widget* _sender);
	void notifyComboAccept(MyGUI::ComboBox* _sender, size_t _index);
	void notifyButtonPressed(MyGUI::Widget* _sender, MyGUI::KeyCode _key, MyGUI::Char _char);

	void internalCommand(MyGUI::Widget* _sender, const MyGUI::UString & _key, const MyGUI::UString & _value);

private:
	MyGUI::Edit* mListHistory;
	MyGUI::ComboBox* mComboCommand;
	MyGUI::Button* mButtonSubmit;

	typedef std::map<MyGUI::UString, CommandDelegate> MapDelegate;
	MapDelegate mDelegates;

	MyGUI::UString mStringCurrent;
	MyGUI::UString mStringError;
	MyGUI::UString mStringSuccess;
	MyGUI::UString mStringUnknow;
	MyGUI::UString mStringFormat;

	// ���� ������� ����� ������������
	bool mAutocomleted;

	static Console * m_instance;
};
