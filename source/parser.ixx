export module MSFFCPP.Parser;
//import MSFFCPP.Parser;
//
//std::vector<std::string> files;
//std::map<std::string, std::string> texts;
//export auto main(int argc, char** argv, char** envp) -> int {
//	// 从命令行读取文件
//	for (auto i = 0; i < argc - 1; ++i) {
//		files.push_back(argv[i + 1]);
//	}
//	std::filesystem::path building_directory = "./build";
//	if (!std::filesystem::exists(building_directory)) {
//		// 不存在
//		std::filesystem::create_directory(building_directory);
//	}
//	std::ifstream ifs{};
//	for (auto& file : files) {
//		if (!std::filesystem::exists(file)) {
//			std::println("The provided file '{}' does not exist.", file);
//			return 0;
//		}
//		ifs.open(file);
//		char ch{};
//		std::uint64_t line{};
//		while (ifs >> ch) {
//			texts[file].push_back(ch);
//		}
//		ifs.close();
//		ifs.clear();
//	}
//
//	
//}
class parser {
public:
};