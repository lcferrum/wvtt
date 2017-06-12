#ifndef LABELED_VALUES_HPP
#define LABELED_VALUES_HPP

#include <string>
#include <vector>
#include <locale>
#include <sstream>
#include <algorithm>
#include <functional>
#include <initializer_list>

#ifdef _WIN32
#include <windows.h>	//DWORD
#endif

#define LABELED_VALUES_ARG(...)	TEXT(#__VA_ARGS__), {__VA_ARGS__}
#if defined(UNICODE) && defined(_WIN32)
template <typename ValT, typename ArgT=ValT, bool unique=false, typename BLV_TCHAR=wchar_t>
#else
template <typename ValT, typename ArgT=ValT, bool unique=false, typename BLV_TCHAR=char>
#endif
class BasicLabeledValues {
	typedef std::basic_string<BLV_TCHAR> BLV_TSTRING;
	typedef std::basic_stringstream<BLV_TCHAR> BLV_TSTRINGSTREAM;
private:
	std::vector<std::pair<BLV_TSTRING, ValT>> ValList;
	BLV_TCHAR comma;
	BLV_TCHAR space;
	
	void FillValList(const BLV_TCHAR* vals_str, const std::initializer_list<ValT> &vals) {
		BLV_TSTRING value(vals_str);
		value.erase(std::remove_if(value.begin(), value.end(), std::bind(std::isspace<BLV_TCHAR>, std::placeholders::_1, std::locale::classic())), value.end());
		BLV_TSTRINGSTREAM vals_ss(value);
		typename std::initializer_list<ValT>::iterator vals_it=vals.begin();
		while (std::getline(vals_ss, value, comma)&&vals_it!=vals.end()) {
			ValList.push_back(std::make_pair(value, *vals_it));
			vals_it++;
		}
	}
public:
	template <bool U=unique, typename std::enable_if<!U>::type* = nullptr>
	BasicLabeledValues(const BLV_TCHAR* vals_str, const std::initializer_list<ValT> &vals): ValList(),
		comma(std::use_facet<std::ctype<BLV_TCHAR>>(std::locale::classic()).widen(',')), space(std::use_facet<std::ctype<BLV_TCHAR>>(std::locale::classic()).widen(' ')) {
		FillValList(vals_str, vals);
	}
	template <bool U=unique, typename std::enable_if<U>::type* = nullptr>
	BasicLabeledValues(const BLV_TCHAR* vals_str, const std::initializer_list<ValT> &vals): ValList(), 
		comma(std::use_facet<std::ctype<BLV_TCHAR>>(std::locale::classic()).widen(',')), space(std::use_facet<std::ctype<BLV_TCHAR>>(std::locale::classic()).widen(' ')) {
		FillValList(vals_str, vals);
		std::sort(ValList.begin(), ValList.end(), [](const std::pair<BLV_TSTRING, ValT> &L, const std::pair<BLV_TSTRING, ValT> &R){
			return L.second<R.second;
		});
		ValList.erase(std::unique(ValList.begin(), ValList.end(), [](const std::pair<BLV_TSTRING, ValT> &L, const std::pair<BLV_TSTRING, ValT> &R){
			return L.second==R.second;
		}), ValList.end());
	}
	ValT Find(const BLV_TSTRING& label, const ValT& def_val=ValT()) {
		for (std::pair<BLV_TSTRING, ValT> &val_pair: ValList)
			if (val_pair.first==label) return val_pair.second;
		return def_val;
	}
	size_t Size() {
		return ValList.size();
	}
	size_t Matches(std::function<bool(const BLV_TSTRING&, ArgT)> enum_function) {
		size_t count=0;
		for (std::pair<BLV_TSTRING, ValT> &val_pair: ValList)
			if (enum_function(val_pair.first, val_pair.second)) count++;
		return count;
	}
	size_t Matches(const ValT& value) {
		size_t count=0;
		for (std::pair<BLV_TSTRING, ValT> &val_pair: ValList)
			if (value==val_pair.second) count++;
		return count;
	}
	bool Values(std::function<bool(const BLV_TSTRING&, ArgT)> enum_function) {
		for (std::pair<BLV_TSTRING, ValT> &val_pair: ValList)
			if (enum_function(val_pair.first, val_pair.second)) return true;
		return false;
	}
	BLV_TSTRING Values() {
		BLV_TSTRING result;
		for (std::pair<BLV_TSTRING, ValT> &val_pair: ValList)
			if (result.empty()) result=val_pair.first;
				else result+=BLV_TSTRING({comma, space})+val_pair.first;
		return result;
	}
	size_t Enums(const ValT& enums, std::function<bool(const BLV_TSTRING&, ArgT, bool)> enum_function) {
		size_t count=0;
		for (std::pair<BLV_TSTRING, ValT> &val_pair: ValList)
			if (enums==val_pair.second&&(count++, enum_function(val_pair.first, val_pair.second, false))) return count;
		if (!count)
			enum_function(BLV_TSTRING(), enums, true);
		return count;
	}
	BLV_TSTRING Enums(const ValT& enums) {
		BLV_TSTRING result;
		for (std::pair<BLV_TSTRING, ValT> &val_pair: ValList) 
			if (enums==val_pair.second) {
				if (result.empty()) result=val_pair.first;
					else result+=BLV_TSTRING({comma, space})+val_pair.first;
			}
		return result;
	}
	BLV_TSTRING operator()(const ValT& value, const BLV_TSTRING& def_label=BLV_TSTRING()) {
		for (std::pair<BLV_TSTRING, ValT> &val_pair: ValList)
			if (value==val_pair.second) return val_pair.first;
		return def_label;
	}
	BLV_TSTRING operator()(const ValT& value, std::function<BLV_TSTRING(ArgT)> def_fn) {
		for (std::pair<BLV_TSTRING, ValT> &val_pair: ValList)
			if (value==val_pair.second) return val_pair.first;
		return def_fn(value);
	}
	size_t Flags(ValT flags, std::function<bool(const BLV_TSTRING&, ArgT, bool)> enum_function) {
		ValT checked_flags=0;
		size_t count=0;
		for (std::pair<BLV_TSTRING, ValT> &val_pair: ValList)
			if ((flags&val_pair.second)==val_pair.second) {
				checked_flags|=val_pair.second;
				count++;
				if (enum_function(val_pair.first, val_pair.second, false)) return count;
			}
		if (checked_flags!=flags) {
			flags&=~checked_flags;
			enum_function(BLV_TSTRING(), flags, true);
		}
		return count;
	}
	BLV_TSTRING Flags(const ValT& flags) {
		BLV_TSTRING result;
		for (std::pair<BLV_TSTRING, ValT> &val_pair: ValList)
			if ((flags&val_pair.second)==val_pair.second) {
				if (result.empty()) result=val_pair.first;
					else result+=BLV_TSTRING({comma, space})+val_pair.first;
			}
		return result;
	}
};

#ifdef _WIN32
typedef BasicLabeledValues<DWORD> LabeledValues;
#else
typedef BasicLabeledValues<unsigned long> LabeledValues;
#endif

#endif //LABELED_VALUES_HPP
