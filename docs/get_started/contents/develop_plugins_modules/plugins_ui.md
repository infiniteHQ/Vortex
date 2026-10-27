### Create windows

You can create custom windows interfaces with Cherry, our UI framework, and this only with Lua ! 

You can create a window directly on the script that is executed like that :

```lua
Vortex.Window("My Window Name", function()
    -- You can call every Cherry or Vortex functions here, in the window
    Cherry.DrawRect(100, 100, 500, 500, "#FF0000FF")
    if Cherry.IsMouseClickedOnCurrentPos(100, 100, 500, 500, 0) then 
        Vortex.Log("Clicked !!!")
    endd
end, "none")
```

You can also create window in seperate lua files, so for example create a "ui" folder for example and put every rendering code here. You can create a gui.lua to make a little test.


```lua
-- In main.lua (or other executed scripts)
Vortex.Window("Hello World2", "src/ui/gui.lua", "none")
```

```lua
-- In gui.lua (src/gui)
Cherry.DrawRect(100, 100, 500, 500, "#0000FFFF")
if Cherry.IsMouseClickedOnCurrentPos(100, 100, 500, 500, 0) then 
    Vortex.Log("Clicked from file !!!")
end
```

### Create your interfaces

You can so use cherry to create cool interfaces ! You can know refer to the Cherry Lua API references to see all functions you can call !

Cherry Logical API (effects, i/o, events, hooks)
Cherry Draw API (shapes, text, lines, colours)