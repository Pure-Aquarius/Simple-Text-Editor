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

    """Creating a Custom Window Class with a basic layout for Text Editing and Search/Replace Functionality"""
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


    """Creating a Menu Bar and Adding Menu Items"""
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

    """Editing the text in the Text Editor(BUFFER)"""
    w->editor = new Fl_Text_Editor(0, 30, 640, 30);
    w->editor->buffer(textbuf); //attach text buffer to editor to enable basic editing of text

    text_buf->add_modify_callback(changed_cb, 0); //set callback to monitor changes to text buffer
    textbuf->call_modify_callbacks(); //call modify callback to initialize changed flag

    w->editor->textfont(FL_COURIER); //set font to fixed width for better alignment
    w->editor->textsize(14); //set text size

    """Creating a search/replace dialog-box/window"""
    Fl_Window *replace_window = new Fl_Winodw(300, 105, "Replace");
    Fl_Input *search_text_to_replace = new Fl_Input(70, 10, 200, 25, "Find");
    Fl_Input *replace_with = new Fl_Input(70, 45, 200, 25, "Replace");
    Fl_Button *replace_all = new Fl_Button(10, 70, 90, 25, "Replace All");
    Fl_Return_Button *return_button =. new Fl_Return_Button(110, 70, 90, 25, "Replace Next");
    Fl_Button *cancel_replace - new Fl_Button(210, 70, 90, 25, "Cancel");

    """Defining all the necessary callback functions for the menuitems and buttons"""

    //Callback function that handles changes in the text buffer
    void changed_cb(int, int nInserted, int nDeleted, int, const char*, void* v)
    {
        """
         This Callback function is triggered when text buffer content is modified i.e when the text is edited
          
          @param ignored1     Unused parameter (required by FLTK callback signature)
          @param nInserted    Number of characters inserted in the buffer
          @param nDeleted     Number of characters deleted from the buffer
          @param ignored2     Unused parameter (required by FLTK callback signature)
          @param ignored3     Unused parameter (required by FLTK callback signature)
          @param v           Pointer to the Editor_Window instance (void* for FLTK compatibility)
          
           This callback function is triggered whenever the text buffer content changes.
           It performs the following tasks:
           - Tracks whether the document has unsaved changes
           - Updates the window title to reflect modification status
           - Updates cursor position when loading files
        
        The function uses a global 'loading' flag to differentiate between user edits
        and programmatic changes during file loading operations.
        """
        if ((nInserted || nDeleted) && !loading)
            changed = 1;
        Editor_Window *w = (Editor_Window *)v;  
        set_title(w);
        
        if(loading)
            w->editor->show_insert_position();
    }

    void copy_cb(Fl_Widget*, void *v)
    {
        Editor_Window *e = (Editor_Window *)v;
        Fl_text_Editor::kf_copy(0, e->editor);
    }

    void paste_cb(Fl_Widget*, void *v)
    {
        Editor_Window *e = (Editor_Window *)v;
        Fl_text_Editor::kf_paste(0, e->editor);
    }

    void cut_cb(Fl_Widget*, void *v)
    {
        Editor_Window *e = (Editor_Window *)v;
        Fl_Text_Editor::kf_cut(0, e->editor);
    }

    void delete_cb(Fl_Widget*, void *v)
    {
        textbuf->remove_selection();
    }

    void find_cb(Fl_Widget* w, void *v)
    {
        
    }

}



