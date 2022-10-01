#include "stdafx.h"
#include "JsonReader.h"

namespace TSUtil
{
	CJsonValue::CJsonValue()
		: val_(nullptr)
	{
	}

	CJsonValue::CJsonValue(yyjson_val* val)
		: val_(val)
	{
	}

	CJsonArray CJsonValue::getArray() const
	{
		return val_;
	}

	CJsonObject CJsonValue::getObject() const
	{
		return val_;
	}

	CJsonObject::CJsonObject(yyjson_val* val)
		: val_(val)
	{
	}

	CJsonObject::CJsonObject(CJsonObject && rhs)
		: val_(rhs.val_)
		, mapValue_(std::move(rhs.mapValue_))
	{
	}

	yyjson_val* CJsonObject::_cacheAndGet(const char* key) const
	{
		if ((val_ == nullptr) || (key == nullptr))
			return nullptr;

		const auto hashValue = std::hash<std::string_view>()(key);

		// cache hit
		auto findIter = mapValue_.find(hashValue);
		if (findIter != mapValue_.end())
			return findIter->second;

		auto rawObj = yyjson_obj_get(val_, key);
		if (rawObj == nullptr)
			return nullptr;

		mapValue_.emplace(hashValue, rawObj);
		return rawObj;
	}

	CJsonArray::CJsonArray(yyjson_val* val)
	{
		size_t size = yyjson_arr_size(val);

		lstJsonValue_.reserve(size);

		yyjson_val* curValue = yyjson_arr_get_first(val);
		while (size--)
		{
			lstJsonValue_.emplace_back(curValue);
			curValue = unsafe_yyjson_get_next(curValue);
		}
	}

	CJsonArray::CJsonArray(CJsonArray&& rhs)
		: lstJsonValue_(std::move(rhs.lstJsonValue_))
	{
	}

	CJsonReader::CJsonReader(CJsonReader&& rhs)
		: docPtr_(std::move(rhs.docPtr_))
	{
	}

	bool CJsonReader::parseFromString(const char* str)
	{
		if (str == nullptr)
			return false;

		yyjson_doc* rawDoc = yyjson_read(str, strlen(str), 0);
		if (rawDoc == nullptr)
			return false;

		val_ = yyjson_doc_get_root(rawDoc);
		docPtr_.reset(rawDoc);

		return true;
	}
}
