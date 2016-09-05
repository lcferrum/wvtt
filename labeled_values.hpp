#ifndef LABELED_VALUES_HPP
#define LABELED_VALUES_HPP

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <initializer_list>
#include <windows.h>	//TCHAR, TEXT and DWORD

#define LABELED_VALUES_ARG(...)			false, TEXT(#__VA_ARGS__), {__VA_ARGS__}
#define UNIQUE_LABELED_VALUES_ARG(...)	true, TEXT(#__VA_ARGS__), {__VA_ARGS__}
template <typename ValT>
class BasicLabeledValues {
#ifdef UNICODE
	typedef std::wstring TSTRING;
	typedef std::wstringstream TSTRINGSTREAM;
#else
	typedef std::string TSTRING;
	typedef std::stringstream TSTRINGSTREAM;
#endif
private:
	std::vector<std::pair<TSTRING, ValT>> ValList;
	TSTRING ShowMany(const TSTRING& label, DWORD value, bool unknown, bool first) {
		if (!unknown)
			return (first?TSTRING():TSTRING(TEXT(", ")))+label;
		else
			return TSTRING();
	}
	TSTRING ShowSingle(const TSTRING& label, DWORD value, bool unknown, bool first) {
		if (!unknown&&first)
			return label;
		else
			return TSTRING();
	}
public:
	BasicLabeledValues(bool unique_vals, const TCHAR* vals_str, const std::initializer_list<ValT> &vals): ValList() {
		TSTRINGSTREAM vals_ss(vals_str);
		TSTRING value;
		typename std::initializer_list<ValT>::iterator vals_it=vals.begin();
		while (std::getline(vals_ss, value, TEXT(','))&&vals_it!=vals.end()) {
			value.erase(std::remove_if(value.begin(), value.end(), [](TCHAR ch){ return std::isspace<TCHAR>(ch, std::locale::classic()); }), value.end());
			ValList.push_back(std::make_pair(value, *vals_it));
			vals_it++;
		}
		if (unique_vals)
			ValList.erase(std::unique(ValList.begin(), ValList.end(), [](const std::pair<TSTRING, ValT> &L, const std::pair<TSTRING, ValT> &R){
				return L.second==R.second;
			}), ValList.end());
	}
	ValT Find(const TSTRING& label, ValT def_val) {
		for (std::pair<TSTRING, ValT> &val_pair: ValList)
			if (val_pair.first==label) return val_pair.second;
		return def_val;
	}
	size_t Size() {
		return ValList.size();
	}
	size_t Values(std::function<void(const TSTRING&, ValT)> enum_function) {
		for (std::pair<TSTRING, ValT> &val_pair: ValList)
			enum_function(val_pair.first, val_pair.second);
		return ValList.size();
	}
	TSTRING Values(std::function<TSTRING(const TSTRING&, ValT, bool)> enum_function) {
		TSTRING result;
		bool first_val=true;
		for (std::pair<TSTRING, ValT> &val_pair: ValList) {
			result+=enum_function(val_pair.first, val_pair.second, first_val);
			first_val=false;
		}
		return result;
	}
	TSTRING Values() {
		return Values(std::bind(&BasicLabeledValues::ShowMany, this, std::placeholders::_1, std::placeholders::_2, false, std::placeholders::_3));
	}
	size_t Enums(ValT enums, std::function<void(const TSTRING&, ValT, bool)> enum_function) {
		size_t count=0;
		for (std::pair<TSTRING, ValT> &val_pair: ValList) {
			if (enums==val_pair.second) {
				count++;
				enum_function(val_pair.first, val_pair.second, false);
			}
		}
		if (!count)
			enum_function(TEXT(""), enums, true);
		return count;
	}
	TSTRING Enums(ValT enums, std::function<TSTRING(const TSTRING&, ValT, bool, bool)> enum_function) {
		TSTRING result;
		bool first_val=true;
		bool found=false;
		for (std::pair<TSTRING, ValT> &val_pair: ValList) {
			if (enums==val_pair.second) {
				found=true;
				result+=enum_function(val_pair.first, val_pair.second, false, first_val);
				first_val=false;
			}
		}
		if (!found)
			result+=enum_function(TEXT(""), enums, true, first_val);
		return result;
	}
	TSTRING Enums(ValT enums) {
		return Enums(enums, std::bind(&BasicLabeledValues::ShowSingle, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}
	TSTRING operator()(ValT value) {
		return Enums(value);
	}
	size_t Flags(ValT flags, std::function<void(const TSTRING&, ValT, bool)> enum_function) {
		ValT checked_flags=0;
		size_t count=0;
		for (std::pair<TSTRING, ValT> &val_pair: ValList) {
			if ((flags&val_pair.second)==val_pair.second) {
				checked_flags|=val_pair.second;
				count++;
				enum_function(val_pair.first, val_pair.second, false);
			}
		}
		if (checked_flags!=flags) {
			flags&=~checked_flags;
			enum_function(TEXT(""), flags, true);
		}
		return count;
	}
	TSTRING Flags(ValT flags, std::function<TSTRING(const TSTRING&, ValT, bool, bool)> enum_function) {
		TSTRING result;
		bool first_val=true;
		ValT checked_flags=0;
		for (std::pair<TSTRING, ValT> &val_pair: ValList) {
			if ((flags&val_pair.second)==val_pair.second) {
				checked_flags|=val_pair.second;
				result+=enum_function(val_pair.first, val_pair.second, false, first_val);
				first_val=false;
			}
		}
		if (checked_flags!=flags) {
			flags&=~checked_flags;
			result+=enum_function(TEXT(""), flags, true, first_val);
		}
		return result;
	}
	TSTRING Flags(ValT flags) {
		return Flags(flags, std::bind(&BasicLabeledValues::ShowMany, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}
};
typedef BasicLabeledValues<DWORD> LabeledValues;

#endif //LABELED_VALUES_HPP
