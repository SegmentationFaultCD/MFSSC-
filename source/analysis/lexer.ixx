export module MFSSCPP.analysis.lexical_analysis;
import std;
export class lexer {
	// 词法解析器
public:
	enum class type {
		Identifier,					// 标识符, 如命名空间名，函数名，变量名，模块名
		Number,						// 0, 1, 2
		String,						// "SSS"
		Operator,					// + - * /  :: >> << = () :
		Keyword,					// i32, i64, char, export, return
	};
	struct Table {
		explicit Table(std::pair<std::int64_t, std::string_view> stream) : stream{ stream } {}
		virtual ~Table() = default;
	public:
		auto check() -> std::optional<std::tuple<type, std::string, std::int64_t>> {
			if (auto catch_result = this->catch_word(this->stream); catch_result.has_value()) {
				auto [final_position, symbol] = catch_result.value();
				if (auto check_result = this->do_check(symbol); check_result.has_value()) {
					return std::tuple<type, std::string, std::int64_t>{ check_result.value(), symbol, final_position - 1 };
				}
			}
			return {};
		}
	protected:
		virtual auto do_check(std::string_view symbol) -> std::optional<type> = 0;
		virtual auto catch_word(std::pair<std::int64_t, std::string_view> stream) -> std::optional<std::pair<std::int64_t, std::string>> = 0;
	private:
		std::pair<std::int64_t, std::string_view> stream;
	};
	struct OperatorTable : Table {
		explicit OperatorTable(std::pair<std::int64_t, std::string_view> stream) : Table{ stream } {}
	private:
		virtual auto do_check(std::string_view symbol) -> std::optional<type> override {
			if (symbol == "+" || symbol == "::" || symbol == "->" || symbol == ":") return type::Operator;
			else return {};
		}
		virtual auto catch_word(std::pair<std::int64_t, std::string_view> stream) -> std::optional<std::pair<std::int64_t, std::string>> override {
			auto start = stream.first;
			while (
				stream.second[start] == '+' ||
				stream.second[start] == '-' ||
				stream.second[start] == '>' ||
				stream.second[start] == '<' ||
				stream.second[start] == ':' ||
				stream.second[start] == '(' ||
				stream.second[start] == ')') ++start;
			if (start - stream.first) {
				return std::pair<std::int64_t, std::string>{ start, stream.second.substr(stream.first, start - stream.first) };
			}
			else {
				return {};
			}
		}
	public:
		virtual ~OperatorTable(void) = default;
	};
	struct KeywordTable : Table {
		constexpr static std::string_view i32_type = "i32";
		constexpr static std::string_view char_type = "char";
		constexpr static std::string_view export_symbol = "export";
		constexpr static std::string_view return_symbol = "return";
		constexpr static std::string_view module_symbol = "module";
		constexpr static std::string_view import_symbol = "import";
		explicit KeywordTable(std::pair<std::int64_t, std::string_view> stream) : Table{ stream } {}
	private:
		virtual auto do_check(std::string_view symbol) -> std::optional<type> override {
			if (symbol == i32_type ||
				symbol == char_type ||
				symbol == export_symbol ||
				symbol == return_symbol ||
				symbol == module_symbol ||
				symbol == import_symbol) {
				return type::Keyword;
			}
			else {
				return {};
			}
		}
		virtual auto catch_word(std::pair<std::int64_t, std::string_view> stream) -> std::optional<std::pair<std::int64_t, std::string>> override {
			auto start = stream.first;
			if (((stream.second[start] >= 'A' && stream.second[start] <= 'Z') || (stream.second[start] >= 'a' && stream.second[start] <= 'z'))) {
				while (
					(stream.second[start] >= 'A' && stream.second[start] <= 'Z') ||
					(stream.second[start] >= 'a' && stream.second[start] <= 'z') ||
					(stream.second[start] >= '0' && stream.second[start] <= '9') ||
					stream.second[start] == '_') ++start;
			}

			if (start - stream.first) {
				return std::pair<std::int64_t, std::string>{ start, stream.second.substr(stream.first, start - stream.first) };
			}
			else {
				return {};
			}
		}
	public:
		virtual ~KeywordTable(void) = default;

	};
	struct NumberTable : Table {
		explicit NumberTable(std::pair<std::int64_t, std::string_view> stream) : Table{ stream } {}
	private:
		virtual auto do_check(std::string_view symbol) -> std::optional<type> override {
			for (auto _ : symbol | std::ranges::views::filter([](auto const& ch) { return !(ch >= '0' && ch <= '9'); })) {
				return {};
			}
			return type::Number;
		}
		virtual auto catch_word(std::pair<std::int64_t, std::string_view> stream) -> std::optional<std::pair<std::int64_t, std::string>> override {
			auto start = stream.first;
			while (stream.second[start] >= '0' && stream.second[start] <= '9') ++start;
			if (start - stream.first) {
				return std::pair<std::int64_t, std::string>{ start, stream.second.substr(stream.first, start - stream.first) };
			}
			else {
				return {};
			}
		}

	public:
		virtual ~NumberTable(void) = default;
	};
	struct StringTable : Table {
		explicit StringTable(std::pair<std::int64_t, std::string_view> stream) : Table{ stream } {}
	private:
		virtual auto do_check(std::string_view symbol) -> std::optional<type> override {
			if (symbol[0] == '\"') {
				auto position = symbol.find_first_of('\"', 1);
				if (position == std::string_view::npos) {
					// 不以 " 结尾的情况
					return {};
				}
				return type::String;
			}
			return {};
		}
		virtual auto catch_word(std::pair<std::int64_t, std::string_view> stream) -> std::optional<std::pair<std::int64_t, std::string>> override {
			auto start = stream.first;
			if (stream.second[start] == '\"') {
				start++;
				while (stream.second[start] != '\"') ++start;
			}
			if (start - stream.first) {
				return std::pair<std::int64_t, std::string>{ start + 1, stream.second.substr(stream.first, start - stream.first + 1 /*包括最后一个"*/) };
			}
			else {
				return {};
			}
		}
	public:
		virtual ~StringTable(void) = default;
	};
	struct IdentifierTable : Table {
		explicit IdentifierTable(std::pair<std::int64_t, std::string_view> stream) : Table{ stream } {}
	private:
		virtual auto do_check([[maybe_unused]] std::string_view symbol) -> std::optional<type> override {
			return type::Identifier;
		}
		virtual auto catch_word(std::pair<std::int64_t, std::string_view> stream) -> std::optional<std::pair<std::int64_t, std::string>> override {
			auto start = stream.first;
			while (((stream.second[start] >= 'A' && stream.second[start] <= 'Z') || (stream.second[start] >= 'a' && stream.second[start] <= 'z'))) ++start;
			if (start - stream.first) {
				return std::pair<std::int64_t, std::string>{ start, stream.second.substr(stream.first, start - stream.first) };
			}
			else {
				return {};
			}
		}
	public:
		virtual ~IdentifierTable(void) = default;
	};
	using token = std::pair<type, std::string>;			// 词法分析使用的抽象符号
public:
	explicit lexer(std::map<std::string, std::string>& string_stream) {
		for (auto& file_and_string : string_stream) {
			for (std::int64_t position{}; position < file_and_string.second.length(); ++position) {
				if (file_and_string.second[position] == '/' && file_and_string.second[position + 1] == '/') {
					// 注释
					position = file_and_string.second.find_first_of('\n', position);
					if (position == std::string::npos) {
						throw "Arrived at the end";

					}
					continue;

				}
				if (file_and_string.second[position] == ' ' ||
					file_and_string.second[position] == '\n' ||
					file_and_string.second[position] == '\t'
					) {
					// 无用字符
					continue;
				}

				// 字符串扫描
				auto check_table = [&](Table* token_checker) {
					auto result = token_checker->check();
					if (result.has_value()) {
						auto token_symbol = std::get<std::string>(result.value());
						auto token_type = std::get<type>(result.value());
						position = std::get<std::int64_t>(result.value());
						tokens[file_and_string.first].push_back({ token_type, token_symbol });
						delete token_checker;
						return true;
					}
					else {
						delete token_checker;
						return false;
					}
					};

				if (check_table(new StringTable{ { position, file_and_string.second } }) ||
					check_table(new KeywordTable{ { position, file_and_string.second } }) ||
					check_table(new NumberTable{ { position, file_and_string.second } }) ||
					check_table(new OperatorTable{ { position, file_and_string.second } }) ||
					check_table(new IdentifierTable{ { position, file_and_string.second } })) {
					continue;
				}
			}
		}
		/*for (auto i : tokens) {
			for (auto j : i.second) {
				std::println("[{} {}]", (int)j.first, j.second.c_str());
			}
		}*/
	}
private:
	std::map<std::string, std::vector<token>> tokens;
};
