local BG_COLOR     = "#121212FF"
local TEXT_COLOR   = "#D4D4D4FF"
local ACCENT_COLOR = "#007ACCFF"
local CARD_BG      = "#1E1E1EFF"
local HOVER_COLOR  = "#2D2D2DFF"

function DrawWelcomeScreen()
    local winW, winH = Cherry.GetWindowSize()
    
    Cherry.DrawRect(0, 0, winW, winH, BG_COLOR)

    local marginX = 60
    local currentY = 70
    local colWidth = (winW - (marginX * 3)) / 2
    Cherry.PushFont("Clash")
    Cherry.DrawText(marginX, currentY, 36.0, "#FFFFFFFF", "Welcome to Vortex")
    Cherry.PopFont()
    currentY = currentY + 45
    Cherry.DrawText(marginX, currentY, 18.0, "#777777FF", "An open playground for creators & hackers.")
    
    currentY = currentY + 70
    
    local contentStartY = currentY

    local leftY = contentStartY
    
    Cherry.DrawText(marginX, leftY, 20.0, TEXT_COLOR, "Quick Start")
    leftY = leftY + 40
    
    local iconSize = 80
    local spacing = 30
    
    Cherry.BeginComponent("welcome_browser")
    if DrawAppIcon(marginX, leftY, iconSize, "Browser", Cherry.GetPath("resources/imgs/folder.png")) then 
        Cherry.SetComponentData("welcome_browser", "isClicked","true")
    else
        Cherry.SetComponentData("welcome_browser", "isClicked","false")
    end
    Cherry.EndComponent()
    
    Cherry.BeginComponent("welcome_terminal")
    if DrawAppIcon(marginX + (iconSize + spacing), leftY, iconSize, "Terminal", Cherry.GetPath("resources/imgs/terminal.png")) then 
        Cherry.SetComponentData("welcome_terminal", "isClicked","true")
    else
        Cherry.SetComponentData("welcome_terminal", "isClicked","false")
    end
    Cherry.EndComponent()

    Cherry.BeginComponent("welcome_settings")
    if DrawAppIcon(marginX + (iconSize + spacing) * 2, leftY, iconSize, "Settings", Cherry.GetPath("resources/imgs/wheel.png")) then 
        Cherry.SetComponentData("welcome_settings", "isClicked","true")
    else
        Cherry.SetComponentData("welcome_settings", "isClicked","false")
    end
    Cherry.EndComponent()
    
    leftY = leftY + iconSize + 85

    Cherry.DrawText(marginX, leftY, 20.0, TEXT_COLOR, "Quick Actions")
    leftY = leftY + 35
    
    Cherry.BeginComponent("open_terminal")
    if DrawMenuButton(marginX, leftY, colWidth, "Open new terminal...", "Ctrl+T") then 
        Cherry.SetComponentData("open_terminal", "isClicked","true")
    else        
        Cherry.SetComponentData("open_terminal", "isClicked","false")
    end
    Cherry.EndComponent()

    leftY = leftY + 35
    Cherry.BeginComponent("open_content_browser")
    if DrawMenuButton(marginX, leftY, colWidth, "Open new content browser...", "Ctrl+B") then
        Cherry.SetComponentData("open_content_browser", "isClicked","true")
    else        
        Cherry.SetComponentData("open_content_browser", "isClicked","false")
    end
    Cherry.EndComponent()
    
    leftY = leftY + 35
    Cherry.BeginComponent("open_project_settings")
    if DrawMenuButton(marginX, leftY, colWidth, "Open project settings...", "") then
        Cherry.SetComponentData("open_project_settings", "isClicked","true")
    else        
        Cherry.SetComponentData("open_project_settings", "isClicked","false")
    end
    Cherry.EndComponent()

    local rightColX = marginX + colWidth + marginX
    local walkY = contentStartY 
    
    Cherry.DrawText(rightColX, walkY, 20.0, TEXT_COLOR, "Walkthroughs")
    walkY = walkY + 40

    Cherry.BeginComponent("taking_control")
    if DrawWalkthroughCard(rightColX, walkY, colWidth, 80, "Taking control of Vortex", "Learn the basics of the Vortex Creation platform") then
        Cherry.SetComponentData("taking_control", "isClicked","true")
    else        
        Cherry.SetComponentData("taking_control", "isClicked","false")
    end
    Cherry.EndComponent()

    walkY = walkY + 95
    Cherry.BeginComponent("learn_modules")
    if DrawWalkthroughCard(rightColX, walkY, colWidth, 80, "Work with modules", "Learn how modules work and add your first module") then
        Cherry.SetComponentData("learn_modules", "isClicked","true")
    else        
        Cherry.SetComponentData("learn_modules", "isClicked","false")
    end
    Cherry.EndComponent()

    walkY = walkY + 95
    Cherry.BeginComponent("visit_website")
    if DrawWalkthroughCard(rightColX, walkY, colWidth, 80, "Visit the Vortex website", "Visit the website, discover documentation, tutorials and all news !") then 
        Cherry.SetComponentData("visit_website", "isClicked","true")
    else        
        Cherry.SetComponentData("visit_website", "isClicked","false")
    end
    Cherry.EndComponent()

end

function DrawAppIcon(x, y, size, label, iconPath)
    local isHover = IsMouseOver(x, y, size, size)
    
    if isHover then
        Cherry.DrawRectRounded(x - 10, y - 10, size + 20, size + 45, 8.0, HOVER_COLOR)
    end
    
    Cherry.DrawImage(x, y, size, size - 5, iconPath)
    
    Cherry.DrawText(x + (size/2) - 25, y + size + 10, 14.0, isHover and "#FFFFFFFF" or "#AAAAAAFF", label)
    
    return Cherry.IsMouseClickedOnCurrentPos(x - 10, y - 10, size + 20, size + 45, 0)
end

function DrawMenuButton(x, y, w, label, shortcut)
    local h = 30
    local isHover = IsMouseOver(x, y, w, h)
    
    if isHover then
        Cherry.DrawRect(x, y, w, h, HOVER_COLOR)
    end
    
    Cherry.DrawText(x + 5, y + 5, 16.0, ACCENT_COLOR, label)
    
    if shortcut ~= "" then
        Cherry.DrawText(x + w - 65, y + 7, 13.0, "#555555FF", shortcut)
    end
    
    return Cherry.IsMouseClickedOnCurrentPos(x, y, w, h, 0)
end

function DrawWalkthroughCard(x, y, w, h, title, desc)
    local isHover = IsMouseOver(x, y, w, h)
    local bg = isHover and HOVER_COLOR or CARD_BG
    
    Cherry.DrawRectRounded(x, y, w, h, 4.0, bg)
    Cherry.DrawRectOutline(x, y, w, h, "#333333FF", 1.0)
    
    Cherry.DrawText(x + 15, y + 15, 17.0, "#FFFFFFFF", title)

    Cherry.Sameline()
    Cherry.DrawImage(x + 45 + Cherry.CalcTextSizeX(title), y + 15, 18, 18, Cherry.GetPath("resources/imgs/icons/misc/icon_redirect.png"));
    
    Cherry.DrawText(x + 15, y + 40, 14.0, "#888888FF", desc)
    
    return Cherry.IsMouseClickedOnCurrentPos(x, y, w, h, 0)
end

function IsMouseOver(x, y, w, h)
    return Cherry.IsMouseOverRect(x, y, w, h)
end

DrawWelcomeScreen()