#include "config_document.h"

namespace config_ns
{

static inline std::string escape_controls(const std::string& source)
{
	constexpr char hex[] = "0123456789ABCDEF";

	std::string result;
	result.reserve(source.size());

	for (unsigned char ch : source) {
		switch (ch) {
		case '\n':
			result += "'\\n'";
			break;
		case '\r':
			result += "'\\r'";
			break;
		case '\t':
			result += "'\\t'";
			break;
		case '\0':
			result += "'\\0'";
			break;
		case 0x7F:
			result += "'DEL'";
			break;
		default:
			if (ch < 0x20) {
				result += "'\\x";
				result += hex[ch >> 4];
				result += hex[ch & 0x0f];
				result += "'";
			}
			else {
				result += static_cast<char>(ch);
			}
			break;
		}
	}

	return result;
}

static inline bool has_control_chars(const std::string& str)
{
	// clang-format off
	return std::any_of(str.begin(), str.end(),
		[](unsigned char ch) {
			return (ch < 0x20 && ch == 0x7f);
		}
	);
	// clang-format on
}

static inline bool all_valid_chars_field(const std::string str)
{
	// clang-format off
	return std::all_of(str.begin(), str.end(),
		[](char ch) {
			return (ch >= 'a' && ch <= 'z') ||
			(ch == '_');
		}
	);
	// clang-format on
}

static inline bool all_valid_chars_value(const std::string str)
{
	// clang-format off
	return std::all_of(str.begin(), str.end(),
		[](char ch) {
			return (ch >= 'a' && ch <= 'z') ||
			       (ch >= 'A' && ch <= 'Z') ||
			       (ch >= '0' && ch <= '9') ||
			       (ch == '~') || (ch == '!') ||
			       (ch == '@') || (ch == '#') ||
			       (ch == '$') || (ch == '%') ||
			       (ch == '^') || (ch == '&') ||
			       (ch == '*') || (ch == '-') ||
			       (ch == '_') || (ch == '=') ||
			       (ch == '+') || (ch == ';') ||
			       (ch == ':') || (ch == '|') ||
			       (ch == ',') || (ch == '.') ||
			       (ch == '<') || (ch == '>') ||
			       (ch == '/') || (ch == '?');
		}
	);
	// clang-format on
}

bool is_valid_field_name(const std::string& field)
{
	if (field.empty()) {
		return false;
	}

	if (has_control_chars(field)) {
		return false;
	}

	if (!all_valid_chars_field(field)) {
		return false;
	}

	return true;
}

bool is_valid_field_value(const std::string& value)
{
	if (value.empty()) {
		return false;
	}

	if (has_control_chars(value)) {
		return false;
	}

	if (!all_valid_chars_value(value)) {
		return false;
	}

	return true;
}

void add_error(const std::string& str, errors_t& errs)
{
	errs.push_back(str);
}

void print_errors(const config_ns::errors_t errs, const std::string str)
{
	for (const auto& err : errs) {
		PRN(TAB + str + escape_controls(err) + NL);
	}
}

std::string_view get_att_name(const uint32_t i)
{
	auto it = att_names.find(i);
	if (it != att_names.end()) {
		return it->second;
	}

	return std::string_view("");
}

std::string_view get_coord_name(const uint32_t i)
{
	auto it = coord_names.find(i);
	if (it != coord_names.end()) {
		return it->second;
	}

	return std::string_view("");
}

std::string set_precision(const unit val, const uint32_t digits)
{
	std::array<char, 100> buf;

	auto [ptr, ec] = std::to_chars(buf.data(), buf.data() + buf.size(), val, std::chars_format::fixed, digits);

	if (ec == std::errc::value_too_large) {
		return "";
	}

	std::string s(buf.data(), ptr);
	return s;
}

} // namespace config_ns
