#include <hyprland/src/Compositor.hpp>
#include "globals.hpp"
#include "workspaceLayout.hpp"


#include <unistd.h>
#include <thread>

namespace {
	inline std::unique_ptr<CWorkspaceLayout> g_pWorkspaceLayout; 
	
	void WSConfigPreload() {
			g_pWorkspaceLayout->clearLayoutMaps();
	}
	
	
	void WSConfigReloaded() {
		static SConfigValue *DEFAULTLAYOUT = HyprlandAPI::getConfigValue(PHANDLE, "plugin:wslayout:default_layout");
		if (g_pWorkspaceLayout)
			g_pWorkspaceLayout->setDefaultLayout(DEFAULTLAYOUT->strValue);
	}
	
	void WSWorkspaceCreated(CWorkspace *pWorkspace) {
		g_pWorkspaceLayout->setupWorkspace(pWorkspace);
	}
	
	void WSLayoutsChanged() {
		g_pWorkspaceLayout->onEnable();
	}
	
	void onMonitorLayout(const std::string& k, const std::string& v) {
		CVarList vars(v);
		std::string monStr = removeBeginEndSpacesTabs(vars[0]);
		g_pWorkspaceLayout->setMonitorMapEntry(monStr,removeBeginEndSpacesTabs(vars[1])); 
	}
	
	void onWorkspaceLayout(const std::string& k, const std::string& v) {
		CVarList vars(v);
		std::string name = "";
		std::string wsStr = removeBeginEndSpacesTabs(vars[0]);
		int id = getWorkspaceIDFromString(wsStr, name);
		if (id != WORKSPACE_INVALID) {
			g_pWorkspaceLayout->setWorkspaceMapEntry(name,removeBeginEndSpacesTabs(vars[1])); 
		} else {
			Debug::log(LOG, "WSLAYOUT: Workspace layout rule name {} is invalid", vars[0]);
		}
	}
	
	
	inline CFunctionHook *g_pCreateWorkspaceHook = nullptr;
	typedef CWorkspace*(*origCreateWorkspace)(void *, const int&, const int&, const std::string&);
	
	CWorkspace *hkCreateWorkspace(void *thisptr, const int& id, const int& monid, const std::string& name) {
	
		CWorkspace *ret = (*(origCreateWorkspace)g_pCreateWorkspaceHook->m_pOriginal)(thisptr, id, monid, name);
		WSWorkspaceCreated(ret);
		return ret;
	}


	inline CFunctionHook *g_pAddLayoutHook = nullptr;
	typedef bool(*origAddLayout)(void *, const std::string&, IHyprLayout *layout); 
	
	bool hkAddLayout(void *thisptr, const std::string& name, IHyprLayout *layout) {
	
		bool ret = (*(origAddLayout)g_pAddLayoutHook->m_pOriginal)(thisptr, name, layout);
		WSLayoutsChanged();
		
		return ret;
	}

	inline CFunctionHook *g_pRemoveLayoutHook = nullptr;
	typedef bool(*origRemoveLayout)(void *, IHyprLayout *layout); 
	
	bool hkRemoveLayout(void *thisptr, IHyprLayout *layout) {
	
		bool ret = (*(origRemoveLayout)g_pRemoveLayoutHook->m_pOriginal)(thisptr,layout);
		WSLayoutsChanged();
		return ret;
	}
}


APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;

		HyprlandAPI::addConfigKeyword(PHANDLE, "workspace_layout", [&](const std::string& k, const std::string& v) {onWorkspaceLayout(k,v);});
		HyprlandAPI::addConfigKeyword(PHANDLE, "monitor_layout", [&](const std::string& k, const std::string& v) {onMonitorLayout(k,v);});

		HyprlandAPI::addConfigValue(PHANDLE, "plugin:wslayout:default_layout", SConfigValue{.strValue = "dwindle"});
		static const auto WSCREATEMETHODS = HyprlandAPI::findFunctionsByName(PHANDLE, "createNewWorkspace");
		g_pCreateWorkspaceHook = HyprlandAPI::createFunctionHook(PHANDLE, WSCREATEMETHODS[0].address, (void *)&hkCreateWorkspace);
	g_pCreateWorkspaceHook->hook();

		static const auto ADDLAYOUTMETHODS = HyprlandAPI::findFunctionsByName(PHANDLE, "addLayout");
		g_pAddLayoutHook = HyprlandAPI::createFunctionHook(PHANDLE, ADDLAYOUTMETHODS[0].address, (void *)&hkAddLayout);
	g_pAddLayoutHook->hook();

		static const auto REMOVELAYOUTMETHODS = HyprlandAPI::findFunctionsByName(PHANDLE, "removeLayout");
		g_pRemoveLayoutHook = HyprlandAPI::createFunctionHook(PHANDLE, REMOVELAYOUTMETHODS[0].address, (void *)&hkRemoveLayout);
	g_pRemoveLayoutHook->hook();

		
		HyprlandAPI::registerCallbackDynamic(PHANDLE, "preConfigReload", [&](void *self, SCallbackInfo&, std::any data) {WSConfigPreload();});

		HyprlandAPI::registerCallbackDynamic(PHANDLE, "configReloaded", [&](void *self, SCallbackInfo&, std::any data) {WSConfigReloaded();});
		g_pWorkspaceLayout = std::make_unique<CWorkspaceLayout>();

        // set default layout, because that will be used by the calls below, when adding itself as a layout or loading the config
        g_pWorkspaceLayout->setDefaultLayout("dwindle");

		HyprlandAPI::addLayout(PHANDLE, "workspacelayout", g_pWorkspaceLayout.get());
		HyprlandAPI::reloadConfig(); // here the actual default layout will be read

    return {"Workspace layouts", "Per-workspace layouts", "Zakk", "1.0"};
}

// Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT void PLUGIN_EXIT() {
    HyprlandAPI::invokeHyprctlCommand("seterror", "disable");
}
