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
		template<typename TValue, typename TDecayValue = std::decay_t<TValue>, typename std::enable_if_t<
			std::is_floating_point_v<TDecayValue> == false
				&& std::is_arithmetic_v<TDecayValue> == true
				&& std::is_unsigned_v<TDecayValue> == false
		, int> = 0>
		void					_write(const char* key, TValue&& value) 
		{
			if (key == nullptr)		yyjson_mut_arr_add_sint(_getDocument(), val_, value);
			else					yyjson_mut_obj_add_sint(_getDocument(), val_, key, value);
		}

		// unsigned integer
		template<typename TValue, typename TDecayValue = std::decay_t<TValue>, typename std::enable_if_t<
			std::is_floating_point_v<TDecayValue> == false 
				&& std::is_arithmetic_v<TDecayValue> == true 
				&& std::is_unsigned_v<TDecayValue> == true
		, int> = 0>
		void					_write(const char* key, TValue&& value)
		{
			if (key == nullptr)		yyjson_mut_arr_add_uint(_getDocument(), val_, value);
			else					yyjson_mut_obj_add_uint(_getDocument(), val_, key, value);
		}
		
		// float / double
		template<typename TValue, typename TDecayValue = std::decay_t<TValue>, typename std::enable_if_t<
			std::is_floating_point_v<TDecayValue> == true
				&& std::is_arithmetic_v<TDecayValue> == true
		, int> = 0>
		void					_write(const char* key, TValue&& value) 
		{
			if (key == nullptr)		yyjson_mut_arr_add_real(_getDocument(), val_, value);
			else					yyjson_mut_obj_add_real(_getDocument(), val_, key, value);
		}

		// const char*
		void					_write(const char* key, const char* value) 
		{
			if (key == nullptr)		yyjson_mut_arr_add_strcpy(_getDocument(), val_, value);
			else					yyjson_mut_obj_add_strcpy(_getDocument(), val_, key, value);
		}

		// std::string
		void					_write(const char* key, const std::string& value) 
		{
			if (key == nullptr)		yyjson_mut_arr_add_strncpy(_getDocument(), val_, value.c_str(), value.size());
			else					yyjson_mut_obj_add_strncpy(_getDocument(), val_, key, value.c_str(), value.size());
		}

		// bool
		void					_write(const char* key, bool value) 
		{
			if (key == nullptr)		yyjson_mut_arr_add_bool(_getDocument(), val_, value);
			else					yyjson_mut_obj_add_bool(_getDocument(), val_, key, value);
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
			_write(nullptr, std::forward<TValue>(value));
			return *this;
		}
	};

	class CMutableJsonObject : private CMutableJsonValue
	{
	public:
		CMutableJsonObject(CJsonWriter& writer, yyjson_mut_val* val);

	public:
		CMutableJsonArray		getArray(const char* key) const;
		CMutableJsonObject		getObject(const char* key) const;

		template<typename TValue>
		CMutableJsonObject&		write(const char* key, TValue&& value)
		{
			_write(key, std::forward<TValue>(value));
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
		yyjson_mut_doc*			_getDocument() const;

	protected:
		docPtr_t				docPtr_;
		mutable std::string		strResult_;
		mutable bool			isDirty_;
	};
}