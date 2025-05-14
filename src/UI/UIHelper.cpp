#include "stdafx.h"
#include "UI/UIHelper.h"

std::string FormatText(const char* i_text, utils::Log::TextFormat i_textFormat)
{
	std::string formatterColor = "\033[{0}m{1}\033[0m";
	if (i_textFormat.textColor.IsValidColor() && i_textFormat.backgroundColor.IsValidColor())
	{
		formatterColor = "\033[{0};38;2;{2};{3};{4};48;2;{5};{6};{7}m{1}\033[0m";
	}
	else if (i_textFormat.textColor.IsValidColor())
	{
		formatterColor = "\033[{0};38;2;{2};{3};{4}m{1}\033[0m";
	}
	else if (i_textFormat.backgroundColor.IsValidColor())
	{
		formatterColor = "\033[{0};48;2;{5};{6};{7}m{1}\033[0m";
	}
	return utils::Format(formatterColor.c_str(), std::underlying_type_t<utils::Log::TextStyle>(i_textFormat.textStyle), i_text, i_textFormat.textColor.red.value, i_textFormat.textColor.green.value, i_textFormat.textColor.blue.value, i_textFormat.backgroundColor.red.value, i_textFormat.backgroundColor.green.value, i_textFormat.backgroundColor.blue.value);
}