#include "stdafx.h"

template<typename ...TArgs>
void log(const char* format, TArgs... args)
{
	std::string newfmt{ format };
	newfmt.append("\n");
	printf(newfmt.c_str(), (args)...);
}

using namespace TSUtil;

int main()
{
	const char* script = R"(
	{ 
		"1": 123,
		"2": [222, "hmm", 444]
	}
)";

	// Parsing Test
	{
		CJsonReader reader;
		if (reader.parseFromString(script) == false)
		{
			log("failed parse");
			return -1;
		}

		CJsonObject obj = reader.getObject();
		if (obj.hasMember("1") == true)
		{
			log("%llu", obj["1"].getInt64());
			log("%llu", obj["1"].getInt64());
			log("%llu", obj["1"].getInt64());
			log("%llu", obj["1"].getInt64());
		}
		if (obj.hasMember("2") == true)
		{
			CJsonValue val = obj["2"];
			if (val.isArray() == true)
			{
				CJsonArray arr = val.getArray();
				arr.for_each(
					[](const CJsonValue& val)
					{
						if (val.isString() == true)
							log("%s", val.getString());
						else if(val.isNumber() == true)
							log("%lld", val.getUInt64());
					}
				);
			}
		}
	}

	// Writer Test - Root Object
	{
		CJsonWriter jsonWriter;
		CMutableJsonObject rootObj = jsonWriter.getRootObject();

		rootObj
			.write("123123", false)
			.write("123123", true)
			.write("123123", "123123");

		rootObj.getArray("123111111123")
			.write("1231312")
			.write("1.111f")
			.write(true)
			.write(1231231123123llu);

		rootObj.getObject("12312313212")
			.write("a", "1231312")
			.write("b", "1.111f")
			.write("c", true)
			.write("d", 1231231123123llu);

		rootObj.getArray("123111111123")
			.write("1231312");

		rootObj.getArray("123111111123")
			.write("1231312");

		log("%s", jsonWriter.getstdString(true).c_str());
	}

	// Writer Test - Root Array
	{
		CJsonWriter jsonWriter;
		CMutableJsonArray rootArr = jsonWriter.getRootArray();
		rootArr.write(false);
		float test = 1.1f;
		rootArr.write(test);
		rootArr.getArray().write(true);
		rootArr.getObject().write("1231231", 123123);
		rootArr.getObject().write("1231211232112", true);

		rootArr.write("123123");
		std::string test2{ "!23123" };
		rootArr.write(test2);

		log("%s", jsonWriter.getstdString(true).c_str());
	}

	return 0;
}