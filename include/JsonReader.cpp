#include "JsonWrapper.h"

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
		: CJsonValue(val)
		, isCached_(false)
	{
	}

	CJsonObject::CJsonObject(CJsonObject&& rhs)
		: CJsonValue(rhs.val_)
		, isCached_(rhs.isCached_)
		, mapValue_(std::move(rhs.mapValue_))
	{
		rhs.val_ = nullptr;
		rhs.isCached_ = false;
	}

	void CJsonObject::_caching() const
	{
		if (isCached_ == true)
			return;

		const size_t max = yyjson_obj_size(val_);
		if (max < 1)
		{
			isCached_ = true;
			return;
		}

		yyjson_val* yyKey = unsafe_yyjson_get_first(val_);
		yyjson_val* yyVal = yyKey + 1;

		for (size_t i = 0; i < max; ++i)
		{
			const auto hashValue = std::hash<std::string_view>()((const char*)yyKey->uni.ptr);
			auto findIter = mapValue_.find(hashValue);
			if (findIter == mapValue_.end())
			{
				findIter = mapValue_.emplace(hashValue, lstDuplicateValue_t{}).first;
				findIter->second.reserve(max);
			}

			findIter->second.emplace_back(yyVal);

			yyKey = unsafe_yyjson_get_next(yyVal);
			yyVal = yyKey + 1;
		}

		isCached_ = true;
	}

	auto CJsonObject::_cacheAndGet(std::string_view key) const -> const lstDuplicateValue_t*
	{
		if ((val_ == nullptr) || (key.empty() == true))
			return nullptr;

		const auto hashValue = std::hash<std::string_view>()(key);

		_caching();

		const auto findIter = mapValue_.find(hashValue);
		return (findIter == mapValue_.end())
			? nullptr
			: &(findIter->second);	// cache hit
	}

	CJsonArray::CJsonArray(yyjson_val* val)
		: CJsonValue(val)
		, isCached_(false)
	{
	}

	CJsonArray::CJsonArray(CJsonArray&& rhs)
		: CJsonValue(rhs.val_)
		, isCached_(rhs.isCached_)
		, lstJsonValue_(std::move(rhs.lstJsonValue_))
	{
		rhs.val_ = nullptr;
		rhs.isCached_ = false;
	}

	const CJsonValue* CJsonArray::at(size_t idx)
	{
		_caching();
		return (idx >= lstJsonValue_.size())
			? nullptr 
			: &(lstJsonValue_.at(idx));
	}

	void CJsonArray::for_each(fnForeach_t&& fn)
	{
		_caching();
		for (const CJsonValue& val : lstJsonValue_)
		{
			fn(val);
		}
	}

	void CJsonArray::_caching()
	{
		if (isCached_ == true)
			return;

		if (val_ == nullptr)
		{
			isCached_ = true;
			return;
		}

		size_t size = yyjson_arr_size(val_);
		if (size < 1)
		{
			isCached_ = true;
			return;
		}

		lstJsonValue_.reserve(size);

		yyjson_val* curValue = yyjson_arr_get_first(val_);
		while (size--)
		{
			lstJsonValue_.emplace_back(curValue);
			curValue = unsafe_yyjson_get_next(curValue);
		}

		isCached_ = true;
	}

	CJsonReader::CJsonReader(CJsonReader&& rhs)
		: docPtr_(std::move(rhs.docPtr_))
	{
	}

	bool CJsonReader::parseFromString(std::string_view str)
	{
		if (str.empty() == true)
			return false;

		const char* data = str.data();

		if (data == nullptr)
			return false;

		yyjson_doc* rawDoc = yyjson_read(data, str.length(), 0);
		if (rawDoc == nullptr)
			return false;

		val_ = yyjson_doc_get_root(rawDoc);
		docPtr_.reset(rawDoc);

		return true;
	}
}
