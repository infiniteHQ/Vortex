### Setup the plugin development

Let's start! Create a folder and open it with your code editor. We recommend using git, to track progress and make versions of your plugin.

To live test your plugin, we recommand creating a dedicated Vortex project from the Launcher, and start your plugin base in the [project]/.vx/plugins/[yourplugin]/. With that approach you can develop your plugin directly, without compiling Vortex. Just launching this project alongside your project editor (you can also use Vortex as code editor!).

Note: You can also look at our example plugin repository, with build scripts [here](https://github.com/infiniteHQ/HelloWorldPlugin)

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

Like this page say at the beegining, you can test your plugin by creating a prototype project with all the modules and contents you want, and copying the folder containing your plugin into the .vx/plugins/ folder of the project.

You will be able to see the log in the Console logs utility.

You can restart your plugin from the plugin utility, on the Vortex menu at the top of the editor.

### What's next?

You've just created your very own plugin! You can now navigate through all the following pages, such as "Program plugin logic", to learn how to use important functions you can call in your Lua files.

You can also go to "Program plugin interface" to learn how to create windows, buttons, texts and more!

You also have the Vortex Plugins API Reference to see all the functions you can call and which parameters to pass.


Note: You can also look at our example plugin repository, with build scripts [here](https://github.com/infiniteHQ/HelloWorldPlugin), if you prefer to learn from existing projects.