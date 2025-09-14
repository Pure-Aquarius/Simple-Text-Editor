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
    
    //Callback function to copy selected text to clipboard
    void copy_cb(Fl_Widget*, void *v)
    {
        Editor_Window *e = (Editor_Window *)v;
        Fl_text_Editor::kf_copy(0, e->text_editor);
    }
    
    //Callback function to paste text from clipboard
    void paste_cb(Fl_Widget*, void *v)
    {
        Editor_Window *e = (Editor_Window *)v;
        Fl_text_Editor::kf_paste(0, e->text_editor);
    }
    
    //Callback function to cut selected text
    void cut_cb(Fl_Widget*, void *v)
    {
        Editor_Window *e = (Editor_Window *)v;
        Fl_Text_Editor::kf_cut(0, e->text_editor);
    }
    
    //Callback function to delete selected text
    void delete_cb(Fl_Widget*, void *v)
    {
        textbuf->remove_selection();
    }
    
    //Callback function to take input of search string
    void find_cb(Fl_Widget* w, void *v)
    {
        Editor_Window *e = (Editor_window *)v;
        const char *val;
        """This callback function asks for a search string using the 
        fl_input() convenience function and then calls the find2_cb() 
        function to find the string"""

        val = fl_input("Search string:"/*Prompt shown to user*/, e->search_text_to_replace);    //input dialog box to take input from user for search string
        if (val !=NULL)
        {
            strcpy(e->search_text_to_replace, val); //copy input string to editor window's search buffer
            find2_cb(w, v);
        }
    }
    
    //Callback function to find next occurence of search string
    void find2_cb(Fl_Widget *w, void *v)
    {
        Editor_Window *e = (Editor_Window*)v;
        if(e->search_text_to_replace[0]=='\0')  //if search string is empty, ask user for search string again and return
        {
            find_cb(w, v);
            return;
        }
        //else - search string is not NULL
        int pos = e->text_editor->insert_position(); //get current cursor position in text editor
        int found_pos = textbuf->search_forward(pos/*start pos to search from*/, e->search_text_to_replace/*text to search for*/, &pos /*pos at which result is stored*/); //search for the string forward from current cursor position
        if(found_pos>=0)    //valid position found
        {
            textbuf->select(pos, pos+strlen(e->search_text_to_replace)); //highlight the found string in text editor
            e->text_editor->insert_position(pos+strlen(e->search_text_to_replace)); //move cursor to end of found string
            e->text_editor->show_insert_position(); //ensure cursor is visible
        }
        else    //string not found
        {
            fl_alert("String '%s' not found", e->search_text_to_replace); //show alert dialog box to user
        }
    }
    
    //Callback function to handle "New File" menu item
    void new_cb(Fl_Widget*, void *)
    {
        if(!check_save())   //if current file has not been saved
            return; 
        //if current file is saved
        filename[0] = '\0'; //reset filename to empty
        textbuf->select(0, textbuf->length()); //select all text in buffer
        textbuf->remove_selection(); //remove all text from buffer
        changed = 0;    //reset changed flag
        textbuf->call_modify_callbacks(); //call modify callbacks to update window title
    }
    
    //Callback function to handle "Open File" menu item
    void open_cb(Fl_Widget*, void*)
    {
        if(!check_save())   //if current file has not been saved
            return;
        //if current file is saved
        char *new_file = fl_file_chooser("Open File", "*"/*file-filter param*/, filename); //show file chooser dialog to user to select file to open
                //new_file will store the full path of the selected file, or NULL if user cancelled the dialog box
        if(new_file != NULL) //if user selected a non-empty file
            load_file(new_file, -1); //load the selected file into text buffer
    }

    //Callback function to handle the Quit menu item
    void quit_cb(Fl_Widget*, void*)
    {
        if(changed && !check_save())    //if current file has not been saved
            return;
        //if current file is saved
        exit(0);    //exit the application    
    }

    //Callback function to handle "Save File" menu item
    void save_cb(Fl_Widget*, void*)
    {
        if(changed)  //if current file has been changed since last save
        {
            if(filename[0] == '\0') //if no name has been assigned to current file
            {
                save_as_cb(); //call save-as callback to ask user for file name
                return;
            }
            //if current file has a valid name
            save_file(filename); //save current file to the assigned filename
        }
        //if current file has not been changed since last save, do nothing
    }

    //Callback function to handle "Save File As" menu item
    void save_as_cb(Fl_widget*, void*)
    {
        char *new_file = fl_file_chooser("Save file as", "*"/*file-filter param*/, filename); //show file chooser dialog to user to name the current file to save as
                //new_file will store the full path of the newly-named file, or NULL if user cancelled the dialog box
        if(new_file != NULL)    //if user provided a non-empty file name
            save_file(new_file); //save current file to the newly-named file
    }

    //Callback function to handle "Replace" menu item
    void replace_cb(Fl_widget*, void *v)//only shows the replace dialog box
    {
        Editor_Window *e = (Editor_Window *)v;
        e->replace_window->show(); //show the replace dialogbox
    }

    //Callback function to replace next immediate [not all] occurence of search string with replace string
    void replace2_cb(Fl_Widget*, void *v)
    {
        Editor_Window *e = (Editor_Window *)v;
        const char *find = e->search_text_to_replace->value(); //get the search string from the input field
        const char *replace = e->replace_with->value(); //get the replace string from the input field

        if(find[0] == '\0') //if search string is empty, ask user for search string again and return
        {
            find_cb(0, v);
            return;
        }

        int pos = e->text_editor->insert_position(); //get current cursor position in text editor
        int found_pos = textbuf->search_forward(pos/*start pos to search from*/, find/*text to search for*/, &pos /*pos at which result is stored*/); //search for the string forward from current cursor position
        
        if(found_pos >= 0)   //valid position found
        {
            //replace the found string with the replace string
            textbuf->select(pos, pos+strlen(find)); //highlight the found string in text editor
            textbuf->remove_selection(); //remove the found string
            textbuf->insert(pos, replace); //insert the replace string at the position of the found string
            e->text_editor->insert_position(pos+strlen(replace)); //move cursor to end of inserted replace string
            e->text_editor->show_insert_position(); //ensure the cursor is visible after replacement
            changed = 1;    //set changed flag to indicate that the text has been modified
            textbuf->call_modify_callbacks(); //call modify callbacks to update window title
        }
        else    //string not found
            fl_alert("String '%s' not found", find); //show alert dialog box to user
    }

    //Callback function to replace all occurences of search string with replace string
    void replace_all_cb(Fl_Widget*, void* v)
    {
        Editor_Window *e = (Editor_Window *)v;
        const char *find = e->search_text_to_replace->value(); get the search string from the input field
        const char *replace = e->replace_with->value(); //get the replace string from the input field

        if(find[0] == '\0') //if search string is empty, ask user for search string again and return
        {
            fl_alert("Search string is empty");
            find_cb(0, v);
            return;
        }
        
        //if search string is not empty
        int times_replaces = 0; //counter to track number of replacements made
        while(true)
        {
            int pos = e->text_editor->insert_position(); //get current cursor position in text editor
            int found_pos = textbuf->search_forward(pos, find, &pos); //search for the string forward from current cursor position

            if(found_pos >=0) //valid position found
            {
                textbuf->select(pos, pos+strlen(find)); //highlight the found string in text editor
                textbuf->remove_selection(); //remove the found string
                textbuf->insert(pos, replace); //insert the replace string at the position of the found string
                e->text_editor->insert_position(pos+strlen(replace)); //move cursor to end of inserted replace string
                e->text_editor->show_insert_position(); //ensure the cursor is visible after replacement
                changed = 1;    //set changed flag to indicate that the text has been modified
                textbuf->call_modify_callbacks(); //call modify callbacks to update window title
                times++; //increment replacement counter after each successful replacement
            }
            else    //string not found
                break;  //exit the loop     
        }
        if(times!=0)    //if at least one replacement made
            fl_message("replaced %d occurences of %s with %s", times, find, replace); //show message dialog to user indicating number of replacements made
        else    //if no replacements made
            fl_alert("String %s not found", find); //show alert dialog box to user
    }

    //Callback function to cancel the replace operation and hide the replace dialog box
    void cancel_replace_cb(Fl_Widget*, void *v)
    {
        Editor_Window *e = (Editor_Window *)v;
        e->replace_window->hide(); //hide the replace dialogbox     
    }
    
    //Defining support functions for file operations and UI updates

    //Funcction to check if current file has unsaved changes and prompt user to save if needed-->returns 1 is file saved or no changes, 0 if user cancels
    int check_save()
    {
        if(!changed)//if current file has not been changed since last save
            return 1;   //no need to save, return true
        
        //if file has been changed since last save
        int r = fl_choice("File has not been saved. Do you want to save now? 
                            \n 1. Save \n 2. Don't Save[Discard] \n 3. Cancel");

        if(r==1) //user chose to save
        {
            save_cb(); //call save callback to save current file
            return !changed; //return true if save was successful (changed flag reset), else false
        }

        if(r==2) //user chose not to save
            return 1;   //return true, proceed without saving
        
        return 0;   //user chose to cancel, return false
    }
    
    //Function to load the file into the text buffer
    int loading = 0; //flag to indicate if a file is being loaded (to prevent triggering change callbacks)
    void load_file(char *newfile, int ipos/*insert position*/)
    {
        loading = 1; //set loading flag to true to prevent change callbacks during file load
        int insert = (ipos != -1); //determine if we are inserting or replacing the buffer content
        changed= insert; //set changed flag if inserting, else reset if replacing

        int r;
        if (!insert)    //if not inserting, i.e replacing buffer content
        {
            strcpy(filename, ""); //reset filename to empty
            r = textbuf->loadfile(newfile); //load the file content into the text buffer
        }
        else    //if inserting
            r = textbuf->insertfile(newfile, ipos); //insert the file content at the specified position in the text buffer       
        
            
        if(r)   //if file load/insert failed
            fl_alert("Error loading file '%s': %s", newfile, strerror(errno)); //show alert dialog to user with error message
        else    //if file load/insert successful
        {
            if(!insert) //if replacing buffer content
                strcpy(filename, newfile); //update current filename to the loaded file's name
            set_title((Editor_Window *)this); //update window title to reflect new filename and modification status
            textbuf->call_modify_callbacks(); //call modify callbacks to update window title    
        }
        loading = 0; //reset loading flag after file load/insert operation
    }
    
    //Function to save the text buffer content to the specified file
    void save_file(char *newfile)
    {
        int r = textbuf->savefile(newfile);  //save the tet buffer content to the specified file
        if(r) //if file save failed
            fl_alert("Error saving %s: %s", newfile, strerror(errno)); //show alert dialog to user with error message
        else    //if file save successful
            strcpy(filename, newfile); //update current filename to the saved file's name
        changed = 0;   //reset changed flag after successful save
        textbuf->call_modify_callbacks(); //call modify callbacks to update window title    
    }


    return 0;        
}



C