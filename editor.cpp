// -- REQUIRED HEADERS FOR IMPLEMENTATION --
#include "editor.h" //CUSTOM HEADERS COME FIRST

// These headers are needed for the "how" (the implementation) of our functions.
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cerrno>

// FLTK headers for things used only in this .cpp file,
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
using namespace std;

// GLOBAL VARIABLES have been removed

//A simple flag to prevent recursion in changed_cb()
int loading = 0;

// ========== SUPPORT FUNCTIONS ==========

int check_save(EditorWindow *win)         //checks if the file has been saved
{
    if(!win->changed_flag) return 1; //no changes, so OK to proceed

    //else, changes have been made that need to be saved or discarded
    int choice = fl_choice("The current file has not been save.\n Would you like to save it? 1. Save 2. Discard Changes 3. Cancel",
                           "Save", "Discard Changes", "Cancel");
    if(choice == 0)
    {
        save_cb(nullptr, win); //save the file
        return !win->changed_flag; //if save was successful, changed_flag will be cleared
    }
    if(choice == 1)
        return 1; //Nothing is saved
    //else choice == 2
    return 0; //cancel operation    
}

void load_file(EditorWindow *win, char *newfile, int ipos)          //load file into the text buffer
{
    if(!newfile || !win || !win->textbuf)    return;           
    
    loading = 1; //prevent changed_cb() from being called
    int insert = (ipos != -1); //flag to indicate if we are inserting or replacing: true(1) --> insert, false(0) --> replace

    if(!insert) //if not inserting file, we are replacing the entire buffer for a new file
        strcpy(win->filename, ""); //clear the filename first
    
    bool success = false; //flag to indicate if file was loaded successfully
    if(insert)
        success = (win->textbuf->insertfile(newfile, ipos) == 0); //insert file at position ipos
    else
        success = (win->textbuf->loadfile(newfile) == 0); //replace entire buffer with file contents
    
    if (success)        //if file loaded successfully
    {
        if(!insert) //if replacing the entire buffer
        {
            strcpy(win->filename, newfile); //set the current filename
            win->changed_flag = 0; //clear the changed flag
        }
    }
    else
        fl_alert("Error loading file '%s': %s", newfile, strerror(errno));
    
    loading =0; //re-enable changed_cb()
    win->textbuf->call_modify_callbacks(); //manually call the modified callback to update the display
    win->editor->show_insert_position(); //show the insertion point
}

void save_file(EditorWindow *win, char *newfile)        //save the buffer contents to a file
{
    if(!win || !win->textbuf || !newfile) return;

    if(win->textbuf->savefile(newfile) == 0) //if file saved successfully
    {
        strcpy(win->filename, newfile);   //set the current filename
        win->changed_flag = 0;            //clear the changed flag
    }
    else
        fl_alert("Error saving file '%s': %s", newfile, strerror(errno));
    
    win->textbuf->call_modify_callbacks(); //manually call the modified callback to update the display
    win->editor->show_insert_position(); //show the insertion point
}

void set_title(EditorWindow *win)         //set the window title based on current filename
{
    if(!win) return;

    char title[300];                                //buffer for the title
    if(win->filename[0] == '\0')                         //if no filename given
        strcpy(title, "Untitled");                  //set title to "Untitled"
    else
    {   
        //use fl_filename_name to extract filename safely from a path
        const char *f_name = fl_filename_name(win->filename);       //find last '/' in the filename, after which is the actual name of the file
        strncpy(title, f_name, sizeof(title) - 1);      //copy the filename into title buffer
        title[sizeof(title) - 1] = '\0';             //no '/' found, so copy the entire filename
    }    

    if(win->changed_flag)                                //if changes have been made to this file
        strcat(title, " (modified)");               //append " (modified)" to the title
    
    win->label(title);                           //set the window title    

}

// ========== CALLBACK IMPLEMENTATIONS ==========

void changed_cb(int, int nInserted, int nDeleted, int, const char*, void* v)          //Called whenever changes are made to the buffer
{
    EditorWindow *win = (EditorWindow*)v;
    if((nInserted || nDeleted) && !loading)     //if something was actually inserted or deleted, and we are not loading a file
        win->changed_flag = 1;                       //set the changed flag
    set_title(win);                             //update the window title after every change
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
        Fl_Text_Editor::kf_paste(0, win->editor);
}

void cut_cb(Fl_Widget*, void *v)        //cut the selected text to the clipboard
{
    EditorWindow *win = (EditorWindow *)v;
    if(win && win->editor)        //if window and editor exist then
        Fl_Text_Editor::kf_cut(0, win->editor);
}

void delete_cb(Fl_Widget*, void *v)
{   
    EditorWindow *win = (EditorWindow *)v;
    if(win->textbuf)
        win->textbuf->remove_selection(); //delete the selected text
}

void undo_cb(Fl_Widget*, void *v)
{
    EditorWindow *win = (EditorWindow *)v;
    if(win->textbuf)
        win->textbuf->undo(); //undo the last action
}

void find_cb(Fl_Widget *w, void *v)       //opens the find dialog box & gets the search text from the user
{
    EditorWindow *win = (EditorWindow *)v;
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
    if(!win || !win->editor || !win->textbuf)
        return; //return if window, editor or text buffer doesn't exist

    //else, find the next occurrence of the search text
    if(win->search_text[0] == '\0') //if no search text given 
    {
        find_cb(win, v); //open the find dialog to get the search text
        return;
    }

    int start_pos = win->editor->insert_position(); //get the current insert position of the cursor
    int found_pos = win->textbuf->search_forward(start_pos, win->search_text, &start_pos); //find the next occurrence of the search text

    if(found_pos >= 0)      //if a valid position of the find_input is found, then look for its start-position
    {
        win->textbuf->select(start_pos, start_pos+strlen(win->search_text));   //select the found text
        win->editor->insert_position(start_pos+strlen(win->search_text));       //insert cursor at this found text's end to insert new value
        win->editor->show_insert_position();    //show the insertion point
    }
    else
        fl_alert("'%s' not found", win->search_text);
}

void new_cb(Fl_Widget*, void* v)          //callback to handle opening a new file - by emptying the text-buffer for new file
{
    EditorWindow *win = (EditorWindow *)v;
    if(!check_save(win))
        return;             //if the currently open file has unsaved changes, then return 

    //else currently open file is saved, so we can open a new file editor window without worries
    win->filename[0] = '\0';     //clear the filename
    if(win->textbuf)            //is the text buffer has content from previous files, then remove it
    {
        win->textbuf->select(0, win->textbuf->length());      //select entire buffer
        win->textbuf->remove_selection();                //remove the selected text
    }  
    win->changed_flag = 0;        //set the changed flag to 0 - as buffer has been cleared for a new file to be opened
    win->textbuf->call_modify_callbacks();           //manually call the modified callback to update the display
}   

void open_cb(Fl_Widget*, void *v)
{
    EditorWindow *win = (EditorWindow *)v;
    if(!check_save(win))
        return;                 //return if there are unsaved changes in currently-open file before opening another one
    //else. there are no changes, OK to proceed
    if(!win)    return;         //return if window doesn't exist

    char *newfile = fl_file_chooser("Open File", "*", win->filename);        //select a filename - user enters a filename to open
    if(newfile!= NULL)
        load_file(win, newfile, -1);
}

void view_cb(Fl_Widget*, void *v)
{
    EditorWindow *win = (EditorWindow *)v;
    if(!win)
        return;
    //else 
    //Create a new window to view another file at a slightly offset/different position than the original window
    EditorWindow *new_win = new EditorWindow(win->w(), win->h(), win->filename);
    new_win->position(win->x()+20, win->y()+20);
    new_win->show();
    set_title(new_win);  
}

void close_cb(Fl_Widget* w, void *v)
{
    EditorWindow *win = (EditorWindow *)v;
    if(!win)
        return;
    //else
    win->hide();    
}

void insert_cb(Fl_Widget *w, void *v)
{
    EditorWindow *win = (EditorWindow *)v;
    if(!win || !win->editor)
        return;

    char *newfile = fl_file_chooser("Insert file", "*", "");
    if(newfile != NULL)
        load_file(win, newfile, win->editor->insert_position());    
}

void quit_cb(Fl_Widget*, void *v)
{   
    EditorWindow *win = (EditorWindow *)v;
    if(check_save(win))    //if there are unsaved changes in current file
        exit(0);                        //exit with exit code 0
}

void save_as_cb(Fl_Widget*, void *v)
{
    EditorWindow *win = (EditorWindow *)v;
    char *newfile = fl_file_chooser("Save file as: ", "*", win->filename);       //select or give the name to store the file
    if(newfile != NULL)
        save_file(win, newfile);
}

void save_cb(Fl_Widget*, void *v)
{
    EditorWindow *win = (EditorWindow *)v;
    if(win->filename[0] == '\0')     //if no name has been given to file, then save as ..
        save_as_cb(nullptr, v);
    else
        save_file(win, win->filename);     //save file which has already been named and saved previously
}

void replace_cb(Fl_Widget*, void *v)
{
    EditorWindow *win = (EditorWindow *)v;
    if(!win || !win->editor)
        return;
    win->replace_win->show();    //show the replace window    
}

void replace2_cb(Fl_Widget*, void *v)
{
    EditorWindow *win = (EditorWindow *)v;
    if(!win || !win->editor || !win->find_input || !win->replace_input)
        return;
    //else, all necessary items available
    const char *find = win->find_input->value();
    const char *replace = win->replace_input->value();
    
    if(find[0]=='\0')       //if search string is empty
    {
        find_cb(0, v);
        return;
    }

    int start_pos = win->editor->insert_position(); //get the current cursor
    int found_pos = win->textbuf->search_forward(start_pos, find, &start_pos);      
    if(found_pos >= 0)
    {
        win->textbuf->select(start_pos, start_pos+strlen(find));
        win->textbuf->remove_selection();
        win->textbuf->insert(start_pos, replace);        //insert replace string at start index of old value
        win->editor->insert_position(start_pos+strlen(replace));        //insert the cursor into the editor
        win->editor->show_insert_position();
        win->changed_flag = 1;
    }
    else
        fl_alert("'%s' not found", find);
}

void replace_all_cb(Fl_Widget*, void *v)                //replaces all the occurences of a string
{
    EditorWindow *win = (EditorWindow *)v;
    const char *find = win->find_input->value();
    const char *replace = win->replace_input->value();

    if(find[0] == '\0')
    {
        find_cb(0,v);
        return;
    }

    int times_replaced = 0;
    int start_pos = 0;

    while(win->textbuf->search_forward(start_pos, find, &start_pos) >= 0)
    {
        win->textbuf->select(start_pos, start_pos+strlen(find));
        win->textbuf->remove_selection();
        win->textbuf->insert(start_pos, replace);
        times_replaced++;
        start_pos+=strlen(replace);             //update the position of cursor after replaceing the string
        win->changed_flag = 1;
    }

    if(times_replaced == 0)
        fl_alert("'%s' not found", find);
    else
        fl_alert("'%s' replaced %d times", find, times_replaced);
}

void cancel_replace_cb(Fl_Widget*, void *v)
{
    EditorWindow *win = (EditorWindow *)v;
    win->replace_win->hide();
}

// ========== LineNumber Widget Constructor & draw() method definition ===========

LineNumberWidget::LineNumberWidget(int x, int y, int w, int h, Fl_Text_Editor *e): Fl_Widget(x, y, w, h)
{
    this->editor = e; //associate the text editor with this line number widget
}

void LineNumberWidget::draw() {
    // Standard widget drawing setup
    draw_box();
    fl_font(FL_COURIER, editor->textsize());
    fl_color(FL_GRAY);

    // Cast the editor to its display component to access display functions
    Fl_Text_Display* display = (Fl_Text_Display*)editor;
    int max_lines = editor->buffer()->count_lines(0, editor->buffer()->length());
    
    // Get the y-coordinate of the top of the actual text display area
    int editor_top_y = display->y();

    // --- Find the top visible line ---
    // We do this by checking the y-position of each line until we find
    // the first one that is visible within the editor's display area.
    int first_line_to_draw = 0;
    for (int i = 0; i < max_lines; i++) {
        // line_start() gives the y-pixel-coordinate of the top of a given line.
        if (display->line_start(i) >= editor_top_y) {
            first_line_to_draw = i;
            break;
        }
    }

    // Get the y-coordinate of the bottom of our line number widget
    int widget_bottom_y = this->y() + this->h();

    // --- Loop and draw the visible line numbers ---
    for (int line_num = first_line_to_draw; line_num <= max_lines; ++line_num) {
        int y_pos = display->line_start(line_num);

        // If the next line to draw is below our widget, we can stop.
        if (y_pos > widget_bottom_y) {
            break;
        }

        // Only draw if the line is actually inside our widget's visible area.
        if (y_pos >= this->y()) {
            char line_num_str[16];
            // Format the number (e.g., " 123 ") and draw it.
            snprintf(line_num_str, sizeof(line_num_str), "%4d ", line_num + 1);
            fl_draw(line_num_str, this->x(), y_pos);
        }
    }
}

// ========== EDITOR WINDOW CONSTRUCTOR & DESTRUCTOR ==========

EditorWindow::EditorWindow(int w, int h, const char *title): Fl_Double_Window(w, h, title) {
    begin(); // Start adding to this window
    
    Fl_Menu_Bar *menubar = new Fl_Menu_Bar(0, 0, w, 25);
    static Fl_Menu_Item menuitems[] = {
      { "&File",              0, 0, 0, FL_SUBMENU },
        { "&New File",        FL_COMMAND + 'n', (Fl_Callback *)new_cb, this },
        { "&Open File...",    FL_COMMAND + 'o', (Fl_Callback *)open_cb, this },
        { "&Insert File...",  FL_COMMAND + 'i', (Fl_Callback *)insert_cb, this, FL_MENU_DIVIDER },
        { "&Save File",       FL_COMMAND + 's', (Fl_Callback *)save_cb, this },
        { "Save File &As...", FL_COMMAND + FL_SHIFT + 's', (Fl_Callback *)save_as_cb, this, FL_MENU_DIVIDER },
        { "New &View",        0, (Fl_Callback *)view_cb, this },
        { "&Close View",      FL_COMMAND + 'w', (Fl_Callback *)close_cb, this, FL_MENU_DIVIDER },
        { "E&xit",            FL_COMMAND + 'q', (Fl_Callback *)quit_cb, this },
        { 0 },

      { "&Edit",              0, 0, 0, FL_SUBMENU },
        { "&Undo",            FL_COMMAND + 'z', (Fl_Callback *)undo_cb, this, FL_MENU_DIVIDER },
        { "Cu&t",             FL_COMMAND + 'x', (Fl_Callback *)cut_cb, this },
        { "&Copy",            FL_COMMAND + 'c', (Fl_Callback *)copy_cb, this },
        { "&Paste",           FL_COMMAND + 'v', (Fl_Callback *)paste_cb, this },
        { "&Delete",          0, (Fl_Callback *)delete_cb, this },
        { 0 },

      { "&Search",            0, 0, 0, FL_SUBMENU },
        { "&Find...",         FL_COMMAND + 'f', (Fl_Callback *)find_cb, this },
        { "F&ind Again",      FL_COMMAND + 'g', (Fl_Callback *)find2_cb, this },
        { "&Replace...",      FL_COMMAND + 'r', (Fl_Callback *)replace_cb, this },
        { "Re&place Again",   FL_COMMAND + 't', (Fl_Callback *)replace2_cb, this },
        { 0 },
      
      { 0 }
    };

    menubar->menu(menuitems);

    //Get the dimensions of the window excluding the menubar
    const int menu_bar_ht = menubar->h();
    const int lineNoWidg_width = 50; //width of the line number widget

    //create a text editor
    editor = new Fl_Text_Editor(lineNoWidg_width, menu_bar_ht, w-lineNoWidg_width, h-menu_bar_ht);
    textbuf = new Fl_Text_Buffer();
    editor->buffer(textbuf);        //connect the editor to its text editor
    editor->textfont(FL_COURIER);
    editor->textsize(15);

    //create a line number widget
    LineNumberWidget *LineNoWidget = new LineNumberWidget(0, menu_bar_ht, lineNoWidg_width, h - menu_bar_ht, this->editor);
    
    // STEP (CRITICAL): Add a callback to the editor's buffer.
    // This tells the line_numbers widget to redraw() whenever the text changes or scrolls.
    textbuf->add_modify_callback([](int, int, int, int, const char*, void* v) {
        ((LineNumberWidget*)v)->redraw();
    }, LineNoWidget);

    resizable(editor); //make the text editor resizable with the window

    //Create a dialog box for the replace functionality
    replace_win = new Fl_Window(300, 120, "Replace");
    find_input = new Fl_Input(80, 10, 200, 25, "Find:");
    replace_input = new Fl_Input(80, 40, 200, 25, "Replace with:");
    Fl_Button *replace_all_btn = new Fl_Button(10, 80, 90, 25, "Replace");
    Fl_Button *replace_nxt_btn = new Fl_Button(110, 80, 90, 25, "Replace Next");
    Fl_Button *cancel_replace_btn = new Fl_Button(210, 80, 90, 25, "Cancel");

    replace_win->end();
    replace_win->hide();

    //Set callbacks - decides - when button clicked, what happens
    replace_all_btn->callback((Fl_Callback *)replace_all_cb, this);
    replace_nxt_btn->callback((Fl_Callback *)replace2_cb, this);
    cancel_replace_btn->callback((Fl_Callback *)cancel_replace_cb, this);

    end();
}

EditorWindow::~EditorWindow()
{
    delete textbuf;         //delete the text buffer
}
// ========== MAIN FUNCTION ==========

int main(int argc, char **argv)
{
    EditorWindow *window = new EditorWindow(800, 600, "Text Editor");
    window->show(argc, argv);
    window->textbuf->add_modify_callback(changed_cb, window);     //changed_cb is triggered/called whenever the text-buffer is modified
    set_title(window);

    if(argc > 1)        //if more than 1 cmd-line argument is given
        load_file(window, argv[1], -1);
    
    return Fl::run();
}
