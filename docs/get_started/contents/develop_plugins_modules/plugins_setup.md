
### Setup the plugin development

Let's start! Create a folder and open it with your code editor. We recommend using git, to track progress and make versions of your plugin.

### Create base files

To form a valid plugin architecture, create these files at the root of the folder:

Create "plugin.json" with these default values:

```json
{
  "type": "ep",
  "group": "Basic tools",
  "name": "yourname.yourpluginname",
  "proper_name": "Your Plugin Name",
  "auto_exec": true,
  "version": "1.0.0",
  "author": "yourname",
  "contributors": [
    "You"
  ],
  "deps": [],
  "compatibility": [
    "1.0"
  ],
  "path": "./src/main.lua",
  "picture": "./resources/logo.png",
  "description": "Your plugin description"
}
```

Then, create a "src" folder that will contain all of your source code files.

Inside the "src" folder, create the file "main.lua".

Inside main.lua, write this piece of example code:

```lua
Vortex.Log("Hello World !")
```

You can also create a "resources" folder and put your plugin logo in it, as logo.png for example.

### Testing

You can test your plugin by creating a prototype project with all the modules and contents you want, and copying the folder containing your plugin into the .vx/plugins/ folder of the project.

You will be able to see the log in the Console logs utility.

### What's next?

You've just created your very own plugin! You can now navigate through all the following pages, such as "Program plugin logic", to learn how to use important functions you can call in your Lua files.

You can also go to "Program plugin interface" to learn how to create windows, buttons, texts and more!

You also have the Vortex Plugins API Reference to see all the functions you can call and which parameters to pass.