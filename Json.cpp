#include"Json.h"
#include<istream>
#include<ostream>
#include<fstream>
#include<sstream>
#include<filesystem>
namespace json {
	namespace fs = std::filesystem;
	Base_type* Json::get() noexcept {

		return _value.get();
	}
	const Base_type* Json::get() const noexcept {

		return _value.get();
	}
	Json::Json(Base_type* const& value) noexcept :
		_value(value) {
	}
	Json::Json(const Json& value) noexcept :
		_value(Copy::copy(value.get())) {
	}
	Json& Json::operator[](const std::string& str) {
		if (_value == nullptr) {
			_value.reset(new Class);
		}
		if (name() != "class") {

			throw std::invalid_argument("非类类型不能使用字符串作为索引");
		}

		return (*static_cast<jclass*>(_value->get_val()))[jstring("\"" + str + "\"")];
	}
	const Json& Json::operator[](const std::string& str) const {
		if (name() != "class") {

			throw std::invalid_argument("非类类型不能使用字符串作为索引");
		}
		String key = "\"" + str + "\"";
		if (!static_cast<Class*>(_value.get())->value.contains(key)) {

			throw std::invalid_argument("索引不存在");
		}

		return static_cast<jclass*>(_value->get_val())->at(key);
	}
	Json& Json::operator[](const std::size_t& index) {
		if (_value == nullptr) {
			_value.reset(new List);
		}
		if (name() != "list") {

			throw std::invalid_argument("非列表类型不能使用数字作为索引");
		}
		if (static_cast<List*>(_value.get())->value.size() <= index) {
			static_cast<List*>(_value.get())->value.resize(index + 1);
		}

		return static_cast<jlist*>(_value->get_val())->at(index);
	}
	const Json& Json::operator[](const std::size_t& index) const {
		if (name() != "list") {

			throw std::invalid_argument("非列表类型不能使用数字作为索引");
		}
		if (static_cast<List*>(_value.get())->value.size() <= index) {

			throw std::invalid_argument("列表长度不足");
		}

		return static_cast<jlist*>(_value->get_val())->at(index);
	}
	Json& Json::operator=(const std::string& value) noexcept {
		_value.reset(new String("\"" + value + "\""));

		return *this;
	}
	Json& Json::operator=(const char* const& value) noexcept {
		this->operator=(std::string(value));

		return *this;
	}
	Json& Json::operator=(const double& value) noexcept {
		_value.reset(new Number);
		static_cast<Number*>(_value.get())->value = value;

		return *this;
	}
	Json& Json::operator=(const long long& value) noexcept {
		this->operator=(static_cast<double>(value));

		return *this;
	}
	Json& Json::operator=(const unsigned long long& value) noexcept {
		this->operator=(static_cast<double>(value));

		return *this;
	}
	Json& Json::operator=(const int& value) noexcept {
		this->operator=(static_cast<double>(value));

		return *this;
	}
	Json& Json::operator=(const unsigned int& value) noexcept {
		this->operator=(static_cast<double>(value));

		return *this;
	}
	Json& Json::operator=(const short& value) noexcept {
		this->operator=(static_cast<double>(value));

		return *this;
	}
	Json& Json::operator=(const unsigned short& value) noexcept {
		this->operator=(static_cast<double>(value));

		return *this;
	}
	Json& Json::operator=(const bool& value) noexcept {
		_value.reset(new Bool(value ? "true" : "false"));

		return *this;
	}
	Json& Json::operator=(const Json& value) noexcept {
		_value.reset(Copy::copy(value.get()));

		return *this;
	}
	bool Json::operator==(const Json& other) const noexcept {

		return Compare::compare(_value.get(), other._value.get());
	}
	void Json::add_key(const std::string& key, Base_type* const& ptr) {
		if (_value == nullptr) {
			_value.reset(new Class);
		}
		if (name() != "class") {

			throw std::invalid_argument("非类类型不能添加键值对");
		}
		static_cast<jclass*>(_value->get_val())->insert(std::make_pair("\"" + key + "\"", ptr));
	}
	void Json::push_back(Base_type* const& ptr) {
		if (_value == nullptr) {
			_value.reset(new List);
		}
		if (name() != "list") {

			throw std::invalid_argument("非列表类型不能添加值");
		}
		static_cast<jlist*>(_value->get_val())->emplace_back(ptr);
	}
	bool Json::contains(const std::string& str) const {
		if (name() != "class") {

			throw std::invalid_argument("非类类型不能使用字符串作为下标");
		}

		return static_cast<jclass*>(_value->get_val())->contains("\"" + str + "\"");
	}
	std::size_t Json::size() const {
		if (name() == "class") {

			return static_cast<jclass*>(_value->get_val())->size();
		}
		else if (name() == "list") {

			return static_cast<jlist*>(_value->get_val())->size();
		}

		throw std::invalid_argument("只有列表和类有大小");
	}
	std::string Json::name() const noexcept {
		if (_value == nullptr) {

			return "null_t";
		}

		return _value->name();
	}
	jnumber Json::as_num() const {
		if (name() != "number") {

			throw std::invalid_argument("只有数字类型可以使用as_num");
		}

		return *static_cast<jnumber*>(_value->get_val());
	}
	jbool Json::as_bool() const {
		if (name() != "bool") {

			throw std::invalid_argument("只有布尔类型可以使用as_bool");
		}

		return *static_cast<jbool*>(_value->get_val());
	}
	jstring Json::as_string() const {
		if (name() != "string") {

			throw std::invalid_argument("只有字符串类型可以使用as_string");
		}

		return *static_cast<jstring*>(_value->get_val());
	}
	static std::size_t tabs = 0;
	static void end_line(std::ostream& os) {
		os << "\n";
		for (std::size_t i = 0;i < tabs;i++) {
			os.put('	');
		}
	}
	static Base_type* new_type(const std::string& value) {
		if (value == "null") {
			return nullptr;
		}
		else if (value.size() > 2) {
			if (value == "true" || value == "false") {
				return new Bool(value);
			}
			else if (value[0] == '\"' && value.back() == '\"') {
				return new String(value);
			}
			else if (value[0] == '[' && value.back() == ']') {
				return  new List(value);
			}
			else if (value[0] == '{' && value.back() == '}') {
				return  new Class(value);
			}
			else {
				return  new Number(value);
			}
		}
		else {
			return new Number(value);
		}
	}
	void Print::print(std::ostream& os, const Json& value) noexcept {
		if (value.name() == "null_t") {
			os << "null";
		}
		else {
			_func[value.name()](os, value.get());
		}
	}
	Base_type* Copy::copy(const Base_type* const& value) noexcept {
		if (value == nullptr) {

			return nullptr;
		}

		return _func[value->name()](value);
	}
	bool Compare::compare(const Base_type* const& a, const Base_type* const& b) noexcept {
		if (a == nullptr && b == nullptr) {

			return true;
		}
		if (a == nullptr || b == nullptr) {

			return false;
		}
		if (a->name() != b->name()) {

			return false;
		}

		return _func[a->name()](a, b);
	}
	Number::Number(const std::string& str) {
		if (str.empty()) {

			throw std::invalid_argument("空字符串不能用于构造数字");
		}
		std::istringstream iss(str);
		iss >> value;
		if (iss.fail() || !iss.eof()) {

			throw std::invalid_argument("用于构造数字的字符串不能包含非数字字符");
		}
	}
	std::ostream& operator<<(std::ostream& os, const Number& value) noexcept {
		os << value.value;

		return os;
	}
	Bool::Bool(const std::string& str) {
		if (str!="true"&&str!="false") {

			throw std::invalid_argument("于构造布尔型的字符串只能是“true”或“false”");
		}
		value = str == "true";
	}
	std::ostream& operator<<(std::ostream& os, const Bool& value) noexcept {
		os << (value.value ? "true" : "false");

		return os;
	}
	String::String(const std::string& str) {
		if (str.size() < 2 || str[0] != '\"' || str.back() != '\"') {

			throw std::invalid_argument("用于构造字符串的字符串格式不合法");
		}
		value = str.substr(1, str.size() - 2);
	}
	std::ostream& operator<<(std::ostream& os, const String& value) noexcept {
		os.put('\"');
		for (const char& i : value.value) {
			if (i == '\"' || i == '\\') {
				os.put('\\');
			}
			os.put(i);
		}
		os.put('\"');

		return os;
	}
	List::List(const std::string& str) {
		if (str.size() < 2 || str[0] != '[' || str.back() != ']') {

			throw std::invalid_argument("用于构造列表的字符串格式不合法");
		}
		std::string copy = str;
		bool change = false;
		bool instr = false;
		std::size_t cnt = 0;
		std::erase_if(copy, [&cnt, &change, &instr](const char& c)->bool {
			if (change) {
				change = false;

				return false;
			}
			if (cnt == 1 && c == '\\') {
				change = true;

				return true;
			}
			if (c == '\"') {
				instr = !instr;
			}
			if (instr) {

				return false;
			}
			if (c == '{' || c == '[') {
				cnt++;

				return false;
			}
			if (c == '}' || c == ']') {
				if (cnt == 0) {

					throw std::invalid_argument("用于构造列表的字符串中的[],{}或\"\"不成对");
				}
				cnt--;

				return false;
			}
			if (cnt == 1 && c == ' ' || c == '	' || c == '\n' || c == '\r') {

				return true;
			}

			return false;
			});
		if (instr || cnt != 0) {

			throw std::invalid_argument("用于构造列表的字符串中的[],{}或\"\"不成对");
		}
		copy.back() = ',';
		std::size_t start = 1;
		while (start < copy.size()) {
			change = instr = false;
			cnt = 0;
			std::size_t found = start - 1;
			while (true) {
				found++;
				if (change) {
					change = false;
					continue;
				}
				if (copy[found] == '\"') {
					instr = !instr;
				}
				if (instr) {
					continue;
				}
				if (copy[found] == '{' || copy[found] == '[') {
					cnt++;
					continue;
				}
				if (copy[found] == '}' || copy[found] == ']') {
					cnt--;
					continue;
				}
				if (cnt == 0 && copy[found] == ',') {
					break;
				}
			}
			std::string new_value = copy.substr(start, found - start);
			Base_type* ptr = new_type(new_value);
			value.emplace_back(ptr);
			start = found + 1;
		}
	}
	std::ostream& operator<<(std::ostream& os, const List& value) noexcept {
		os << '[';
		for (auto i = value.value.begin();i != value.value.end();i++) {
			Print::print(os, *i);
			if (i + 1 != value.value.end()) {
				os << ',';
				if (i->name() == "class") {
					end_line(os);
				}
			}
		}
		os << ']';

		return os;
	}
	Class::Class(const std::string& str) {
		if (str.size() < 2 || str[0] != '{' || str.back() != '}') {

			throw std::invalid_argument("用于构造类的字符串格式不合法");
		}
		std::string copy = str;
		bool change = false;
		bool instr = false;
		std::size_t cnt = 0;
		std::erase_if(copy, [&cnt, &change, &instr](const char& c)->bool {
			if (change) {
				change = false;

				return false;
			}
			if (cnt == 1 && c == '\\') {
				change = true;

				return true;
			}
			if (c == '\"') {
				instr = !instr;
			}
			if (instr) {

				return false;
			}
			if (c == '{' || c == '[') {
				cnt++;

				return false;
			}
			if (c == '}' || c == ']') {
				if (cnt == 0) {

					throw std::invalid_argument("用于构造类的字符串中的[],{}或\"\"不成对");
				}
				cnt--;

				return false;
			}
			if (cnt == 1 && c == ' ' || c == '	' || c == '\n' || c == '\r') {

				return true;
			}

			return false;
			});
		if (instr || cnt != 0) {
			throw std::invalid_argument("用于构造类的字符串中的[],{}或\"\"不成对");
		}
		copy.back() = ',';
		std::size_t start = 1;
		while (start < copy.size()) {
			change = instr = false;
			cnt = 0;
			std::size_t found = start - 1, num = 0;
			while (true) {
				found++;
				if (change) {
					change = false;
					continue;
				}
				if (copy[found] == '\"') {
					instr = !instr;
				}
				if (instr) {
					continue;
				}
				if (copy[found] == '{' || copy[found] == '[') {
					cnt++;
					continue;
				}
				if (copy[found] == '}' || copy[found] == ']') {
					cnt--;
					continue;
				}
				if (cnt == 0 && copy[found] == ',') {
					break;
				}
				if (cnt == 0 && copy[found] == ':') {
					num++;
				}
			}
			std::string new_pair = copy.substr(start, found - start);
			if (num != 1) {

				throw std::invalid_argument("用于构造类的字符串中的键值对格式不合法");
			}
			std::string key = new_pair.substr(0, new_pair.find(':'));
			std::string new_value = new_pair.substr(key.size() + 1);
			String s(key);
			Base_type* ptr = new_type(new_value);
			value.insert(std::make_pair(s, ptr));
			start = found + 1;
		}
	}
	std::ostream& operator<<(std::ostream& os, const Class& value) noexcept {
		os << '{';
		tabs++;
		end_line(os);
		if (!value.value.empty()) {
			os << value.value.begin()->first << ':';
			Print::print(os, value.value.begin()->second);
		}
		auto i = value.value.begin();
		for (i++;i != value.value.end();i++) {
			os << ',';
			end_line(os);
			os << i->first << ':';
			Print::print(os, i->second);
		}
		tabs--;
		end_line(os);
		os << '}';

		return os;
	}
	Json read(std::istream& is) {
		std::string json;
		std::string line;
		while (std::getline(is, line)) {
			json += line;
		}
		Base_type* ptr;
		if (json.size() > 2) {
			if (json[0] == '\"' && json.back() == '\"') {
				ptr = new String(json);
			}
			else if (json[0] == '[' && json.back() == ']') {
				ptr = new List(json);
			}
			else if (json[0] == '{' && json.back() == '}') {
				ptr = new Class(json);
			}
			else {
				ptr = new Number(json);
			}
		}
		else {
			ptr = new Number(json);
		}

		return ptr;
	}
	Json read(const fs::path& path) {
		std::ifstream file(path);

		return read(file);
	}
	void write(std::ostream& os, const Json& value) noexcept {
		Print::print(os, value);
	}
	void write(const fs::path& path, const Json& value) noexcept {
		std::ofstream file(path);
		write(file, value);
	}
}