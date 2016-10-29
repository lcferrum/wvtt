#ifndef TEE_BUF_HPP
#define TEE_BUF_HPP

#include <string>
#include <ostream>
#include <streambuf>
#include <functional>

#define CreateOstreamTeeBuf(s, n, f) OstreamTeeBuf<decltype(s)> n(s, f)

template <class OstrT>
class OstreamTeeBuf: private std::basic_streambuf<typename OstrT::char_type> {
	typedef std::function<void(const typename OstrT::char_type* s, std::streamsize n)> TeeCallbackType;
private:
	OstrT &ostr;
	std::basic_streambuf<typename OstrT::char_type> *orig_buf;
	TeeCallbackType tee_callback;
	
	//Hacky hacky hacky...
	//Class tries to redirect all the output to original ostream/wcostream buffer
	//This is not intended behaviour of basic_streambuf streambuf descendant
	//We are expected to provide our own buffer and handle overflow and sync functions as law abiding descendant
	//And only while behaving like proper basic_streambuf we are allowed to do whatever we want with data passed (like forwarding it to original buffer via public functions)
	//There is a perfect reason for this - we don't know how basic_streambuf will be used and by who
	//By conforming to the standard we are ensuring that whoever use it, if he also conforms to standard, will get expected results
	//But no... we will cut off some corners and just redirect protected members to it's equivalents of original buffer via hacky means
	//Actually it works and if we redirect all of the protected members we are pretty safe
	//Observations on behaviour of GCC's ostream implementaion show that it's enough to redirect only protected functions listed below (at least for this particular implementation)
	//(BTW, somehow ostream calls these protected functions directly...)
	
	std::streamsize xsputn(const typename OstrT::char_type* s, std::streamsize n) {
		if (tee_callback) tee_callback(s, n);
		((OstreamTeeBuf<OstrT>*)orig_buf)->xsputn(s, n);
		return n;
	};
	
	typename std::char_traits<typename OstrT::char_type>::int_type overflow(typename std::char_traits<typename OstrT::char_type>::int_type c) {
		if (tee_callback) tee_callback((const typename OstrT::char_type*)&c, 1);
		((OstreamTeeBuf<OstrT>*)orig_buf)->overflow(c);
		return c==std::char_traits<typename OstrT::char_type>::eof()?std::char_traits<typename OstrT::char_type>::not_eof(c):c;
	};
	
	int sync() {
		((OstreamTeeBuf<OstrT>*)orig_buf)->sync();
		return 0;
	};
public:
	OstreamTeeBuf(OstrT &ostr, TeeCallbackType tee_callback): ostr(ostr), orig_buf(ostr.rdbuf(this)), tee_callback(tee_callback) {};
	
	void Deactivate() {
		if (orig_buf) {
			ostr.rdbuf(orig_buf);
			orig_buf=NULL;
		}
	};
	
	~OstreamTeeBuf() {
		Deactivate();
	};
};

#endif //TEE_BUF_HPP
