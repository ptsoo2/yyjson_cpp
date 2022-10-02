#pragma once

/*
* file		JsonReader.h
* brief		yyjson read wrapper
* date		2022-10-02
* auther	ptsoo						*/

namespace TSUtil
{
	class CJsonArray;
	class CJsonObject;

	class CJsonValue
	{
	public:
		CJsonValue();
		CJsonValue(yyjson_val* val);

	public:
		yyjson_type				getType() const { return yyjson_get_type(val_); }
		yyjson_subtype			getSubType() const { return yyjson_get_subtype(val_); }

		const char*				getString() const { return yyjson_get_str(val_); }
		std::string				getstdString() const { return getString(); }
		int64_t					getInt64() const { return yyjson_get_sint(val_); }
		uint64_t				getUInt64() const { return yyjson_get_uint(val_); }
		double					getDouble() const { return yyjson_get_real(val_); }
		bool					getBool() const { return yyjson_get_bool(val_); }
		CJsonArray				getArray() const;
		CJsonObject				getObject() const;

		bool					isObject() const { return yyjson_is_obj(val_); }
		bool					isArray() const { return yyjson_is_arr(val_); }
		bool					isString() const { return yyjson_is_str(val_); }
		bool					isNumber() const { return yyjson_is_num(val_); }
		bool					isInt64() const { return yyjson_is_sint(val_); }
		bool					isUInt64() const { return yyjson_is_uint(val_); }
		bool					isDouble() const { return yyjson_is_real(val_); }
		bool					isBool() const { return yyjson_is_bool(val_); }

	protected:
		yyjson_val*				val_;
	};

	class CJsonArray
	{
		using lstJsonValue_t = std::vector<CJsonValue>;

	public:
		CJsonArray(yyjson_val* val);
		CJsonArray(CJsonArray&& rhs);

	public:
		const CJsonValue*		at(size_t idx) const { return idx >= lstJsonValue_.size() ? nullptr : &(lstJsonValue_.at(idx)); }
		size_t					size() const { return lstJsonValue_.size(); }

		template<typename TFn>
		void					for_each(TFn&& fn) const
		{
			for (const auto& val : lstJsonValue_)
				fn(val);
		}

	protected:
		lstJsonValue_t			lstJsonValue_;
	};

	class CJsonObject : private CJsonValue
	{
		using lstDuplicateValue_t = std::vector<CJsonValue>;
		using mapValue_t = std::map<std::size_t /* hashValue */, lstDuplicateValue_t>;

	public:
		CJsonObject(yyjson_val* val);
		CJsonObject(CJsonObject&& rhs);

	public:
		auto					operator[](const char* key) const -> const lstDuplicateValue_t* { return _cacheAndGet(key); }
		bool					hasMember(const char* name) const { return _cacheAndGet(name) != nullptr; }

	protected:
		auto					_cacheAndGet(const char* key) const -> const lstDuplicateValue_t*;
		lstDuplicateValue_t		_getValueList(std::string_view key) const;

	protected:
		yyjson_val*				val_;
		mutable mapValue_t		mapValue_;
	};

	class CJsonReader : public CJsonValue
	{
		struct stDeleter_t { void operator()(yyjson_doc* doc) { yyjson_doc_free(doc); } };
		using docPtr_t = std::unique_ptr<yyjson_doc, stDeleter_t>;

	public:
		CJsonReader() = default;
		CJsonReader(CJsonReader&& rhs);

	public:
		bool					parseFromString(const char* str);

	protected:
		docPtr_t				docPtr_;
	};
}
