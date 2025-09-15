# Simple-Text-Editor
This repository contains the source code for a simple text editor built using FLTK [Fast, Light Toolkit].


# How to build and execute the GUI.cpp file?

You need to add the output of fltk-config, a script that comes with the FLTK installation, to your compilation command. Open your terminal and compile your file like this:
        g++ your_file_name.cpp -o your_program_name `fltk-config --cxxflags --ldflags`

# Solving the Squiggly-error underlining for the FLTK libraries in the include statements
## For VS Code Users

If you're using VS Code, the red squiggly lines are from the IntelliSense engine. You need to tell it where the FLTK headers are by editing your c_cpp_properties.json file.

Press Ctrl+Shift+P (or Cmd+Shift+P on Mac) and type "C/C++: Edit Configurations (JSON)".

Find the "Include path" setting. Add the path to your FLTK headers. You can find this path by running the following command in your terminal:
        fltk-config --includedir
This will output something like /usr/local/include or /opt/homebrew/include. Add this exact path to your "Include path" list.

Save the c_cpp_properties.json file. The errors should disappear.

Remember, this only fixes the IntelliSense errors. You still need to use the fltk-config cmd (g++ your_file_name.cpp -o your_program_name `fltk-config --cxxflags --ldflags`) to actually compile your project, for example, in a tasks.json file 
