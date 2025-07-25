#include "workspaceLayout.hpp"

SWorkspaceLayoutWindowData* CWorkspaceLayout::getDataFromWindow(PHLWINDOW pWindow, bool create) {
    for (auto& nd : m_vWorkspaceWindowData) {
        if (nd.pWindow.lock() == pWindow) {
            return &nd;
        }
    }

    //Create it if we don't have it, using the window's current workspaceID.
    SWorkspaceLayoutWindowData* WINDOWDATA = nullptr;
    if (create) {
        WINDOWDATA              = &m_vWorkspaceWindowData.emplace_back();
        WINDOWDATA->pWindow     = pWindow;
        WINDOWDATA->workspaceID = pWindow->workspaceID();
    }
    return WINDOWDATA;
}

void CWorkspaceLayout::setupWorkspace(PHLWORKSPACE pWorkspace) {
    if (!pWorkspace) {
        //??
        return;
    }
    IHyprLayout* wlret = nullptr;
    wlret              = findUserLayoutForWorkspace(pWorkspace);
    bool isDefault     = false;
    if (!wlret) {
        isDefault = true;
        wlret     = m_pDefaultLayout;
    }
    setLayoutForWorkspace(wlret, pWorkspace, isDefault);
}
void CWorkspaceLayout::onEnable() {

    for (auto& wsp : g_pCompositor->getWorkspaces()) {
        setupWorkspace(wsp.lock());
    }
}

void CWorkspaceLayout::onDisable() {

    for (auto& wsd : m_vWorkspacesData) {
        if (wsd.layout) {
            wsd.layout->onDisable();
        }
    }
    m_vWorkspacesData.clear();
}

void CWorkspaceLayout::onWindowCreated(PHLWINDOW pWindow, eDirection direction) {

    if (!pWindow)
        return; //??
    auto WDATA = getDataFromWindow(pWindow);
    if (!WDATA)
        return;
    auto const   WSID   = WDATA->workspaceID;
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout) {
        return layout->onWindowCreated(pWindow, direction);
    }
}

void CWorkspaceLayout::onWindowCreatedTiling(PHLWINDOW pWindow, eDirection direction) {
    if (!pWindow)
        return; //??
    auto WDATA = getDataFromWindow(pWindow);
    if (!WDATA)
        return;
    auto const   WSID   = WDATA->workspaceID;
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout) {
        layout->onWindowCreatedTiling(pWindow, direction);
    }
}

void CWorkspaceLayout::onWindowCreatedFloating(PHLWINDOW pWindow) {

    if (!pWindow)
        return; //??
    auto WDATA = getDataFromWindow(pWindow);
    if (!WDATA)
        return;
    auto const   WSID   = WDATA->workspaceID;
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout) {
        return layout->onWindowCreatedFloating(pWindow);
    }
}

bool CWorkspaceLayout::isWindowTiled(PHLWINDOW pWindow) {
    if (!pWindow)
        return false; //??
    auto const   WSID   = pWindow->workspaceID();
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout)
        return layout->isWindowTiled(pWindow);
    return false;
}

void CWorkspaceLayout::onWindowRemoved(PHLWINDOW pWindow) {
    if (!pWindow)
        return; //??
    auto         WDATA  = getDataFromWindow(pWindow, false);
    auto const   WSID   = WDATA ? WDATA->workspaceID : pWindow->workspaceID();
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout) {
        if (WDATA) //??
            m_vWorkspaceWindowData.remove(*WDATA);
        return layout->onWindowRemoved(pWindow);
    }
}

void CWorkspaceLayout::onWindowRemovedTiling(PHLWINDOW pWindow) {
    if (!pWindow)
        return; //??
    auto         WDATA  = getDataFromWindow(pWindow, false);
    auto const   WSID   = WDATA ? WDATA->workspaceID : pWindow->workspaceID();
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout) {
        if (WDATA) //??
            m_vWorkspaceWindowData.remove(*WDATA);

        layout->onWindowRemovedTiling(pWindow);
    }
}

void CWorkspaceLayout::onWindowRemovedFloating(PHLWINDOW pWindow) {
    if (!pWindow)
        return; //??
    auto WDATA = getDataFromWindow(pWindow, false);
    if (WDATA)
        return;
    auto const   WSID   = WDATA->workspaceID;
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout) {
        if (WDATA) //??
            m_vWorkspaceWindowData.remove(*WDATA);
        return layout->onWindowRemovedFloating(pWindow);
    }
}

void CWorkspaceLayout::recalculateMonitor(const MONITORID& monID) {
    const auto PMONITOR = g_pCompositor->getMonitorFromID(monID);
    if (!PMONITOR)
        return;

    if (PMONITOR->m_activeSpecialWorkspace) {
        const auto PSPWS = PMONITOR->m_activeSpecialWorkspace;
        if (PSPWS) {
            IHyprLayout* layout = getLayoutForWorkspace(PSPWS->m_id);
            if (layout)
                return layout->recalculateMonitor(monID);
        }
    }
    const auto PWORKSPACE = PMONITOR->m_activeWorkspace;
    if (!PWORKSPACE)
        return;
    IHyprLayout* layout = getLayoutForWorkspace(PWORKSPACE->m_id);
    if (layout)
        return layout->recalculateMonitor(monID);
}

void CWorkspaceLayout::recalculateWindow(PHLWINDOW pWindow) {
    if (!pWindow)
        return; //??
    auto const   WSID   = pWindow->workspaceID();
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout)
        return layout->recalculateWindow(pWindow);
}

void CWorkspaceLayout::changeWindowFloatingMode(PHLWINDOW pWindow) {
    if (!pWindow)
        return; //??
    auto const   WSID   = pWindow->workspaceID();
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout)
        return layout->changeWindowFloatingMode(pWindow);
}

void CWorkspaceLayout::onBeginDragWindow() {
    const auto pWindow = g_pInputManager->m_currentlyDraggedWindow.lock();
    if (!pWindow)
        return; //??
    auto const   WSID   = pWindow->workspaceID();
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout) {
        layout->onBeginDragWindow();

        m_beginDragXY                     = layout->m_beginDragXY;
        m_beginDragPositionXY             = layout->m_beginDragPositionXY;
        m_beginDragSizeXY                 = layout->m_beginDragSizeXY;
        m_lastDragXY                      = layout->m_lastDragXY;
        m_draggingWindowOriginalFloatSize = layout->m_draggingWindowOriginalFloatSize;
        m_grabbedCorner                   = layout->m_grabbedCorner;
    }
}

void CWorkspaceLayout::resizeActiveWindow(const Vector2D& vec, eRectCorner corner, PHLWINDOW pWindow) {
    const auto PWINDOW = pWindow ? pWindow : g_pCompositor->m_lastWindow.lock();
    if (!validMapped(PWINDOW))
        return; //??
    auto const   WSID   = PWINDOW->workspaceID();
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout)
        return layout->resizeActiveWindow(vec, corner, PWINDOW);
}

void CWorkspaceLayout::moveActiveWindow(const Vector2D& vec, PHLWINDOW pWindow) {
    const auto PWINDOW = pWindow ? pWindow : g_pCompositor->m_lastWindow.lock();
    if (!validMapped(PWINDOW))
        return; //??
    auto const   WSID   = PWINDOW->workspaceID();
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout)
        return layout->moveActiveWindow(vec, PWINDOW);
}

void CWorkspaceLayout::onEndDragWindow() {

    const auto pWindow = g_pInputManager->m_currentlyDraggedWindow.lock();
    if (!validMapped(pWindow))
        return; //??
    auto const   WSID   = pWindow->workspaceID();
    IHyprLayout* layout = getLayoutForWorkspace(WSID);

    if (layout) {
        layout->m_beginDragXY                     = m_beginDragXY;
        layout->m_beginDragPositionXY             = m_beginDragPositionXY;
        layout->m_beginDragSizeXY                 = m_beginDragSizeXY;
        layout->m_lastDragXY                      = m_lastDragXY;
        layout->m_draggingWindowOriginalFloatSize = m_draggingWindowOriginalFloatSize;
        layout->m_grabbedCorner                   = m_grabbedCorner;

        layout->onEndDragWindow();
        auto WDATA = getDataFromWindow(pWindow);
        if (WDATA) {
            //The layout's onEndDragWindow called onWindowCreatedTiling internally. Update our map to reflect the new workspace for the window)
            WDATA->workspaceID = WSID;
        }
    }
}

void CWorkspaceLayout::onMouseMove(const Vector2D& vec) {
    const auto pWindow = g_pInputManager->m_currentlyDraggedWindow.lock();
    if (!validMapped(pWindow))
        return; //??
    auto const   WSID   = pWindow->workspaceID();
    IHyprLayout* layout = getLayoutForWorkspace(WSID);

    if (layout) {
        layout->m_beginDragXY                     = m_beginDragXY;
        layout->m_beginDragPositionXY             = m_beginDragPositionXY;
        layout->m_beginDragSizeXY                 = m_beginDragSizeXY;
        layout->m_lastDragXY                      = m_lastDragXY;
        layout->m_draggingWindowOriginalFloatSize = m_draggingWindowOriginalFloatSize;
        layout->m_grabbedCorner                   = m_grabbedCorner;

        layout->onMouseMove(vec);

        m_lastDragXY = layout->m_lastDragXY;
    }
}

void CWorkspaceLayout::fullscreenRequestForWindow(PHLWINDOW pWindow, const eFullscreenMode CURRENT_EFFECTIVE_MODE, const eFullscreenMode EFFECTIVE_MODE) {
    if (!pWindow)
        return; //??
    auto const   WSID   = pWindow->workspaceID();
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout)
        return layout->fullscreenRequestForWindow(pWindow, CURRENT_EFFECTIVE_MODE, EFFECTIVE_MODE);
}

std::any CWorkspaceLayout::layoutMessage(SLayoutMessageHeader header, std::string message) {
    CVarList vars(message, 0, ' ');

    if (vars.size() < 1 || vars[0].empty()) {
        Debug::log(ERR, "layoutmsg called without params");
        return 0;
    }

    auto       command = vars[0];

    const auto PWINDOW = header.pWindow;
    auto const WSID    = PWINDOW->workspaceID();
    if (command == "setlayout" && (vars.size() == 2)) {
        IHyprLayout* layout = getLayoutByName(vars[1]);
        if (layout) {
            setLayoutForWorkspace(layout, WSID);
        }
        return 0;
    } else if (command == "cyclelayout") {
        size_t       layoutidx = 0;

        IHyprLayout* wslayout = getLayoutForWorkspace(WSID);

        for (size_t i = 0; i < m_vLayoutList.size(); i++) {
            if (m_vLayoutList[i] == wslayout) {
                layoutidx = i;
                break;
            }
        }
        if (vars.size() == 1 || vars[1].contains("next")) {
            layoutidx++;
        } else if (vars.size() == 2 && vars[1].contains("prev")) {
            if (layoutidx == 0)
                layoutidx = m_vLayoutList.size() - 1;
            else
                layoutidx--;
        }

        if (layoutidx >= m_vLayoutList.size())
            layoutidx = 0;
        IHyprLayout* newLayout = m_vLayoutList[layoutidx];
        if (newLayout)
            setLayoutForWorkspace(newLayout, WSID);
    } else if (command == "togglelayout") {
        IHyprLayout* prevLayout = getPreviousLayoutForWorkspace(WSID);
        IHyprLayout* currLayout = getLayoutForWorkspace(WSID);
        IHyprLayout* newLayout  = nullptr;
        if (vars.size() == 2) {
            newLayout = getLayoutByName(vars[1]);
            if (newLayout) {
                if (newLayout == currLayout)
                    setLayoutForWorkspace(prevLayout, WSID);
                else
                    setLayoutForWorkspace(newLayout, WSID);
            }
        } else {
            setLayoutForWorkspace(prevLayout, WSID);
        }
    } else {
        IHyprLayout* layout = getLayoutForWorkspace(WSID);
        if (layout)
            return layout->layoutMessage(header, message);
    }

    return 0;
}

SWindowRenderLayoutHints CWorkspaceLayout::requestRenderHints(PHLWINDOW pWindow) {
    SWindowRenderLayoutHints hints;
    if (!pWindow)
        return hints; //??
    auto const   WSID   = pWindow->workspaceID();
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout)
        return layout->requestRenderHints(pWindow);
    return hints;
}

Vector2D CWorkspaceLayout::predictSizeForNewWindowTiled() {
    if (!g_pCompositor->m_lastMonitor)
        return {};
    auto const   WSID   = g_pCompositor->m_lastMonitor->m_activeWorkspace->m_id;
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout)
        return layout->predictSizeForNewWindowTiled();
    return {};
}

void CWorkspaceLayout::switchWindows(PHLWINDOW pWindow, PHLWINDOW pWindow2) {

    if (!pWindow || !pWindow2)
        return; //??
    auto const   WSID1 = pWindow->workspaceID();
    auto const   WSID2 = pWindow2->workspaceID();

    IHyprLayout* layout1 = getLayoutForWorkspace(WSID1);
    IHyprLayout* layout2 = getLayoutForWorkspace(WSID2);

    if (layout1 == layout2)
        return layout1->switchWindows(pWindow, pWindow2);

    //Different layouts; hax

    std::swap(pWindow2->m_monitor, pWindow->m_monitor);
    std::swap(pWindow2->m_workspace, pWindow->m_workspace);

    pWindow->setAnimationsToMove();
    pWindow2->setAnimationsToMove();

    layout1->replaceWindowDataWith(pWindow, pWindow2);
    layout2->replaceWindowDataWith(pWindow2, pWindow);
    recalculateMonitor(pWindow->monitorID());
    recalculateMonitor(pWindow2->monitorID());

    g_pHyprRenderer->damageWindow(pWindow);
    g_pHyprRenderer->damageWindow(pWindow2);
}

void CWorkspaceLayout::moveWindowTo(PHLWINDOW pWindow, const std::string& direction, bool silent) {
    if (!pWindow)
        return; //??
    auto const   WSID   = pWindow->workspaceID();
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout) {
        layout->moveWindowTo(pWindow, direction, silent);

        auto const   NEXTID     = pWindow->workspaceID();
        IHyprLayout* nextlayout = getLayoutForWorkspace(NEXTID);

        // layout has changed -> readd
        if (nextlayout && nextlayout != layout) {
            layout->onWindowRemoved(pWindow);
            nextlayout->onWindowCreated(pWindow);
        }
    }
}

void CWorkspaceLayout::alterSplitRatio(PHLWINDOW pWindow, float ratio, bool exact) {

    if (!pWindow)
        return; //??
    auto const   WSID   = pWindow->workspaceID();
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout)
        return layout->alterSplitRatio(pWindow, ratio, exact);
}

std::string CWorkspaceLayout::getLayoutName() {
    return "WSLayout";
}

PHLWINDOW CWorkspaceLayout::getNextWindowCandidate(PHLWINDOW pWindow) {
    if (!pWindow)
        return nullptr; //??
    auto const   WSID   = pWindow->workspaceID();
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout)
        return layout->getNextWindowCandidate(pWindow);
    return nullptr;
}

void CWorkspaceLayout::onWindowFocusChange(PHLWINDOW pWindow) {

    if (!pWindow)
        return; //??
    auto const   WSID   = pWindow->workspaceID();
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout)
        return layout->onWindowFocusChange(pWindow);
}

void CWorkspaceLayout::replaceWindowDataWith(PHLWINDOW from, PHLWINDOW to) {
    if (!from)
        return; //??
    auto const   WSID   = from->workspaceID();
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout)
        return layout->replaceWindowDataWith(from, to);
}

bool CWorkspaceLayout::isWindowReachable(PHLWINDOW pWindow) {
    if (!pWindow)
        return false; //??
    auto const   WSID   = pWindow->workspaceID();
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout)
        return layout->isWindowReachable(pWindow);
    return false;
}

void CWorkspaceLayout::bringWindowToTop(PHLWINDOW pWindow) {
    if (!pWindow)
        return; //??
    auto const   WSID   = pWindow->workspaceID();
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout)
        return layout->bringWindowToTop(pWindow);
}

void CWorkspaceLayout::requestFocusForWindow(PHLWINDOW pWindow) {
    if (!pWindow)
        return; //??
    auto const   WSID   = pWindow->workspaceID();
    IHyprLayout* layout = getLayoutForWorkspace(WSID);
    if (layout)
        return layout->requestFocusForWindow(pWindow);
}

IHyprLayout* CWorkspaceLayout::findUserLayoutForWorkspace(PHLWORKSPACE pWorkspace) {
    std::string retName    = "";
    const auto  wsrule     = g_pConfigManager->getWorkspaceRuleFor(pWorkspace);
    const auto  layoutopts = wsrule.layoutopts;
    if (layoutopts.contains("wslayout-layout")) {
        retName = layoutopts.at("wslayout-layout");
    }
    return getLayoutByName(retName);
}

IHyprLayout* CWorkspaceLayout::getPreviousLayoutForWorkspace(const int& ws) {

    for (auto& w : m_vWorkspacesData) {
        if (w.workspaceID == ws && w.layout) {
            return w.previousLayout;
        }
    }
    return nullptr;
}

IHyprLayout* CWorkspaceLayout::getLayoutForWorkspace(const int& ws) {

    for (auto& w : m_vWorkspacesData) {
        if (w.workspaceID == ws && w.layout) {
            return w.layout;
        }
    }
    return nullptr;
}

void CWorkspaceLayout::setLayoutForWorkspace(IHyprLayout* layout, PHLWORKSPACE pWorkspace, bool isDefault) {

    setLayoutForWorkspace(layout, pWorkspace->m_id, isDefault);
}

void CWorkspaceLayout::setLayoutForWorkspace(IHyprLayout* layout, const int& ws, bool isDefault) {
    if (!layout)
        return;
    if (layout->getLayoutName() == getLayoutName())
        return;

    for (auto& w : m_vWorkspacesData) {
        if (w.workspaceID == ws) {
            if (w.layout) {
                if (w.layout == layout)
                  return;
                for (auto& win : g_pCompositor->m_windows) {
                    if ((win->workspaceID() != ws) || !win->m_isMapped || win->isHidden())
                        continue;
                    w.layout->onWindowRemoved(win);
                }
            }

            if (w.layout)
                w.previousLayout = w.layout;
            w.layout    = layout;
            w.isDefault = isDefault;
            for (auto& win : g_pCompositor->m_windows) {
                if ((win->workspaceID() != ws) || !win->m_isMapped || win->isHidden())
                    continue;
                onWindowCreated(win);
            }
            return;
        }
    }
    //Not found, create a new entry
    const auto WSENTRY   = &m_vWorkspacesData.emplace_back();
    WSENTRY->workspaceID = ws;
    WSENTRY->layout      = layout;
    WSENTRY->isDefault   = isDefault;
    for (auto& win : g_pCompositor->m_windows) {
        if ((win->workspaceID() != ws) || !win->m_isMapped || win->isHidden())
            continue;
        onWindowCreated(win);
    }
}

void CWorkspaceLayout::setDefaultLayout(std::string name) {
    IHyprLayout* layout = getLayoutByName(name);
    if (layout) {
        m_pDefaultLayout = layout;
        for (auto& w : m_vWorkspacesData) {
            if (w.isDefault) {
                setLayoutForWorkspace(m_pDefaultLayout, w.workspaceID, true);
            }
        }
    }
}

IHyprLayout* CWorkspaceLayout::getLayoutByName(const std::string& name) {
    for (auto& layoutp : g_pLayoutManager->m_layouts) {
        if (layoutp.first == name) {
            return layoutp.second;
        }
    }
    return nullptr;
}

void CWorkspaceLayout::clearLayoutMaps() {}

void CWorkspaceLayout::setupLayoutList() {
    static auto* const LAYOUTS = (Hyprlang::STRING const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:wslayout:layouts")->getDataStaticPtr();
    m_vLayoutList.clear();
    CVarList layoutlist(*LAYOUTS, 0, 's');
    for (size_t i = 0; i < layoutlist.size(); i++) {
        auto         layoutName = layoutlist[i];
        IHyprLayout* layout     = getLayoutByName(layoutName);
        if (layout && layout->getLayoutName() != getLayoutName())
            m_vLayoutList.push_back(layout);
    }

    if (!m_vLayoutList.size()) {
        for (auto& layoutp : g_pLayoutManager->m_layouts) {
            if (layoutp.second->getLayoutName() != getLayoutName())
                m_vLayoutList.push_back(layoutp.second);
        }
    }
}
