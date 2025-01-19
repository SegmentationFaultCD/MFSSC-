export module MFSSCPP;
import MFSSCPP.analysis;
import std;


std::map<std::string, std::string> texts;
export auto main(int argc, char** argv, [[maybe_unused]] char** envp) -> int {
	std::vector<std::string> files;
	// 从命令行读取文件
	for (auto i = 0; i < argc - 1; ++i) {
		files.push_back(argv[i + 1]);
	}

	std::filesystem::path binary_directory = "./bin";
	if (!std::filesystem::exists(binary_directory)) {
		// 不存在
		std::filesystem::create_directory(binary_directory);
	}

	std::ifstream ifs{};
	for (auto& file : files) {
		if (!std::filesystem::exists(file)) {
			std::println("The provided file '{}' does not exist.", file);
			return 0;
		}
		ifs.open(file);

		while (!ifs.eof()) {
			if (auto ch = ifs.get(); ch != -1) {
				texts[file].push_back(char(ch));
				/*std::print("{:c}", ch);*/
			}
		}
		/*std::println();*/
		ifs.close();
		ifs.clear();
	}

	lexer lex{ texts };
	return 0;

}