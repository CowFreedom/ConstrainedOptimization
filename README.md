# TemplatePlugin #

**TemplatePlugin** is a **UG4-Plugin** that serves as a starting point for the creation of new plugins.

Copyright 2011-2016 Goethe Center for Scientific Computing, University Frankfurt

Please install/clone this repository through UG4's package manager
[ughub](https://github.com/UG4/ughub):

    ughub install TemplatePlugin

    
## Using TemplatePlugin as a base for your own plugin ##
In order to use this plugin as a base for your own plugin, please copy 'template_plugin.cpp'
and 'CMakeLists.txt' to your plugin folder (e.g. 'plugins/YourPlugin') and adjust
the filname accordingly (e.g. 'template_plugin.cpp' -> 'your_plugin.cpp').

Furthermore, in 'CMakeLists.txt', change the plugin name from 'TemplatePlugin' to
the name of your plugin and adjust the sources:

    set(pluginName	YourPlugin)
    set(SOURCES		your_plugin.cpp)

Afterwards, in 'your_plugin.cpp', rename the namespace 'TemplatePlugin' to 'YourPlugin' and
rename the function 'InitUGPlugin_TemplatePlugin' to

    InitUGPlugin_YourPlugin(Registry* reg, string grp)

You can now activate your plugin and build it along with UG4 in the same way as
the other plugins, too. To this go to your build-directory and type:

    cmake -DYourPlugin=ON .
