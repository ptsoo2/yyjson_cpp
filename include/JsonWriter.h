#pragma once

/*
* file		JsonWriter.h
* brief		yyjson write wrapper
* date		2022-10-02
* auther	ptsoo						*/

namespace TSUtil
{
	class CJsonWriter;
	class CMutableJsonValue
	{
	public:
		CMutableJsonValue(CJsonWriter& writer, yyjson_mut_val* val);

	protected:
		yyjson_mut_doc*			_getDocument() const;

		// signed integer
		template<SignedIntegerTrait TValue, bool isArray>
		void					_write(std::string_view key, TValue&& value) 
		{
			if constexpr (isArray == true)
				yyjson_mut_arr_add_sint(_getDocument(), val_, value);
			else
				yyjson_mut_obj_add_sint(_getDocument(), val_, key.data(), value);
		}

		// unsigned integer
		template<UnSignedIntegerTrait TValue, bool isArray>
		void					_write(std::string_view key, TValue&& value)
		{
			if constexpr (isArray == true)
				yyjson_mut_arr_add_uint(_getDocument(), val_, value);
			else
				yyjson_mut_obj_add_uint(_getDocument(), val_, key.data(), value);
		}
		
		// float / double
		template<FloatingPointTrait TValue, bool isArray>
		void					_write(std::string_view key, TValue&& value)
		{
			if constexpr (isArray == true)
				yyjson_mut_arr_add_real(_getDocument(), val_, value);
			else
				yyjson_mut_obj_add_real(_getDocument(), val_, key.data(), value);
		}

		// const char*
		template<typename TValue, bool isArray>
		void					_write(std::string_view key, const char* value)
		{
			if constexpr (isArray == true)
				yyjson_mut_arr_add_strcpy(_getDocument(), val_, value);
			else
				yyjson_mut_obj_add_strcpy(_getDocument(), val_, key.data(), value);
		}

		// std::string
		template<typename TValue, bool isArray>
		void					_write(std::string_view key, const std::string& value)
		{
			if constexpr (isArray == true)
				yyjson_mut_arr_add_strncpy(_getDocument(), val_, value.c_str(), value.size());
			else
				yyjson_mut_obj_add_strncpy(_getDocument(), val_, key.data(), value.c_str(), value.size());
		}

		// bool
		template<typename TValue, bool isArray>
		void					_write(std::string_view key, bool value)
		{
			if constexpr (isArray == true)
				yyjson_mut_arr_add_bool(_getDocument(), val_, value);
			else
				yyjson_mut_obj_add_bool(_getDocument(), val_, key.data(), value);
		}

	protected:
		CJsonWriter&			writer_;
		yyjson_mut_val*			val_;
	};

	class CMutableJsonObject;
	class CMutableJsonArray : private CMutableJsonValue
	{
	public:
		CMutableJsonArray(CJsonWriter& writer, yyjson_mut_val* val);

	public:
		CMutableJsonArray		getArray() const;
		CMutableJsonObject		getObject() const;

		template<typename TValue>
		CMutableJsonArray&		write(TValue&& value)
		{
			_write<TValue, true>("", std::forward<TValue>(value));
			return *this;
		}
	};

	class CMutableJsonObject : private CMutableJsonValue
	{
	public:
		CMutableJsonObject(CJsonWriter& writer, yyjson_mut_val* val);

	public:
		CMutableJsonArray		getArray(std::string_view key) const;
		CMutableJsonObject		getObject(std::string_view key) const;

		template<typename TValue>
		CMutableJsonObject&		write(std::string_view key, TValue&& value)
		{
			_write<TValue, false>(key, std::forward<TValue>(value));
			return *this;
		}
	};

	class CJsonWriter
	{
		friend class CMutableJsonValue;

		struct stDeleter_t { void operator()(yyjson_mut_doc* doc) { yyjson_mut_doc_free(doc); } };
		using docPtr_t = std::unique_ptr<yyjson_mut_doc, stDeleter_t>;

	public:
		CJsonWriter();
		CJsonWriter(CJsonWriter&& rhs);

	public:
		const char*				getString(bool isDebug = false) const;
		std::string				getstdString(bool isDebug = false) const { return getString(isDebug); }

	public:
		CMutableJsonObject		getRootObject();
		CMutableJsonArray		getRootArray();

	protected:
		void					_clear();
		yyjson_mut_doc*			_getDocument() const;

	protected:
		docPtr_t				docPtr_;
		yyjson_mut_val*			rootVal_ = nullptr;

		mutable std::string		strResult_;
		mutable bool			isDirty_ = false;
	};
}
