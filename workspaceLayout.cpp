#include "workspaceLayout.hpp"
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/managers/LayoutManager.hpp>


SWorkspaceLayoutWindowData* CWorkspaceLayout::getDataFromWindow(CWindow* pWindow, bool create) {
    for (auto& nd : m_vWorkspaceWindowData) {
        if (nd.pWindow == pWindow)
            return &nd;
    }

		//Create it if we don't have it, using the window's current workspaceID.
		SWorkspaceLayoutWindowData *WINDOWDATA = nullptr;
		if (create) {
	 		WINDOWDATA = &m_vWorkspaceWindowData.emplace_back();
			WINDOWDATA->pWindow = pWindow;
			WINDOWDATA->workspaceID = pWindow->m_iWorkspaceID;
		} 
		return WINDOWDATA; 
}

void CWorkspaceLayout::setupWorkspace(CWorkspace *pWorkspace) {
	if (!pWorkspace) {
		//??
		return;
	}
	IHyprLayout *wlret = nullptr;
	wlret = findUserLayoutForWorkspace(pWorkspace);
	bool isDefault = false;
	if (!wlret) {
			isDefault = true;
			wlret = m_pDefaultLayout;
	} 
 	setLayoutForWorkspace(wlret, pWorkspace, isDefault);
}
void CWorkspaceLayout::onEnable() {

	for (auto &wsp : g_pCompositor->m_vWorkspaces) {
		setupWorkspace(wsp.get());
	}
}

void CWorkspaceLayout::onDisable() {

	for (auto &wsd : m_vWorkspacesData) {
		if (wsd.layout) {
			wsd.layout->onDisable();
		}
	}
	m_vWorkspacesData.clear();
}

void CWorkspaceLayout::onWindowCreated(CWindow* pWindow, eDirection direction) {

	if (!pWindow) return; //??
	auto WDATA = getDataFromWindow(pWindow);
	if (!WDATA) return;
	auto const WSID = WDATA->workspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout) {
		return layout->onWindowCreated(pWindow, direction);
	}

}


void CWorkspaceLayout::onWindowCreatedTiling(CWindow* pWindow, eDirection direction) {
	if (!pWindow) return; //??
	auto WDATA = getDataFromWindow(pWindow);
	if (!WDATA) return;
	auto const WSID = WDATA->workspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout) {
		layout->onWindowCreatedTiling(pWindow, direction);
	}
}

void CWorkspaceLayout::onWindowCreatedFloating(CWindow* pWindow) {

	if (!pWindow) return; //??
	auto WDATA = getDataFromWindow(pWindow);
	if (!WDATA) return;
	auto const WSID = WDATA->workspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout) {
		return layout->onWindowCreatedFloating(pWindow);
	}
}

bool CWorkspaceLayout::isWindowTiled(CWindow* pWindow) {
	if (!pWindow) return false; //??
	auto const WSID = pWindow->m_iWorkspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout)
		return layout->isWindowTiled(pWindow);
	return false;
}


void CWorkspaceLayout::onWindowRemoved(CWindow* pWindow) {
	if (!pWindow) return; //??
	auto WDATA = getDataFromWindow(pWindow, false);
	if (!WDATA) return;
	auto const WSID = WDATA->workspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout) {
		if (WDATA) //??
			m_vWorkspaceWindowData.remove(*WDATA);
		return layout->onWindowRemoved(pWindow);
	}

}

void CWorkspaceLayout::onWindowRemovedTiling(CWindow* pWindow) {
	if (!pWindow) return; //??
	auto WDATA = getDataFromWindow(pWindow, false);
	if (!WDATA) return;
	auto const WSID = WDATA->workspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout)
	{
		if (WDATA) //??
			m_vWorkspaceWindowData.remove(*WDATA);

		layout->onWindowRemovedTiling(pWindow);
	}

}

void CWorkspaceLayout::onWindowRemovedFloating(CWindow* pWindow) {
	if (!pWindow) return; //??
	auto WDATA = getDataFromWindow(pWindow, false);
	if (WDATA) return;
	auto const WSID = WDATA->workspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout) {
		if (WDATA) //??
			m_vWorkspaceWindowData.remove(*WDATA);
		return layout->onWindowRemovedFloating(pWindow);
	}

}

void CWorkspaceLayout::recalculateMonitor(const int& monID) {
	const auto PMONITOR = g_pCompositor->getMonitorFromID(monID);
	if (!PMONITOR)
		return;

	if (PMONITOR->specialWorkspaceID) {
		const auto PSPWS = g_pCompositor->getWorkspaceByID(PMONITOR->specialWorkspaceID);
		if (PSPWS) {
			IHyprLayout *layout = getLayoutForWorkspace(PSPWS->m_iID);
			if (layout)
				return layout->recalculateMonitor(monID);
		}
	}
	const auto PWORKSPACE = g_pCompositor->getWorkspaceByID(PMONITOR->activeWorkspace);
	if (!PWORKSPACE) return;
	IHyprLayout *layout = getLayoutForWorkspace(PWORKSPACE->m_iID);
	if (layout)
		return layout->recalculateMonitor(monID);
}

void CWorkspaceLayout::recalculateWindow(CWindow* pWindow) {
	if (!pWindow) return; //??
	auto const WSID = pWindow->m_iWorkspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout)
		return layout->recalculateWindow(pWindow);
}

void CWorkspaceLayout::changeWindowFloatingMode(CWindow* pWindow) {
	if (!pWindow) return; //??
	auto const WSID = pWindow->m_iWorkspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout)
		return layout->changeWindowFloatingMode(pWindow);
}

void CWorkspaceLayout::onBeginDragWindow() {
  const auto pWindow = g_pInputManager->currentlyDraggedWindow;
	if (!pWindow) return; //??
	auto const WSID = pWindow->m_iWorkspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout) {
        	layout->onBeginDragWindow();

        	m_vBeginDragXY = layout->m_vBeginDragXY;
        	m_vBeginDragPositionXY = layout->m_vBeginDragPositionXY;
        	m_vBeginDragSizeXY = layout->m_vBeginDragSizeXY;
        	m_vLastDragXY = layout->m_vLastDragXY;
        	m_vDraggingWindowOriginalFloatSize = layout->m_vDraggingWindowOriginalFloatSize;
        	m_eGrabbedCorner = layout->m_eGrabbedCorner;
    	}
}

void CWorkspaceLayout::resizeActiveWindow(const Vector2D& vec, eRectCorner corner, CWindow* pWindow) {
	if (!pWindow) return; //??
	auto const WSID = pWindow->m_iWorkspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout)
		return layout->resizeActiveWindow(vec, corner, pWindow);

}

void CWorkspaceLayout::moveActiveWindow(const Vector2D& vec, CWindow* pWindow) {
	if (!pWindow) return; //??
	auto const WSID = pWindow->m_iWorkspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout)
		return layout->moveActiveWindow(vec, pWindow);

}

void CWorkspaceLayout::onEndDragWindow() {

  const auto pWindow = g_pInputManager->currentlyDraggedWindow;
	if (!pWindow) return; //??
	auto const WSID = pWindow->m_iWorkspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);

	if (layout) {
        	layout->m_vBeginDragXY = m_vBeginDragXY;
        	layout->m_vBeginDragPositionXY = m_vBeginDragPositionXY;
        	layout->m_vBeginDragSizeXY = m_vBeginDragSizeXY;
        	layout->m_vLastDragXY = m_vLastDragXY;
        	layout->m_vDraggingWindowOriginalFloatSize = m_vDraggingWindowOriginalFloatSize;
        	layout->m_eGrabbedCorner = m_eGrabbedCorner;

        	return layout->onEndDragWindow();
    	}
}

void CWorkspaceLayout::onMouseMove(const Vector2D& vec) {
  const auto pWindow = g_pInputManager->currentlyDraggedWindow;
	if (!pWindow) return; //??
	auto const WSID = pWindow->m_iWorkspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);

	if (layout) {
        	layout->m_vBeginDragXY = m_vBeginDragXY;
        	layout->m_vBeginDragPositionXY = m_vBeginDragPositionXY;
        	layout->m_vBeginDragSizeXY = m_vBeginDragSizeXY;
        	layout->m_vLastDragXY = m_vLastDragXY;
        	layout->m_vDraggingWindowOriginalFloatSize = m_vDraggingWindowOriginalFloatSize;
        	layout->m_eGrabbedCorner = m_eGrabbedCorner;

        	return layout->onMouseMove(vec);
    	}
}

void CWorkspaceLayout::fullscreenRequestForWindow(CWindow* pWindow, eFullscreenMode fsMode, bool force) {
	if (!pWindow) return; //??
	auto const WSID = pWindow->m_iWorkspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout)
		return layout->fullscreenRequestForWindow(pWindow, fsMode, force);
}

std::any CWorkspaceLayout::layoutMessage(SLayoutMessageHeader header, std::string message) {
    CVarList vars(message, 0, ' ');

    if (vars.size() < 1 || vars[0].empty()) {
        Debug::log(ERR, "layoutmsg called without params");
        return 0;
    }

    auto command = vars[0];

		const auto PWINDOW = header.pWindow;
		auto const WSID = PWINDOW->m_iWorkspaceID;
		if (command == "setlayout" && (vars.size() == 2)) {
			IHyprLayout *layout = getLayoutByName(vars[1]);
			if (layout) {
				setLayoutForWorkspace(layout, WSID); 
			}
			return 0;
		} else {
			IHyprLayout *layout = getLayoutForWorkspace(WSID);
			if (layout)
				return layout->layoutMessage(header, message);
		}

	return 0;
}


SWindowRenderLayoutHints CWorkspaceLayout::requestRenderHints(CWindow* pWindow) {
	SWindowRenderLayoutHints hints;
	if (!pWindow) return hints; //??
	auto const WSID = pWindow->m_iWorkspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout)
		return layout->requestRenderHints(pWindow);
	return hints;
}

void CWorkspaceLayout::switchWindows(CWindow* pWindow, CWindow* pWindow2) {

	if (!pWindow) return; //??
	auto const WSID = pWindow->m_iWorkspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout)
		return layout->switchWindows(pWindow, pWindow2);
}

void CWorkspaceLayout::moveWindowTo(CWindow* pWindow, const std::string& direction) {
	if (!pWindow) return; //??
	auto const WSID = pWindow->m_iWorkspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout)
		return layout->moveWindowTo(pWindow, direction);

}

void CWorkspaceLayout::alterSplitRatio(CWindow* pWindow, float ratio, bool exact) {

	if (!pWindow) return; //??
	auto const WSID = pWindow->m_iWorkspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout)
		return layout->alterSplitRatio(pWindow, ratio, exact);
}

std::string CWorkspaceLayout::getLayoutName() {
	return "WSLayout";
}

CWindow* CWorkspaceLayout::getNextWindowCandidate(CWindow* pWindow) {
	if (!pWindow) return nullptr; //??
	auto const WSID = pWindow->m_iWorkspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout)
		return layout->getNextWindowCandidate(pWindow);
	return nullptr;

}

void CWorkspaceLayout::onWindowFocusChange(CWindow* pWindow) {

	if (!pWindow) return; //??
	auto const WSID = pWindow->m_iWorkspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout)
		return layout->onWindowFocusChange(pWindow);
}

void CWorkspaceLayout::replaceWindowDataWith(CWindow* from, CWindow* to) {
	if (!from) return; //??
	auto const WSID = from->m_iWorkspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout)
		return layout->replaceWindowDataWith(from, to);

}

bool CWorkspaceLayout::isWindowReachable(CWindow* pWindow) {
	if (!pWindow) return false; //??
	auto const WSID = pWindow->m_iWorkspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout)
		return layout->isWindowReachable(pWindow);
	return false;

}

void CWorkspaceLayout::bringWindowToTop(CWindow* pWindow) {
	if (!pWindow) return; //??
	auto const WSID = pWindow->m_iWorkspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout)
		return layout->bringWindowToTop(pWindow);

}

void CWorkspaceLayout::requestFocusForWindow(CWindow* pWindow) {
	if (!pWindow) return; //??
	auto const WSID = pWindow->m_iWorkspaceID;
	IHyprLayout *layout = getLayoutForWorkspace(WSID);
	if (layout)
		return layout->requestFocusForWindow(pWindow);

}


IHyprLayout *CWorkspaceLayout::findUserLayoutForWorkspace(CWorkspace *pWorkspace) {
	std::string retName = "";
		const auto wsrule = g_pConfigManager->getWorkspaceRuleFor(pWorkspace);
		const auto layoutopts = wsrule.layoutopts;
		if (layoutopts.contains("wslayout-layout")) {
			retName = layoutopts.at("wslayout-layout");
		}
		return getLayoutByName(retName);
}


IHyprLayout *CWorkspaceLayout::getLayoutForWorkspace(const int &ws) {

	for (auto& w : m_vWorkspacesData) {
		if (w.workspaceID == ws && w.layout)
		{
			return w.layout;
		}
	}
	return nullptr;
}



void CWorkspaceLayout::setLayoutForWorkspace(IHyprLayout *layout, CWorkspace *pWorkspace, bool isDefault) {

	setLayoutForWorkspace(layout, pWorkspace->m_iID, isDefault);
}



void CWorkspaceLayout::setLayoutForWorkspace(IHyprLayout *layout, const int& ws, bool isDefault) {
	for (auto& w : m_vWorkspacesData) {
		if (w.workspaceID == ws) {
			if (w.layout) {
				for (auto &win : g_pCompositor->m_vWindows) {
					if ((win->m_iWorkspaceID != ws) || !win->m_bIsMapped || win->isHidden())
						continue;
					w.layout->onWindowRemoved(win.get());
				}
			}

			w.layout = layout;
			w.isDefault = isDefault;
			for (auto &win : g_pCompositor->m_vWindows) {
				if ((win->m_iWorkspaceID != ws) || !win->m_bIsMapped || win->isHidden())
					continue;
				onWindowCreated(win.get());
			}
			return;
		}
	}
	//Not found, create a new entry
	const auto WSENTRY = &m_vWorkspacesData.emplace_back();
	WSENTRY->workspaceID = ws;
	WSENTRY->layout = layout;
	WSENTRY->isDefault = isDefault;
	for (auto &win : g_pCompositor->m_vWindows) {
		if ((win->m_iWorkspaceID != ws) || !win->m_bIsMapped || win->isHidden())
			continue;
		onWindowCreated(win.get());
	}
}

void CWorkspaceLayout::setDefaultLayout(std::string name) {
	IHyprLayout *layout = getLayoutByName(name);
	if (layout)
	{
		m_pDefaultLayout = layout;
		for (auto& w : m_vWorkspacesData) {
			if (w.isDefault) {
				setLayoutForWorkspace(m_pDefaultLayout, w.workspaceID, true);
			}
		}
	}
}


IHyprLayout *CWorkspaceLayout::getLayoutByName(const std::string& name) {
	for (auto &layoutp : g_pLayoutManager->m_vLayouts) {
		if(layoutp.first == name) {
			return layoutp.second;
		}
	}
	return nullptr;
}

void CWorkspaceLayout::clearLayoutMaps() {
}

