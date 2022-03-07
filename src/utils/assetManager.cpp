#include "assetManager.hpp"

#include "../game.hpp"
#include "../hooks.hpp"

AssetManager::AssetManager() { m_assets = {}; }

AssetManager::~AssetManager() {}

void AssetManager::callback(AssetType t_type, int t_index, std::string t_name) {
	if (m_assets[t_name])
		throw std::range_error(spdlog::fmt_lib::format(
		    "Asset {} (t: {}, i: {}) already loaded at {}.", t_name, t_type,
		    t_index, m_assets[t_name]->index));
	m_assets[t_name] = std::make_shared<Asset_t>(t_type, t_index);
}

void AssetManager::replaceAsset(std::string destination, std::string source) {
	auto asset = m_assets[destination];
	if (!asset) throw std::out_of_range("Asset doesn't exist.");

	switch (asset->type) {
		case AssetType_CMO: {
			subhook::ScopedHookRemove remove(&g_hooks->loadCMOHook);
			g_game->loadCMOFunc(asset->index, -1, (char*)source.c_str());
			break;
		};
		case AssetType_CMC: {
			subhook::ScopedHookRemove remove(&g_hooks->loadCMCHook);
			g_game->loadCMCFunc(asset->index, (char*)source.c_str());
			break;
		};

		default:
			break;
	}
}