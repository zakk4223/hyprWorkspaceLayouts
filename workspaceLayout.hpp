#pragma once

#include "globals.hpp"
#include <hyprland/src/layout/IHyprLayout.hpp>
#include <hyprland/src//layout/DwindleLayout.hpp>
#include <hyprland/src/layout/MasterLayout.hpp>
#include <string>
#include <vector>
#include <list>
#include <deque>
#include <any>

//orientation determines which side of the screen the master area resides
struct SWorkspaceLayoutData {
    int                workspaceID = -1;
    IHyprLayout *      layout = nullptr;
		bool							 isDefault = false;


    bool               operator==(const SWorkspaceLayoutData& rhs) const {
        return workspaceID == rhs.workspaceID;
    }
};


class CWorkspaceLayoutHackLayoutManager {
  public:
    CWorkspaceLayoutHackLayoutManager();

    IHyprLayout* getCurrentLayout();

    void         switchToLayout(std::string);

    bool         addLayout(const std::string& name, IHyprLayout* layout);
    bool         removeLayout(IHyprLayout* layout);

    enum HYPRLAYOUTS
    {
        LAYOUT_DWINDLE = 0,
        LAYOUT_MASTER
    };

    int                                               m_iCurrentLayoutID = LAYOUT_DWINDLE;

    CHyprDwindleLayout                                m_cDwindleLayout;
    CHyprMasterLayout                                 m_cMasterLayout;

    std::vector<std::pair<std::string, IHyprLayout*>> m_vLayouts;
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

		void setDefaultLayout(std::string name);
		void setWorkspaceMapEntry(std::string, std::string);
		void setMonitorMapEntry(std::string, std::string);
		void clearLayoutMaps();
		void setupWorkspace(CWorkspace *pWorkspace);



  private:
		std::vector<std::pair<std::string, std::string>> m_vWorkspaceLayoutMap;
		std::vector<std::pair<std::string, std::string>> m_vMonitorLayoutMap;
		std::vector<SWorkspaceLayoutData> m_vWorkspacesData;
		IHyprLayout *getLayoutForWorkspace(const int& ws);
		void setLayoutForWorkspace(IHyprLayout *layout, CWorkspace *pWorkspace, bool isDefault);
		void setLayoutForWorkspace(IHyprLayout *layout, const int& ws, bool isDefault = false);
		IHyprLayout *findUserLayoutForWorkspace(CWorkspace *pWorkspace);
		IHyprLayout *getLayoutByName(const std::string& name);
		IHyprLayout												*m_pDefaultLayout = nullptr;

    friend struct SWorkspaceLayoutData;
};
