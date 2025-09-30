#ifndef EDITOR_H
#define EDITOR_H

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

//Window class
class EditorWindow: public Fl_Double_Window
{
    public:
        EditorWindow(int w, int h, const char *title);

        Fl_Window *replace_win = nullptr; // Pointer to the replace window
        Fl_Input *find_input = nullptr;   // Pointer to the find input field
        Fl_Input *replace_input = nullptr; // Pointer to the replace input field

        Fl_Text_Editor *editor = nullptr; // Pointer to the text editor
        char search_text[256] = {0}; // Text to find
};

#endif