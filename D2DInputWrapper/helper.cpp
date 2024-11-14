#include "helper.h"
#include <string>
#include <locale>
#include <codecvt>

Json::Value ReadJsonFromFile(std::string path, Json::Value defValue)
{
	Json::Value result;
	FILE *f = fopen(path.c_str(), "rb");
	if (f)
	{
		fseek(f, 0, SEEK_END);
		long size = ftell(f);
		fseek(f, 0, SEEK_SET);
		char *configData = (char *)malloc(size + 1);
		fread(configData, 1, size, f);
		configData[size] = 0;
		fclose(f);
		Json::Reader jsonReader;
		jsonReader.parse(std::string(configData), result, false);
		free(configData);
	}
	else
	{
		result = defValue;
	}
	return result;
}

bool SaveJsonToFile(std::string path, Json::Value value)
{
	FILE *f = fopen(path.c_str(), "wb");
	if (f)
	{
		std::string bufStr = value.toStyledString();
		fwrite(bufStr.c_str(), 1, bufStr.size(), f);
		fclose(f);
		return true;
	}
	return false;
}

void JsonValueSetWString(Json::Value &json, std::wstring value)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	json = converter.to_bytes(value);
}

std::wstring JsonValueGetWString(Json::Value &json)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(json.asString());
}