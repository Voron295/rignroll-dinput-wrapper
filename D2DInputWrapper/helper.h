#include <json\json.h>

Json::Value ReadJsonFromFile(std::string path, Json::Value defValue);
bool SaveJsonToFile(std::string path, Json::Value value);
void JsonValueSetWString(Json::Value &json, std::wstring value);
std::wstring JsonValueGetWString(Json::Value &json);