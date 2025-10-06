#ifndef EDITOR_H
#define EDITOR_H

// --- REQUIRED HEADERS FOR DECLARATIONS ---
// We include the headers needed to define our class and function prototypes.
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Widget.H> // Needed for custom widget for line numbers

//forward declaration
class Fl_Text_Editor;

// --- CLASS DEFINITIONS ---

//class to create a widget that displays line numbers on the left side of the text editor and scrolls in sync with it
class LineNumberWidget: public Fl_Widget        
{
    private:
        Fl_Text_Editor *editor;     //pointer to the associated text editor
    public:
        LineNumberWidget(int x, int y, int w, int h, Fl_Text_Editor *e);
        void draw() override; //override the draw method to render line numbers
};

// This is the "what" - it declares what our EditorWindow is.
class EditorWindow: public Fl_Double_Window
{
    public:
        EditorWindow(int w, int h, const char *title);
        ~EditorWindow(); // Destructor to clean up our buffer

        Fl_Window *replace_win = nullptr;       // Pointer to the replace window
        Fl_Input *find_input = nullptr;         // Pointer to the find input field
        Fl_Input *replace_input = nullptr;      // Pointer to the replace input field
        Fl_Text_Editor *editor = nullptr;       // Pointer to the text editor
        char search_text[256] = {0};            // Text to find

        LineNumberWidget *lineNo = nullptr;     //Pointer to the line number widget

        // public memeber variables for the state of the editor, unique to every text editor window
        int changed_flag = 0;                   // flag to indicate if changes have been made to a file
        char filename[256] = "";                // name of the current file
        Fl_Text_Buffer *textbuf = nullptr;      // text buffer for the text editor
};

//Function prototypes
void changed_cb(int pos, int nInserted, int nDeleted, int nRestyled, const char *deletedText, void *v);
void copy_cb(Fl_Widget*, void* v);
void paste_cb(Fl_Widget*, void* v);
void cut_cb(Fl_Widget*, void* v);
void delete_cb(Fl_Widget*, void *v);
void find_cb(Fl_Widget*, void* v);
void find2_cb(Fl_Widget* w, void* v);
void new_cb(Fl_Widget*, void* v);
void open_cb(Fl_Widget*, void* v);
void quit_cb(Fl_Widget*, void* v); // CORRECTED: Added void* for consistency
void save_cb(Fl_Widget*, void* v);
void save_as_cb(Fl_Widget*, void* v);
void replace_cb(Fl_Widget*, void* v);
void replace2_cb(Fl_Widget*, void* v);
void replace_all_cb(Fl_Widget*, void* v);
void cancel_replace_cb(Fl_Widget*, void* v);
int check_save(EditorWindow *win); // CORRECTED: Pass window pointer to save
void load_file(EditorWindow* win, char *newfile, int ipos = -1);
void save_file(EditorWindow* win, char *newfile);
void set_title(EditorWindow *win);
void insert_cb(Fl_Widget*, void*);
void view_cb(Fl_Widget*, void* v);
void close_cb(Fl_Widget*, void*);
void undo_cb(Fl_Widget*, void *v);

#endif