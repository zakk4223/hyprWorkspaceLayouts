#pragma once

#include <hyprland/src/includes.hpp>
#include <string>
#include <vector>
#include <list>
#include <deque>
#include <any>
#include <sstream>
#define private public
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/DesktopTypes.hpp>
#include <hyprland/src/managers/LayoutManager.hpp>
#include <hyprland/src/managers/input/InputManager.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/render/Renderer.hpp>
#undef private
#include "globals.hpp"

struct SWorkspaceLayoutWindowData {
    PHLWINDOWREF pWindow;
    int          workspaceID = -1;
    bool         operator==(const SWorkspaceLayoutWindowData& rhs) const {
        return pWindow.lock() == rhs.pWindow.lock();
    }
};

struct SWorkspaceLayoutData {
    int          workspaceID    = -1;
    IHyprLayout* layout         = nullptr;
    IHyprLayout* previousLayout = nullptr;
    bool         isDefault      = false;

    bool         operator==(const SWorkspaceLayoutData& rhs) const {
        return workspaceID == rhs.workspaceID;
    }
};

class CWorkspaceLayout : public IHyprLayout {
  public:
    virtual void                     onEnable();
    virtual void                     onDisable();
    virtual void                     onWindowCreated(PHLWINDOW, eDirection direction = DIRECTION_DEFAULT);
    virtual void                     onWindowCreatedTiling(PHLWINDOW, eDirection direction = DIRECTION_DEFAULT);
    virtual void                     onWindowCreatedFloating(PHLWINDOW);
    virtual bool                     isWindowTiled(PHLWINDOW);
    virtual void                     onWindowRemoved(PHLWINDOW);
    virtual void                     onWindowRemovedTiling(PHLWINDOW);
    virtual void                     onWindowRemovedFloating(PHLWINDOW);
    virtual void                     recalculateMonitor(const MONITORID&);
    virtual void                     recalculateWindow(PHLWINDOW);
    virtual void                     changeWindowFloatingMode(PHLWINDOW);
    virtual void                     onBeginDragWindow();
    virtual void                     resizeActiveWindow(const Vector2D&, eRectCorner corner = CORNER_NONE, PHLWINDOW pWindow = nullptr);
    virtual void                     moveActiveWindow(const Vector2D&, PHLWINDOW pWindow = nullptr);
    virtual void                     onEndDragWindow();
    virtual void                     onMouseMove(const Vector2D&);
    virtual void                     fullscreenRequestForWindow(PHLWINDOW pWindow, const eFullscreenMode CURRENT_EFFECTIVE_MODE, const eFullscreenMode EFFECTIVE_MODE);
    virtual std::any                 layoutMessage(SLayoutMessageHeader, std::string);
    virtual SWindowRenderLayoutHints requestRenderHints(PHLWINDOW);
    virtual void                     switchWindows(PHLWINDOW, PHLWINDOW);
    virtual void                     moveWindowTo(PHLWINDOW, const std::string& direction, bool silent);
    virtual void                     alterSplitRatio(PHLWINDOW, float, bool exact = false);
    virtual std::string              getLayoutName();
    virtual PHLWINDOW                getNextWindowCandidate(PHLWINDOW);
    virtual void                     onWindowFocusChange(PHLWINDOW);
    virtual void                     replaceWindowDataWith(PHLWINDOW from, PHLWINDOW to);
    virtual bool                     isWindowReachable(PHLWINDOW);
    virtual void                     bringWindowToTop(PHLWINDOW);
    virtual void                     requestFocusForWindow(PHLWINDOW);
    virtual Vector2D                 predictSizeForNewWindowTiled();

    void                             setDefaultLayout(std::string name);
    void                             clearLayoutMaps();
    void                             setupWorkspace(PHLWORKSPACEREF pWorkspaceRef);
    void                             setupLayoutList();

  private:
    std::vector<SWorkspaceLayoutData>     m_vWorkspacesData;
    std::list<SWorkspaceLayoutWindowData> m_vWorkspaceWindowData;
    SWorkspaceLayoutWindowData*           getDataFromWindow(PHLWINDOW, bool create = true);
    IHyprLayout*                          getLayoutForWorkspace(const int& ws);
    IHyprLayout*                          getPreviousLayoutForWorkspace(const int& ws);
    void                                  setLayoutForWorkspace(IHyprLayout* layout, PHLWORKSPACE pWorkspace, bool isDefault);
    void                                  setLayoutForWorkspace(IHyprLayout* layout, const int& ws, bool isDefault = false);
    IHyprLayout*                          findUserLayoutForWorkspace(PHLWORKSPACE pWorkspace);
    IHyprLayout*                          getLayoutByName(const std::string& name);
    IHyprLayout*                          m_pDefaultLayout = nullptr;
    std::vector<IHyprLayout*>             m_vLayoutList;

    friend struct SWorkspaceLayoutData;
    friend struct SWorkspaceLayoutWindowData;
};
