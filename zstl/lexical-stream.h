#ifndef _ZXY_LEXICAL_STREAM_H
#define _ZXY_LEXICAL_STREAM_H

#include <string.h>
#include <string>
#include <stdint.h>
#include <stdio.h>
#include "config.h"
#include "type_traits.h"
#include "algorithm.h"
#include <assert.h>
#include "string_view.h"
#include "util/noncopyable.h"

namespace zstl {

namespace detail{
/**
 * @class FixedBuffer
 * @tparam SZ size of buffer
 * maintain a fixed-size buffer
 */
template<unsigned SZ>
class FixedBuffer
{
	using Self = FixedBuffer;
public:
	FixedBuffer() 
		: len_(0) 
	{ }
	
	//prohibit modify througt data()
	char const* data() const noexcept { return data_; }
	
	// length, avaliable space
	unsigned len() const noexcept 
	{ return len_; }

	unsigned avali() const noexcept 
	{ return SZ - len_; }
	
	char const* end() const noexcept 
	{ return data_ + SZ; }

	// append
	void append(char const* str, unsigned len) noexcept {
		if(len < avali()) {
			memcpy(cur(), str, len);
			len_ += len;
			*cur() = 0;
		}
	}
	
	// inplace modify
	// cur() -> set()
	char* cur() noexcept
	{ return data_ + len_; }

	void reset() noexcept
	{ len_ = 0; }

	void set(unsigned diff) noexcept
	{ 
		len_ += diff; 
		assert(len_ < SZ);
	}
	
	void swap(Self& other) noexcept{
		STL_SWAP(data_, other.data_);
		STL_SWAP(len_, other.len_);
	}

	void zero() noexcept
	{ memset(data_, 0, SZ); }
private:
	char data_[SZ];
	unsigned len_;
};

template<unsigned SZ>
void swap(FixedBuffer<SZ> const& lhs, FixedBuffer<SZ> const& rhs) noexcept(noexcept(lhs.swap(rhs)))
{ lhs.swap(rhs); }

} // namespace detail

template<unsigned SZ>
class LexicalStream : noncopyable
{
	using Self = LexicalStream;
	using Buffer = detail::FixedBuffer<SZ>;
public:

	LexicalStream() = default;
	LexicalStream(LexicalStream&&) noexcept;
	LexicalStream& operator=(LexicalStream&&) noexcept;	

	void append(char const* buf, unsigned len) noexcept 
	{ buffer_.append(buf, len); }
	
	void reset() noexcept
	{ buffer_.reset(); }

	char const* data() const noexcept
	{ return buffer_.data(); }
	
	Buffer& buffer() noexcept
	{ return buffer_; }
	
	unsigned size() const noexcept
	{ return buffer_.len(); }
	
	unsigned maxsize() const noexcept
	{ return SZ; }

	Self& operator<<(bool);
	Self& operator<<(char);

	Self& operator<<(short);
	Self& operator<<(unsigned short);
	Self& operator<<(int);
	Self& operator<<(unsigned);
	Self& operator<<(long);
	Self& operator<<(unsigned long);
	Self& operator<<(long long);
	Self& operator<<(unsigned long long);
	Self& operator<<(unsigned char);

	Self& operator<<(float f)
	{ return *this << static_cast<double>(f); }

	Self& operator<<(double);
	
	Self& operator<<(char const*);
	Self& operator<<(std::string const& str);
	Self& operator<<(string_view const&);

	Self& operator<<(void const*);
private:
	Buffer buffer_;

// static
public:
	static constexpr unsigned kMaxIntSize = 32;
	static constexpr unsigned kMaxFloatingSize = 324;
};

template<unsigned SZ>
constexpr unsigned LexicalStream<SZ>::kMaxFloatingSize;

template<unsigned SZ>
LexicalStream<SZ>::LexicalStream(LexicalStream&& other) noexcept
	: buffer_(other.buffer())
{ }

template<unsigned SZ>
LexicalStream<SZ>& LexicalStream<SZ>::operator=(LexicalStream&& other) noexcept
{
	buffer_.swap(other.buffer_);	
	return *this;
}

namespace detail {
	const char digits[] = {
		'9', '8', '7', '6', '5', '4', '3', '2', '1',
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
	};
	
	extern char const* pzero;
	
	template<
		typename T,
		typename = STL_ Enable_if_t<
			STL_ Is_integral<T>::value
		>>
	unsigned int2Str(char* buf, T integer){
		char* end = buf;

		do{
			T left = integer % 10;
			integer /= 10;
			*(end++) = *(pzero + left);
		}while(integer != 0);
		
		if(integer < 0)
			*(end++) = '-';
	
		*end = 0;
		STL_ reverse(buf, end);
		
		return end - buf;
	}

} // namespace detail

template<unsigned SZ>
auto LexicalStream<SZ>::operator<<(char c)->
	Self&
{
	append(&c, 1);
	return *this;
}

template<unsigned SZ>
auto LexicalStream<SZ>::operator<<(bool b)->
	Self&
{
	if(b)
		append("True", 4);
	else
		append("False", 5);
	
	return *this;
}

#define LEXICALSTREAMg_OPERATOR_LEFT_SHIFT(type) \
template<unsigned SZ> \
auto LexicalStream<SZ>::operator<<(type i)->\
	Self&\
{\
	if(buffer_.avali() > kMaxIntSize)\
		buffer_.set(detail::int2Str(buffer_.cur(), i)); \
	return *this; \
}

LEXICALSTREAMg_OPERATOR_LEFT_SHIFT(unsigned char)
LEXICALSTREAMg_OPERATOR_LEFT_SHIFT(short)
LEXICALSTREAMg_OPERATOR_LEFT_SHIFT(unsigned short)
LEXICALSTREAMg_OPERATOR_LEFT_SHIFT(int)
LEXICALSTREAMg_OPERATOR_LEFT_SHIFT(unsigned)
LEXICALSTREAMg_OPERATOR_LEFT_SHIFT(long)
LEXICALSTREAMg_OPERATOR_LEFT_SHIFT(unsigned long)
LEXICALSTREAMg_OPERATOR_LEFT_SHIFT(long long)
LEXICALSTREAMg_OPERATOR_LEFT_SHIFT(unsigned long long)

template<unsigned SZ>
auto LexicalStream<SZ>::operator<<(double d)->
	Self&
{
	if(buffer_.avali() > kMaxFloatingSize){
		int len = snprintf(buffer_.cur(), kMaxFloatingSize, "%.12g", d);

		buffer_.set(len);
	}

	return *this;
}

template<unsigned SZ>
auto LexicalStream<SZ>::operator<<(char const* str)->
	Self&
{
	if(str){
		append(str, strlen(str));
	}else{
		append("(null)", 6);
	}

	return *this;
}

template<unsigned SZ>
auto LexicalStream<SZ>::operator<<(std::string const& str) ->
	Self&
{
	append(str.data(), str.size());
	return *this;
}

template<unsigned SZ>
auto LexicalStream<SZ>::operator<<(string_view const& sv) ->
	Self&
{
	append(sv.data(), sv.size());
	return *this;
}

class Fmt
{
public:
	template<typename T>
	explicit Fmt(char const* str, T val)
	{
		static_assert(STL_ Is_arithmetic<T>::value, 
				"Fmt: Give value's type must be integer or floating-point");
		snprintf(buf_, sizeof buf_, str, val);
	}

	explicit Fmt(std::string const& str)
		: Fmt(str.c_str(), str.size()) 
	{ }
	
	char const* buf() const noexcept
	{ return buf_; }

	unsigned len() const noexcept
	{ return len_; }

private:
	char buf_[32];
	unsigned len_;
};

template<unsigned SZ, typename T>
LexicalStream<SZ>& operator<<(LexicalStream<SZ>& stream, Fmt const& fmt)
{
	stream.append(fmt.buf(), fmt.len());
	return stream;
}

constexpr int64_t kSmallStreamSize = 4000;
constexpr int64_t kLargeStreamSize = 4000 * 1000;

using SmallLexicalStream = LexicalStream<kSmallStreamSize>;
using LargeLexicalStream = LexicalStream<kLargeStreamSize>;

extern SmallLexicalStream lstream;


} // namespace zstl

#endif // _ZXY_LEXICAL_STREAM_H
