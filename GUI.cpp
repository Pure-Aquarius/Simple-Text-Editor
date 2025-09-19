#include <iostream>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/fl_ask.H>
using namespace std;

//Forward declarations
class EditorWindow;

//Global variables              NOTE: For a multi-window app, it's better to avoid globals.
                                   // But for a single-window editor, this is acceptable.

int changed_flag = 0;            //flag to indicate if changes have been made to a file
char filename[256] = "";         //name of the current file
Fl_Text_Buffer *textbuf = nullptr; //text buffer for the text editor

//Function prototypes
void changed_cb(int pos, int nInserted, int nDeleted, int nRestyled, const char *deletedText, void *v);
void copy_cb(Fl_Widget*, void*);
void paste_cb(Fl_Widget*, void*);
void cut_cb(Fl_Widget*, void*);
void delete_cb(Fl_Widget*, void*);
void find_cb(Fl_Widget*, void*);
void find2_cb(Fl_Widget*, void*);
void new_cb(Fl_Widget*, void*);
void open_cb(Fl_Widget*, void*);
void quit_cb(Fl_Widget*, void* v); // CORRECTED: Added void* for consistency
void save_cb(Fl_Widget*, void* v);
void save_as_cb(Fl_Widget*, void* v);
void replace_cb(Fl_Widget*, void*);
void replace2_cb(Fl_Widget*, void*);
void replace_all_cb(Fl_Widget*, void*);
void cancel_replace_cb(Fl_Widget*, void*);
int check_save(void *v); // CORRECTED: Pass window pointer to save
void load_file(EditorWindow* win, char *newfile, int ipos = -1);
void save_file(EditorWindow* win, char *newfile);
void set_title(EditorWindow *w);
void insert_cb(Fl_Widget*, void*);
void view_cb(Fl_Widget*, void*);
void close_cb(Fl_Widget*, void*);
void undo_cb(Fl_Widget*, void*);

//A simple flag to prevent recursion in changed_cb()
int loading = 0;

//Window class
class EditorWindow: public Fl_Double_Window
{
    public:
        EditorWindow(int w, int h, const char *title);
        ~EditorWindow();

        Fl_Window *replace_win = nullptr; // Pointer to the replace window
        Fl_Input *find_input = nullptr;   // Pointer to the find input field
        Fl_Input *replace_input = nullptr; // Pointer to the replace input field

        Fl_Text_Editor *editor = nullptr; // Pointer to the text editor
        char search_text[256] = {0}; // Text to find
};

// ========== SUPPORT FUNCTIONS ==========

int check_save(void *v)         //checks if the file has been saved
{
    if(!changed_flag) return 1; //no changes, so OK to proceed

    //else, changes have been made that need to be saved or discarded
    int choice = fl_choice("The current file has not been save.\n Would you like to save it? 1. Save 2. Discard Changes 3. Cancel",
                           "Save", "Discard Changes", "Cancel");
    if(choice == 1)
    {
        save_cb(0, v); //save the file
        return !changed_flag; //if save was successful, changed_flag will be cleared
    }
    if(choice == 2)
        return 1; //Nothing is saved
    //else choice == 3
    return 0; //cancel operation    
}

void load_file(EditorWindow *win, char *newfile, int ipos)          //load file into the text buffer
{
    if(!newfile || !win || !textbuf)    return;           
    
    loading = 1; //prevent changed_cb() from being called
    int insert = (ipos != -1); //flag to indicate if we are inserting or replacing: true(1) --> insert, false(0) --> replace

    if(!insert) //if replacing the entire buffer
        strcpy(filename, ""); //clear the filename first
    
    bool success = false; //flag to indicate if file was loaded successfully
    if(insert)
        success = (textbuf->insertfile(newfile, ipos) == 0); //insert file at position ipos
    else
        success = (textbuf->loadfile(newfile) == 0); //replace entire buffer with file contents
    
    if (success)        //if file loaded successfully
    {
        if(!insert) //if replacing the entire buffer
        {
            strcpy(filename, newfile); //set the current filename
            changed_flag = insert; //clear the changed flag
        }
    }
    else
        fl_alert("Error loading file '%s': %s", newfile, strerror(errno));
    
    loading =0; //re-enable changed_cb()
    textbuf->call_modify_callbacks(); //manually call the modified callback to update the display
    win->editor->show_insert_position(); //show the insertion point
}

void save_file(EditorWindow *win, char *newfile)        //save the buffer contenc=ts to a file
{
    if(!win || !textbuf || !newfile) return;

    if(textbuf->savefile(newfile) == 0) //if file saved successfully
    {
        strcpy(filename, newfile);   //set the current filename
        changed_flag = 0;            //clear the changed flag
    }
    else
        fl_alert("Error saving file '%s': %s", newfile, strerror(errno));
    
    textbuf->call_modify_callbacks(); //manually call the modified callback to update the display
    win->editor->show_insert_position(); //show the insertion point
}

void set_title(EditorWindow *w)         //set the window title based on current filename
{
    if(!w) return;

    char title[300]; //buffer for the title
    if(filename[0] == '\0') //if no filename given
        strcpy(title, "Untitled");  //set title to "Untitled"
    else
    {
        char *slash = strrchr(filename, '/'); //find last '/' in the filename, after which is the actual name of the file
        if(slash != nullptr)
            strcpy(title, slash+1); //copy the actual filename
        else
            strcpy(title, filename); //no '/' found, so copy the entire filename
    }    

    if(changed_flag)    //if changes have been made to this file
        strcat(title, " (modified)");     //append " (modified)" to the title
    
    w->copy_label(title); //set the window title    

}

// ========== CALLBACK IMPLEMENTATIONS ==========

void changed_cb(int, int nInserted, int nDeleted, int, const char*, void* v)          //Called whenever changes are made to the buffer
{
    if((nInserted || nDeleted) && !loading) //if something was actually inserted or deleted, and we are not loading a file
        changed_flag = 1; //set the changed flag
    EditorWindow *win = (EditorWindow*)v;
    set_title(win); //update the window title after every change
}

void copy_cb(Fl_Widget*, void* v)      //copy the selected text to the clipboard
{
    EditorWindow *win = (EditorWindow *)v;
    if(win && win->editor)              //if window and editor exist then
        Fl_Text_Editor::kf_copy(0, win->editor);
}

void paste_cb(Fl_Widget*, void *v)    //paste the clipboard text to the editor
{
    EditorWindow *win = (EditorWindow *)v;
    if(win && win->editor)              //if window and editor exist then
        Fl_Text_Editor::kf_page_down(0, win->editor);
}

void cut_cb(Fl_Widget*, void *v)        //cut the selected text to the clipboard
{
    EditorWindow *win = (EditorWindow *)v;
    if(win && win->editor)        //if window and editor exist then
        Fl_Text_Editor::kf_cut(0, win->editor);
}

void delete_cb(Fl_Widget*, void *v)
{
    if(textbuf)
        textbuf->remove_selection(); //delete the selected text
}

void undo_cb(Fl_Widget*, void *v)
{
    if(textbuf)
        textbuf->undo(); //undo the last action
}

void find_cb(Fl_Widget *w, void *v)       //opens the find dialog box & gets the search text from the user
{
    EditorWindow *win = (EditorWindow *)v;
    if(!win)    return;     //return if window doesn't exist

    const char *val = fl_input("Enter the text to find:", win->search_text);
    if(val)
    {
        strcpy(win->search_text, val);      //copy the text to find into the window's search_text variable
        if(win->find_input)
            win->find_input->value(val);    //set the find_input field in the replace window, if it exists
    }
}

void find2_cb(Fl_Widget*, void *v)
{
    EditorWindow *win = (EditorWindow *)v;
    if(!win || !win->editor || !textbuf)
        return; //return if window, editor or text buffer doesn't exist

    //else, find the next occurrence of the search text
    if(win->search_text[0] == '\0') //if no search text specified
    {
        find_cb(win, v); //open the find dialog to get the search text
        return;
    }

    int start_pos = win->editor->insert_position(); //get the current insert position of the cursor
    int found_pos = textbuf->search_forward(start_pos, win->search_text, &start_pos); //find the next occurrence of the search text

    if(found_pos >= 0)      //if a valid position of the find_input is found, then look for its start-position
    {
        textbuf->select(start_pos, start_pos+strlen(win->find_input->value()));   //select the found text
        win->editor->insert_position(start_pos+strlen(win->find_input->value()));       //insert cursor at this found text's end to insert new value
        win->editor->show_insert_position();    //show the insertion point
    }
}

void new_cb(Fl_Widget*, void*)          //callback to handle opening a new file - by emptying the text-buffer for new file
{
    if(!check_save(nullptr))
        return;             //if the currently open file has unsaved changes, then return 
    //else currently open file is saved
    if(textbuf)
    {
        textbuf->select(0, textbuf->length());      //select entire buffer
        textbuf->remove_selection();                //remove the selected text
    }  
    changed_flag = 0;        //set the changed flag to 0 - as buffer has been cleared for a new file to be opened
    if(textbuf)
        textbuf->call_modify_callbacks();           //manually call the modified callback to update the display
}   

