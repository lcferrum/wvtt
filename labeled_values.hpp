#ifndef LABELED_VALUES_HPP
#define LABELED_VALUES_HPP

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <initializer_list>

#ifdef _WIN32
#include <windows.h>	//TCHAR, TEXT and DWORD
#else
#define TEXT(c) c		//On non-Windows targets assume that SBCS/MBCS character set is in use
#endif

#define LABELED_VALUES_ARG(...)			false, TEXT(#__VA_ARGS__), {__VA_ARGS__}
#define UNIQUE_LABELED_VALUES_ARG(...)	true, TEXT(#__VA_ARGS__), {__VA_ARGS__}
template <typename ValT>
class BasicLabeledValues {
#if defined(UNICODE) && defined(_WIN32)
	typedef std::wstring TSTRING;
	typedef std::wstringstream TSTRINGSTREAM;
#else
	typedef std::string TSTRING;
	typedef std::stringstream TSTRINGSTREAM;
#endif
#ifndef _WIN32
	typedef char TCHAR;
#endif
private:
	std::vector<std::pair<TSTRING, ValT>> ValList;
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
		if (unique_vals) {
			std::sort(ValList.begin(), ValList.end(), [](const std::pair<TSTRING, ValT> &L, const std::pair<TSTRING, ValT> &R){
				return L.second<R.second;
			});
			ValList.erase(std::unique(ValList.begin(), ValList.end(), [](const std::pair<TSTRING, ValT> &L, const std::pair<TSTRING, ValT> &R){
				return L.second==R.second;
			}), ValList.end());
		}
	}
	ValT Find(const TSTRING& label, ValT def_val) {
		for (std::pair<TSTRING, ValT> &val_pair: ValList)
			if (val_pair.first==label) return val_pair.second;
		return def_val;
	}
	size_t Size() {
		return ValList.size();
	}
	size_t Values(std::function<bool(const TSTRING&, ValT, size_t)> enum_function) {
		size_t count=0;
		for (std::pair<TSTRING, ValT> &val_pair: ValList)
			if (enum_function(val_pair.first, val_pair.second, count++)) break;
		return count;
	}
	TSTRING Values() {
		TSTRING result;
		Values([&result](const TSTRING& label, ValT value, size_t idx){
			result+=(idx?TSTRING(TEXT(", ")):TSTRING())+label;
			return false;
		});
		return result;
	}
	size_t Enums(ValT enums, std::function<bool(const TSTRING&, ValT, bool, size_t)> enum_function) {
		size_t count=0;
		for (std::pair<TSTRING, ValT> &val_pair: ValList)
			if (enums==val_pair.second&&enum_function(val_pair.first, val_pair.second, false, count++)) return count;
		if (!count)
			enum_function(TEXT(""), enums, true, 0);
		return count;
	}
	TSTRING Enums(ValT enums) {
		TSTRING result;
		Enums(enums, [&result](const TSTRING& label, ValT value, bool unknown, size_t idx){
			if (!unknown)
				result+=(idx?TSTRING(TEXT(", ")):TSTRING())+label;
			return false;
		});
		return result;
	}
	TSTRING operator()(ValT value) {
		TSTRING result;
		Enums(value, [&result](const TSTRING& label, ValT value, bool unknown, size_t idx){
			if (!unknown) {
				result=label;
				return true;
			} else
				return false;
		});
		return result;
	}
	size_t Flags(ValT flags, std::function<bool(const TSTRING&, ValT, bool, size_t)> enum_function) {
		ValT checked_flags=0;
		size_t count=0;
		for (std::pair<TSTRING, ValT> &val_pair: ValList)
			if ((flags&val_pair.second)==val_pair.second) {
				checked_flags|=val_pair.second;
				if (enum_function(val_pair.first, val_pair.second, false, count++)) return count;
			}
		if (checked_flags!=flags) {
			flags&=~checked_flags;
			enum_function(TEXT(""), flags, true, count);
		}
		return count;
	}
	TSTRING Flags(ValT flags) {
		TSTRING result;
		Flags(flags, [&result](const TSTRING& label, ValT value, bool unknown, size_t idx){
			if (!unknown)
				result+=(idx?TSTRING(TEXT(", ")):TSTRING())+label;
			return false;
		});
		return result;
	}
};
#ifdef _WIN32
typedef BasicLabeledValues<DWORD> LabeledValues;
#else
#undef TEXT
typedef BasicLabeledValues<unsigned long> LabeledValues;
#endif

#endif //LABELED_VALUES_HPP
