#pragma once

#include <boost/leaf.hpp>
#include <boost/leaf/exception.hpp>

#include <fmt/core.h>

namespace vvv
{
	class error_description
	{
		const char*		  m_description{nullptr};
		const std::string m_description_str;

	public:
		error_description(const char* const str) : m_description{str} { }
		error_description(std::string&& str) : m_description_str{std::move(str)} { }
		error_description() : m_description{""} { }

		operator const char*() const
		{
			return (m_description != nullptr) ? m_description : m_description_str.c_str();
		}

		operator std::string_view() const
		{
			return (m_description != nullptr) ? std::string_view(m_description) : std::string_view(m_description_str);
		}
	};

	struct common_error
	{
		const error_description m_description;
	};

	struct runtime_error
	{
		struct not_specified : common_error
		{
		};
	};

	namespace leaf = ::boost::leaf;

	template<typename... T_ARGS>
	using result = leaf::result<T_ARGS...>;

} // namespace vvv

// Default boost leaf macros fail when nested due to duplicate symbol names

#define vvv_LEAF_TOKEN_PASTE(x, y, sep, c) x##y##sep##c
#define vvv_LEAF_TOKEN_PASTE2(x, y, c)	   vvv_LEAF_TOKEN_PASTE(x, y, _, c)
#define vvv_LEAF_TMP					   vvv_LEAF_TOKEN_PASTE2(vvv_leaf_tmp_, __LINE__, __COUNTER__)

#define vvv_LEAF_ASSIGN2(v, r, VAR)                                                                                                                                                \
	auto&& VAR = r;                                                                                                                                                                \
	static_assert(                                                                                                                                                                 \
		::boost::leaf::is_result_type<typename std::decay<decltype(VAR)>::type>::value,                                                                                            \
		"vvv_LEAF_ASSIGN/vvv_LEAF_AUTO requires a result object as the second argument (see is_result_type)");                                                                     \
	if (!VAR)                                                                                                                                                                      \
		return VAR.error();                                                                                                                                                        \
	v = std::forward<decltype(VAR)>(VAR).value()

#define vvv_LEAF_ASSIGN(v, r) vvv_LEAF_ASSIGN2(v, r, vvv_LEAF_TMP)

#define vvv_LEAF_AUTO(v, r) vvv_LEAF_ASSIGN(auto v, r)

#define vvv_CLEAF_ASSIGN2(v, r, CLEANUP, VAR)                                                                                                                                      \
	auto&& VAR = r;                                                                                                                                                                \
	static_assert(                                                                                                                                                                 \
		::boost::leaf::is_result_type<typename std::decay<decltype(VAR)>::type>::value,                                                                                            \
		"vvv_LEAF_ASSIGN/vvv_LEAF_AUTO requires a result object as the second argument (see is_result_type)");                                                                     \
	if (!VAR)                                                                                                                                                                      \
	{                                                                                                                                                                              \
		CLEANUP();                                                                                                                                                                 \
		return VAR.error();                                                                                                                                                        \
	}                                                                                                                                                                              \
	v = std::forward<decltype(VAR)>(VAR).value()

#define vvv_CLEAF_ASSIGN(v, r, CLEANUP) vvv_CLEAF_ASSIGN2(v, r, CLEANUP, vvv_LEAF_TMP)

#define vvv_CLEAF_AUTO(v, r, CLEANUP) vvv_CLEAF_ASSIGN(auto v, r, CLEANUP)

#if vvv_LEAF_CFG_GNUC_STMTEXPR

#define vvv_LEAF_CHECK2(r, VAR)                                                                                                                                                    \
	({                                                                                                                                                                             \
		auto&& VAR = (r);                                                                                                                                                          \
		static_assert(::boost::leaf::is_result_type<typename std::decay<decltype(VAR)>::type>::value, "vvv_LEAF_CHECK requires a result object (see is_result_type)");             \
		if (!VAR)                                                                                                                                                                  \
			return VAR.error();                                                                                                                                                    \
		std::move(VAR);                                                                                                                                                            \
	}).value()

#define vvv_LEAF_CHECK(r) vvv_LEAF_CHECK2(r, vvv_LEAF_TMP)

#else

#define vvv_LEAF_CHECK2(r, VAR)                                                                                                                                                    \
	{                                                                                                                                                                              \
		auto&& VAR = (r);                                                                                                                                                          \
		static_assert(::boost::leaf::is_result_type<typename std::decay<decltype(VAR)>::type>::value, "vvv_LEAF_CHECK requires a result object (see is_result_type)");             \
		if (!VAR)                                                                                                                                                                  \
			return VAR.error();                                                                                                                                                    \
	}

#define vvv_LEAF_CHECK(r) vvv_LEAF_CHECK2(r, vvv_LEAF_TMP)

#endif

#define vvv_AUTO	  vvv_LEAF_AUTO
#define vvv_CHECK	  vvv_LEAF_CHECK
#define vvv_NEW_ERROR BOOST_LEAF_NEW_ERROR
#define vvv_ASSIGN	  vvv_LEAF_ASSIGN

#define vvv_THROW2(v, r, VAR)                                                                                                                                                      \
	auto&& VAR = r;                                                                                                                                                                \
	static_assert(                                                                                                                                                                 \
		::boost::leaf::is_result_type<typename std::decay<decltype(VAR)>::type>::value,                                                                                            \
		"vvv_LEAF_ASSIGN/vvv_LEAF_AUTO requires a result object as the second argument (see is_result_type)");                                                                     \
	if (!VAR)                                                                                                                                                                      \
		throw std::exception();                                                                                                                                                    \
	v = std::forward<decltype(VAR)>(VAR).value()

#define vvv_THROW(v, r) vvv_THROW2(v, r, vvv_LEAF_TMP)

#define vvv_ASSERT(EXPR, ERR)                                                                                                                                                      \
	if (!(EXPR)) [[unlikely]]                                                                                                                                                      \
	{                                                                                                                                                                              \
		return vvv_NEW_ERROR(ERR);                                                                                                                                                 \
	}
/**/

#define vvv_EXPECT_VALID(POINTER)                                                                                                                                                  \
	if ((POINTER) == nullptr) [[unlikely]]                                                                                                                                         \
	{                                                                                                                                                                              \
		return BOOST_LEAF_NEW_ERROR(runtime_error::not_specified{});                                                                                                               \
	}
/**/

#define vvv_err_not_specified(VALUE)   vvv_NEW_ERROR(vvv::runtime_error::not_specified{VALUE});
#define vvv_throw_not_specified(VALUE) vvv::leaf::throw_exception(vvv::runtime_error::not_specified{VALUE});
