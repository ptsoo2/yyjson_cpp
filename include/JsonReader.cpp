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

	auto CJsonObject::_cacheAndGet(const char* key) const -> const lstDuplicateValue_t*
	{
		if ((val_ == nullptr) || (key == nullptr))
			return nullptr;

		const auto hashValue = std::hash<std::string_view>()(key);

		// cache hit
		auto findIter = mapValue_.find(hashValue);
		if (findIter != mapValue_.end())
			return &(findIter->second);

		// cache miss
		findIter = mapValue_.emplace(hashValue, _getValueList(key)).first;
		return &(findIter->second);
	}

	auto CJsonObject::_getValueList(std::string_view key) const -> lstDuplicateValue_t
	{
		lstDuplicateValue_t lstDuplicateValue;
		const size_t max = yyjson_obj_size(val_);
		if (max < 1)
		{
			return lstDuplicateValue;
		}

		lstDuplicateValue.reserve(max);

		const size_t keyLen = key.length();
		const uint64_t tag = (keyLen << YYJSON_TAG_BIT) | YYJSON_TYPE_STR;

		yyjson_val* yyKey = unsafe_yyjson_get_first(val_);
		yyjson_val* yyVal = yyKey + 1;

		for (size_t i = 0; i < max; ++i)
		{
			if ((yyKey->tag == tag) && (memcmp(yyKey->uni.ptr, (void*)key.data(), keyLen) == 0))
			{
				lstDuplicateValue.emplace_back(yyVal);
			}

			yyKey = unsafe_yyjson_get_next(yyVal);
			yyVal = yyKey + 1;
		}

		return lstDuplicateValue;
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
