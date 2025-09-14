#include "globals.hpp"
#include <hyprland/src/Compositor.hpp>
#include "workspaceLayout.hpp"

#include <hyprland/src/desktop/DesktopTypes.hpp>
#include <unistd.h>
#include <thread>

namespace {
    inline std::unique_ptr<CWorkspaceLayout> g_pWorkspaceLayout;
    inline std::vector<IHyprLayout*>         g_vLayoutList;

    void                                     WSConfigPreload() {
        g_pWorkspaceLayout->clearLayoutMaps();
    }

    void WSConfigReloaded() {

        static auto* const DEFAULTLAYOUT = (Hyprlang::STRING const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:wslayout:default_layout")->getDataStaticPtr();
        if (g_pWorkspaceLayout) {
            g_pWorkspaceLayout->setDefaultLayout(*DEFAULTLAYOUT);
            g_pWorkspaceLayout->setupLayoutList();
        }
    }

    void WSWorkspaceCreated(PHLWORKSPACE pWorkspace) {
        g_pWorkspaceLayout->setupWorkspace(PHLWORKSPACEREF{pWorkspace});
    }

    void WSLayoutsChanged() {
        g_pWorkspaceLayout->setupLayoutList();
        g_pWorkspaceLayout->onEnable();
    }

    inline CFunctionHook* g_pCreateWorkspaceHook = nullptr;
    typedef PHLWORKSPACE (*origCreateWorkspace)(void*, const int&, const int&, const std::string&);

    PHLWORKSPACE hkCreateWorkspace(void* thisptr, const int& id, const int& monid, const std::string& name) {

        PHLWORKSPACE ret = (*(origCreateWorkspace)g_pCreateWorkspaceHook->m_original)(thisptr, id, monid, name);
        WSWorkspaceCreated(ret);
        return ret;
    }

    inline CFunctionHook* g_pAddLayoutHook = nullptr;
    typedef bool (*origAddLayout)(void*, const std::string&, IHyprLayout* layout);

    bool hkAddLayout(void* thisptr, const std::string& name, IHyprLayout* layout) {

        bool ret = (*(origAddLayout)g_pAddLayoutHook->m_original)(thisptr, name, layout);
        WSLayoutsChanged();

        return ret;
    }

    inline CFunctionHook* g_pRemoveLayoutHook = nullptr;
    typedef bool (*origRemoveLayout)(void*, IHyprLayout* layout);

    bool hkRemoveLayout(void* thisptr, IHyprLayout* layout) {

        //If the layout is us, call onDisable. It should probably be done for every type of layout, but be safe
        if (layout == (IHyprLayout*)g_pWorkspaceLayout.get()) {
            layout->onDisable();
        }
        bool ret = (*(origRemoveLayout)g_pRemoveLayoutHook->m_original)(thisptr, layout);
        WSLayoutsChanged();
        return ret;
    }
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;

    HyprlandAPI::addConfigValue(PHANDLE, "plugin:wslayout:default_layout", Hyprlang::STRING{"dwindle"});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:wslayout:layouts", Hyprlang::STRING(""));

    static const auto WSCREATEMETHODS = HyprlandAPI::findFunctionsByName(PHANDLE, "createNewWorkspace");
    g_pCreateWorkspaceHook            = HyprlandAPI::createFunctionHook(PHANDLE, WSCREATEMETHODS[0].address, (void*)&hkCreateWorkspace);
    g_pCreateWorkspaceHook->hook();

    static auto PCRCB = HyprlandAPI::registerCallbackDynamic(PHANDLE, "preConfigReload", [&](void* self, SCallbackInfo&, std::any data) { WSConfigPreload(); });

    static auto CRCB   = HyprlandAPI::registerCallbackDynamic(PHANDLE, "configReloaded", [&](void* self, SCallbackInfo&, std::any data) { WSConfigReloaded(); });
    g_pWorkspaceLayout = std::make_unique<CWorkspaceLayout>();
    HyprlandAPI::addLayout(PHANDLE, "workspacelayout", g_pWorkspaceLayout.get());
    //Always set a default layout. It is possible other layouts are added before the reload config ticks.
    g_pWorkspaceLayout->setDefaultLayout("dwindle");
    HyprlandAPI::reloadConfig();

    static const auto ADDLAYOUTMETHODS = HyprlandAPI::findFunctionsByName(PHANDLE, "addLayout");
    g_pAddLayoutHook                   = HyprlandAPI::createFunctionHook(PHANDLE, ADDLAYOUTMETHODS[0].address, (void*)&hkAddLayout);
    g_pAddLayoutHook->hook();

    static const auto REMOVELAYOUTMETHODS = HyprlandAPI::findFunctionsByName(PHANDLE, "removeLayout");
    g_pRemoveLayoutHook                   = HyprlandAPI::createFunctionHook(PHANDLE, REMOVELAYOUTMETHODS[0].address, (void*)&hkRemoveLayout);
    g_pRemoveLayoutHook->hook();
    return {"hyprWorkspaceLayouts", "Per-workspace layouts", "Zakk", "1.0"};
}

// Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT void PLUGIN_EXIT() {
    HyprlandAPI::invokeHyprctlCommand("seterror", "disable");
}
