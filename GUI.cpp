//using FLTK [Fast Light Toolkit] library for GUI
#include <iostream>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_MENU_BAR.H>
using namespace std;

int changed = 0;        //flag to indicate if text has been changed since last save
char filename[256] = "";    //name of current file
Fl_Text_Buffer *textbuf;    //buffer to hold text being edited

int main() {

    class Editor_Window : public Fl_Double_Window 
    {
        public:
            Editor_Window(int w, int h, const char *title);
            ~Editor_Window();

            Fl_Window *replace_window;            //actual winodow
            Fl_Input *search_text_to_replace;     //text input that specifies what to search for
            Fl_Input *replace_with;               //text input specifying what to replace with
            Fl_Button *replace_all;               //button to replace all occurences
            Fl_Return_Button *replace_next;       //button to replace next occurence
            Fl_Button *cancel_replace;            //cancel replace operation

            Fl_Text_Editor *text_editor;          //main area for text editing
            char search [256];
    };

    Fl_Menu_Item menuitems[] = {

            Fl_Menu_Item menuitems[] = {
        { "&File",              0, 0, 0, FL_SUBMENU },              
            { "&New File",        0, (Fl_Callback *)new_cb },
            { "&Open File...",    FL_COMMAND + 'o', (Fl_Callback *)open_cb },
            { "&Insert File...",  FL_COMMAND + 'i', (Fl_Callback *)insert_cb, 0, FL_MENU_DIVIDER },
            { "&Save File",       FL_COMMAND + 's', (Fl_Callback *)save_cb },
            { "Save File &As...", FL_COMMAND + FL_SHIFT + 's', (Fl_Callback *)saveas_cb, 0, FL_MENU_DIVIDER },
            { "New &View",        FL_OPTION + 'v', (Fl_Callback *)view_cb, 0 },  // Changed ALT to OPTION
            { "&Close View",      FL_COMMAND + 'w', (Fl_Callback *)close_cb, 0, FL_MENU_DIVIDER },
            { "E&xit",           FL_COMMAND + 'q', (Fl_Callback *)quit_cb, 0 },
            { 0 },

        { "&Edit", 0, 0, 0, FL_SUBMENU },                           
            { "&Undo",       FL_COMMAND + 'z', (Fl_Callback *)undo_cb, 0, FL_MENU_DIVIDER },
            { "Cu&t",        FL_COMMAND + 'x', (Fl_Callback *)cut_cb },
            { "&Copy",       FL_COMMAND + 'c', (Fl_Callback *)copy_cb },
            { "&Paste",      FL_COMMAND + 'v', (Fl_Callback *)paste_cb },
            { "&Delete",     0, (Fl_Callback *)delete_cb },
            { 0 },

        { "&Search", 0, 0, 0, FL_SUBMENU },                           
            { "&Find...",       FL_COMMAND + 'f', (Fl_Callback *)find_cb },
            { "F&ind Again",    FL_COMMAND + 'g', find2_cb },
            { "&Replace...",    FL_COMMAND + 'r', replace_cb },
            { "Re&place Again", FL_COMMAND + 't', replace2_cb },
            { 0 },

        { 0 }
    };

    Fl_Menu_Bar *menubar = new Fl_Menu_Bar(0,0, 250, 40);
    menubar->copy(menuitems());

}



