#pragma once

#include <string>
#include <map>
#include <memory>

enum AssetType_ {
	AssetType_None = 0,
	AssetType_CMO,
	AssetType_CMC,
	AssetType_Count,
};

typedef int AssetType;

struct Asset_t {
	AssetType type;
    int index;
};

class AssetManager {
 private:
 std::map<std::string, std::shared_ptr<Asset_t>> m_assets;
 public:
	AssetManager();
	~AssetManager();

	void callback(AssetType t_type, int t_index, std::string t_name);
	void replaceAsset(std::string to_destination, std::string to_source);
};

inline std::unique_ptr<AssetManager> g_assetManager;