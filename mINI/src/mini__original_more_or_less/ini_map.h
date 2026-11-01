#ifndef mini_ini_map_h
#define mini_ini_map_h

namespace mINI
{
	namespace ci {
		struct comp
		{
			bool operator() (const std::string& lhs, const std::string& rhs) const {
				// On non Windows OS, use the function "strcasecmp" in #include <strings.h>
				return _stricmp(lhs.c_str(), rhs.c_str()) == 0;
			}
		};
		struct hash
		{
			std::size_t operator() (std::string str) const {
				std::transform(str.begin(), str.end(), str.begin(), ::tolower);
				return std::hash<std::string>{}(str);
			}
		};
	};

	template<typename T,	typename _Hasher, class _Keyeq>
	class INIMap
	{
	private:
		using T_DataIndexMap = std::unordered_map<std::string, std::size_t, _Hasher, _Keyeq>;
		using T_DataItem = std::pair<std::string, T>;
		using T_DataContainer = std::vector<T_DataItem>;
		using T_MultiArgs = typename std::vector<std::pair<std::string, T>>;

		T_DataIndexMap dataIndexMap;
		T_DataContainer data;

		inline std::size_t setEmpty(std::string& key)
		{
			std::size_t index = data.size();
			dataIndexMap[key] = index;
			data.emplace_back(key, T());
			return index;
		}

	public:
		using const_iterator = typename T_DataContainer::const_iterator;

		INIMap() { }

		INIMap(INIMap const& other)
		{
			std::size_t data_size = other.data.size();
			for (std::size_t i = 0; i < data_size; ++i)
			{
				auto const& key = other.data[i].first;
				auto const& obj = other.data[i].second;
				data.emplace_back(key, obj);
			}
			dataIndexMap = T_DataIndexMap(other.dataIndexMap);
		}

		T& operator[](std::string key)
		{
			INIStringUtil::trim(key);
//#ifndef MINI_CASE_SENSITIVE
//			INIStringUtil::toLower(key);
//#endif
			auto it = dataIndexMap.find(key);
			bool hasIt = (it != dataIndexMap.end());
			std::size_t index = (hasIt) ? it->second : setEmpty(key);
			return data[index].second;
		}
		T get(std::string key) const
		{
			INIStringUtil::trim(key);
//#ifndef MINI_CASE_SENSITIVE
//			INIStringUtil::toLower(key);
//#endif
			auto it = dataIndexMap.find(key);
			if (it == dataIndexMap.end())
			{
				return T();
			}
			return T(data[it->second].second);
		}
		bool has(std::string key) const
		{
			INIStringUtil::trim(key);
//#ifndef MINI_CASE_SENSITIVE
//			INIStringUtil::toLower(key);
//#endif
			return (dataIndexMap.count(key) == 1);
		}
		void set(std::string key, T obj)
		{
			INIStringUtil::trim(key);
//#ifndef MINI_CASE_SENSITIVE
//			INIStringUtil::toLower(key);
//#endif
			auto it = dataIndexMap.find(key);
			if (it != dataIndexMap.end())
			{
				data[it->second].second = obj;
			}
			else
			{
				dataIndexMap[key] = data.size();
				data.emplace_back(key, obj);
			}
		}
		void set(T_MultiArgs const& multiArgs)
		{
			for (auto const& it : multiArgs)
			{
				auto const& key = it.first;
				auto const& obj = it.second;
				set(key, obj);
			}
		}
		bool remove(std::string key)
		{
			INIStringUtil::trim(key);
//#ifndef MINI_CASE_SENSITIVE
//			INIStringUtil::toLower(key);
//#endif
			auto it = dataIndexMap.find(key);
			if (it != dataIndexMap.end())
			{
				std::size_t index = it->second;
				data.erase(data.begin() + index);
				dataIndexMap.erase(it);
				for (auto& it2 : dataIndexMap)
				{
					auto& vi = it2.second;
					if (vi > index)
					{
						vi--;
					}
				}
				return true;
			}
			return false;
		}
		void clear()
		{
			data.clear();
			dataIndexMap.clear();
		}
		std::size_t size() const
		{
			return data.size();
		}
		const_iterator begin() const { return data.begin(); }
		const_iterator end() const { return data.end(); }
	};

	using INIStructureCS = INIMap<INIMap<std::string, std::hash<std::string>, std::equal_to<std::string>>, std::hash<std::string>, std::equal_to<std::string>>;
	using INIStructureCI = INIMap<INIMap<std::string, ci::hash, ci::comp>, ci::hash, ci::comp>;
	// using INIStructure	= INIStructureCI;
}

#endif // mini_ini_map_h
