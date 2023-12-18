#include "JsonWrapper.h"

namespace TSUtil
{
	CMutableJsonValue::CMutableJsonValue(CJsonWriter & writer, yyjson_mut_val * val)
		: writer_(writer)
		, val_(val)
	{
	}

	yyjson_mut_doc* CMutableJsonValue::_getDocument() const
	{
		return writer_._getDocument();
	}

	CMutableJsonObject::CMutableJsonObject(CJsonWriter& writer, yyjson_mut_val* val)
		: CMutableJsonValue(writer, val)
	{
	}

	CMutableJsonArray CMutableJsonObject::getArray(std::string_view key) const
	{
		yyjson_mut_doc* doc = _getDocument();
		yyjson_mut_val* val = yyjson_mut_arr(doc);
		yyjson_mut_obj_add_val(doc, val_, key.data(), val);

		return { writer_, val };
	}

	CMutableJsonObject CMutableJsonObject::getObject(std::string_view key) const
	{
		yyjson_mut_doc* doc = _getDocument();
		yyjson_mut_val* val = yyjson_mut_obj(doc);
		yyjson_mut_obj_add_val(doc, val_, key.data(), val);

		return { writer_, val };
	}

	CMutableJsonArray::CMutableJsonArray(CJsonWriter& writer, yyjson_mut_val* val)
		: CMutableJsonValue(writer, val)
	{
	}

	CMutableJsonArray CMutableJsonArray::getArray() const
	{
		return { writer_, yyjson_mut_arr_add_arr(_getDocument(), val_) };
	}

	CMutableJsonObject CMutableJsonArray::getObject() const
	{
		return { writer_, yyjson_mut_arr_add_obj(_getDocument(), val_) };
	}

	CJsonWriter::CJsonWriter()
		: isDirty_(false)
	{
		yyjson_mut_doc* mutDocument = yyjson_mut_doc_new(nullptr);
		if (mutDocument == nullptr)
			return;

		docPtr_.reset(mutDocument);
	}

	CJsonWriter::CJsonWriter(CJsonWriter&& rhs)
		: docPtr_(std::move(rhs.docPtr_))
		, strResult_(std::move(rhs.strResult_))
		, isDirty_(rhs.isDirty_)
	{
		rhs.isDirty_ = false;
	}

	const char* CJsonWriter::getString(bool isDebug /*= false*/) const
	{
		if (isDebug == true)
			isDirty_ = true;

		if (isDirty_ == true)
		{
			const yyjson_write_flag writeFlag = isDebug == true ?
				YYJSON_WRITE_PRETTY :	// 디버그면 이쁘게
				YYJSON_WRITE_NOFLAG;	// 아니면 짧게

			char* json = yyjson_mut_write(docPtr_.get(), writeFlag, nullptr);
			if (json != nullptr)
			{
				strResult_ = json;
				free(json);
			}
			else
			{
				strResult_.clear();
			}

			isDirty_ = false;
		}

		return strResult_.c_str();
	}

	CMutableJsonObject CJsonWriter::getRootObject()
	{
		yyjson_mut_doc* mutDoc = _getDocument();
		yyjson_mut_val* root = yyjson_mut_obj(mutDoc);

		yyjson_mut_doc_set_root(mutDoc, root);
		return { *this, root };
	}

	CMutableJsonArray CJsonWriter::getRootArray()
	{
		yyjson_mut_doc* mutDoc = _getDocument();
		yyjson_mut_val* root = yyjson_mut_arr(mutDoc);

		yyjson_mut_doc_set_root(mutDoc, root);
		return { *this, root };
	}

	yyjson_mut_doc* CJsonWriter::_getDocument() const
	{
		isDirty_ = true;
		return docPtr_.get();
	}
}
