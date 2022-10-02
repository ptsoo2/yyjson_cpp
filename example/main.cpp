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
		"1": 456,
		"2": [222, "hmm", 444]
	}
)";

	// Reader Test
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
			const auto lstValue = obj["1"];
			for (const auto& val : *lstValue)
			{
				log("%llu", val.getInt64());
			}
		}
		if (obj.hasMember("2") == true)
		{
			const auto lstValue = obj["2"];
			for (const auto& val : *lstValue)
			{
				if (val.isArray() == true)
				{
					CJsonArray arr = val.getArray();
					arr.for_each(
						[](const CJsonValue& val)
						{
							if (val.isString() == true)
								log("%s", val.getString());
							else if (val.isNumber() == true)
								log("%lld", val.getUInt64());
						}
					);
				}
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

		log("%s", jsonWriter.getString(true));
	}

	// Writer Test - Root Array
	{
		CJsonWriter jsonWriter;
		CMutableJsonArray rootArr = jsonWriter.getRootArray();
		rootArr
			.write(false)
			.write(1.1111f);

		rootArr.getArray()
			.write(true);

		rootArr.getObject()
			.write("1231231", 123123)
			.write("1231211232112", true);

		rootArr.write("123123");

		std::string test2{ "!23123" };
		rootArr.write(test2);

		log("%s", jsonWriter.getString(true));
	}

	return 0;
}