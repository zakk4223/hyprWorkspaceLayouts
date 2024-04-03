#pragma once

#include <hyprland/src/includes.hpp>
#include <string>
#include <vector>
#include <list>
#include <deque>
#include <any>
#define private public
#include <hyprland/src/managers/LayoutManager.hpp>
#undef private
#include <hyprland/src/layout/IHyprLayout.hpp>
#include "globals.hpp"

struct SWorkspaceLayoutWindowData {
	CWindow *pWindow = nullptr;
	int workspaceID = -1;
    bool operator==(const SWorkspaceLayoutWindowData& rhs) const {
        return pWindow == rhs.pWindow;
    }
};


struct SWorkspaceLayoutData {
    int                workspaceID = -1;
    IHyprLayout *      layout = nullptr;
		bool							 isDefault = false;


    bool               operator==(const SWorkspaceLayoutData& rhs) const {
        return workspaceID == rhs.workspaceID;
    }
};



class CWorkspaceLayout : public IHyprLayout {
  public:
    virtual void onEnable();
    virtual void onDisable();
    virtual void onWindowCreated(CWindow*, eDirection direction = DIRECTION_DEFAULT);
    virtual void onWindowCreatedTiling(CWindow*, eDirection direction = DIRECTION_DEFAULT);
    virtual void onWindowCreatedFloating(CWindow*);
    virtual bool isWindowTiled(CWindow*);
    virtual void onWindowRemoved(CWindow*);
    virtual void onWindowRemovedTiling(CWindow*);
    virtual void onWindowRemovedFloating(CWindow*);
    virtual void recalculateMonitor(const int&);
    virtual void recalculateWindow(CWindow*);
    virtual void changeWindowFloatingMode(CWindow*);
    virtual void onBeginDragWindow();
    virtual void resizeActiveWindow(const Vector2D&, eRectCorner corner = CORNER_NONE, CWindow* pWindow = nullptr);
    virtual void moveActiveWindow(const Vector2D&, CWindow* pWindow = nullptr);
    virtual void onEndDragWindow();
    virtual void onMouseMove(const Vector2D&);
    virtual void fullscreenRequestForWindow(CWindow*, eFullscreenMode, bool);
    virtual std::any layoutMessage(SLayoutMessageHeader, std::string);
    virtual SWindowRenderLayoutHints requestRenderHints(CWindow*);
    virtual void switchWindows(CWindow*, CWindow*);
    virtual void moveWindowTo(CWindow*, const std::string& direction);
    virtual void alterSplitRatio(CWindow*, float, bool exact = false);
    virtual std::string getLayoutName();
    virtual CWindow* getNextWindowCandidate(CWindow*);
    virtual void onWindowFocusChange(CWindow*);
    virtual void replaceWindowDataWith(CWindow* from, CWindow* to);
    virtual bool isWindowReachable(CWindow*);
    virtual void bringWindowToTop(CWindow*);
    virtual void requestFocusForWindow(CWindow*);
		virtual Vector2D predictSizeForNewWindowTiled();

		void setDefaultLayout(std::string name);
		void clearLayoutMaps();
		void setupWorkspace(CWorkspace *pWorkspace);



  private:
		std::vector<SWorkspaceLayoutData> m_vWorkspacesData;
		std::list<SWorkspaceLayoutWindowData> m_vWorkspaceWindowData;
    SWorkspaceLayoutWindowData* getDataFromWindow(CWindow*, bool create=true);
		IHyprLayout *getLayoutForWorkspace(const int& ws);
		void setLayoutForWorkspace(IHyprLayout *layout, CWorkspace *pWorkspace, bool isDefault);
		void setLayoutForWorkspace(IHyprLayout *layout, const int& ws, bool isDefault = false);
		IHyprLayout *findUserLayoutForWorkspace(CWorkspace *pWorkspace);
		IHyprLayout *getLayoutByName(const std::string& name);
		IHyprLayout												*m_pDefaultLayout = nullptr;

    friend struct SWorkspaceLayoutData;
    friend struct SWorkspaceLayoutWindowData;
};
