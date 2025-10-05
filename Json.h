#ifndef _JSON_H_
#define _JSON_H_
#include<iosfwd>
#include<string>
#include<unordered_map>
#include<vector>
#include<functional>
#include<memory>
#include<stdexcept>
namespace std {
	namespace filesystem {
		class path;
	}
}
namespace json {
	struct Jstring_hasher;
	class Base_type;
	class Json {//json�ļ���
	protected:
		std::unique_ptr<Base_type> _value;
		//��ȡָ��
		Base_type* get() noexcept;
		const Base_type* get() const noexcept;
	public:
		Json(Base_type* const& value = nullptr) noexcept;
		Json(const Json& value) noexcept;
		//����������
		Json& operator[](const std::string& str);
		const Json& operator[](const std::string& str) const;
		Json& operator[](const std::size_t& index);
		const Json& operator[](const std::size_t& index) const;
		//��ֵ����
		Json& operator=(const std::string& value) noexcept;
		Json& operator=(const char* const& value) noexcept;
		Json& operator=(const double& value) noexcept;
		Json& operator=(const long long& value) noexcept;
		Json& operator=(const unsigned long long& value) noexcept;
		Json& operator=(const int& value) noexcept;
		Json& operator=(const unsigned int& value) noexcept;
		Json& operator=(const short& value) noexcept;
		Json& operator=(const unsigned short& value) noexcept;
		Json& operator=(const bool& value) noexcept;
		Json& operator=(const Json& value) noexcept;
		//���ֵ
		void add_key(const std::string& key, Base_type* const& ptr);
		void push_back(Base_type* const& ptr);
		//�����Ƿ����
		bool contains(const std::string& key) const;
		//�б����Ĵ�С
		std::size_t size() const;
		//��������
		std::string name() const noexcept;
		//ת��Ϊ����
		long double as_num() const;
		//ת��Ϊ����
		bool as_bool() const;
		//ת��Ϊ�ַ���
		std::string as_string() const;
		//ת��Ϊ�б�
		inline auto as_list() const;
		//ת��Ϊ��
		inline auto as_class() const;
		//�Ƚ�����
		bool operator==(const Json& other) const noexcept;

		friend class Print;
	};
	class Base_type {//���ͻ���
	public:
		//��������
		virtual std::string name() const noexcept = 0;
		//��ȡ��Ϣ
		virtual void* get_val() noexcept = 0;
		virtual const void* get_val() const noexcept = 0;
		virtual ~Base_type() noexcept = default;
	};
	class Print {//�����
	private:
		Print() = delete;
	protected:
		inline static std::unordered_map<std::string, std::function<void(std::ostream&, const Base_type* const&)> > _func;
	public:
		//�������
		template<typename T>
		inline static void add(const std::string& name) noexcept;
		//���
		static void print(std::ostream& os, const Json& value) noexcept;
	};
	class Copy {//������
	private:
		Copy() = delete;
	protected:
		inline static std::unordered_map<std::string, std::function<Base_type*(const Base_type* const&)> > _func;
	public:
		//�������
		template<typename T>
		inline static void add(const std::string& name) noexcept;
		//����
		static Base_type* copy(const Base_type* const& value) noexcept;
	};
	class Compare {//�Ƚ���
	private:
		Compare() = delete;
	protected:
		inline static std::unordered_map<std::string, std::function<bool(const Base_type* const&, const Base_type* const&)> > _func;
	public:
		//�������
		template<typename T>
		inline static void add(const std::string& name) noexcept;
		//����
		static bool compare(const Base_type* const& a, const Base_type* const& b) noexcept;
	};
#define ARG(...) __VA_ARGS__
#define VAL(class_name, type, type_name) using j##type_name = type;\
	class class_name :public Base_type{\
	private:\
		struct Adder{\
			Adder() noexcept {\
				Print::add<class_name>(#type_name);\
				Copy::add<class_name>(#type_name);\
				Compare::add<class_name>(#type_name);\
			}\
		};\
		inline static Adder _adder;\
	public:\
		j##type_name value;\
		class_name() = default;\
		class_name(const class_name& other) = default;\
		class_name(const std::string& str);\
		std::string name() const noexcept override {\
			\
			return #type_name;\
		}\
		void* get_val() noexcept override {\
			\
			return static_cast<void*>(&value);\
		}\
		const void* get_val() const noexcept override {\
			\
			return static_cast<const void*>(&value);\
		}\
		bool operator==(const class_name& other) const noexcept{\
			\
			return value==other.value;\
		}\
		class_name& operator=(const class_name& other) noexcept{\
			value = other.value;\
			\
			return *this;\
		}\
		\
		friend std::ostream& operator<<(std::ostream& os, const class_name& value) noexcept;\
	};\
	std::ostream& operator<<(std::ostream& os, const class_name& value) noexcept
	VAL(Number, long double, number);
	VAL(Bool, bool, bool);
	VAL(String, std::string, string);
	struct Jstring_hasher {
		std::size_t operator()(const String& value) const noexcept {
			std::hash<std::string> hasher;

			return hasher(value.value);
		}
	};
	VAL(List, std::vector<Json>, list);
	VAL(Class, ARG(std::unordered_map<String, Json, Jstring_hasher>), class);
#undef ARG
#undef VAL
	//��ȡjson
	Json read(std::istream& is);
	Json read(const std::filesystem::path& path);
	//д��json
	void write(std::ostream& os, const Json& value) noexcept;
	void write(const std::filesystem::path& path, const Json& value) noexcept;

	auto Json::as_list() const {
		if (name() != "lsit") {

			throw std::invalid_argument("ֻ���б����Ϳ���ʹ��as_list");
		}

		return *static_cast<jlist*>(_value->get_val());
	}
	auto Json::as_class() const {
		if (name() != "class") {

			throw std::invalid_argument("ֻ�������Ϳ���ʹ��as_class");
		}

		return *static_cast<jclass*>(_value->get_val());
	}
	template<typename T>
	void Print::add(const std::string& name) noexcept {
		_func[name] = [](std::ostream& os, const Base_type* const& value)->void {
			os << *static_cast<const T* const>(value);
			};
	}
	template<typename T>
	void Copy::add(const std::string& name) noexcept {
		_func[name] = [](const Base_type* const& value)->Base_type* {

			return new T(*static_cast<const T* const>(value));
			};
	}
	template<typename T>
	void Compare::add(const std::string& name) noexcept {
		_func[name] = [](const Base_type* const& a, const Base_type* const& b)->bool {

			return *static_cast<const T* const>(a) == *static_cast<const T* const>(b);
			};
	}
}
#endif