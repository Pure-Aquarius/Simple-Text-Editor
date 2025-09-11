#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Double_Window.H>
using namespace std;

int changed = 0;        //flag to indicate if text has been changed since last save
char filename[256] = "";    //filename of current file
Fl_Text_Buffer *textbuf;    //buffer to hold text being edited

int main() {
    class Editor_Window : public Fl_Double_Window 
    {
        public:
            Editor_Window(int w, int h, const char *title);
            ~Editor_Window();

            Fl_Window *replace_dlg;     //actual winodow
            Fl_Input *replace_find;     //text input that specifies what to search for
            Fl_Input *replace_with;     //text input specifying what to replace with
            Fl_Button *replace_all;     //button to replace all occurences
            Fl_Return_Button *replace_next;     //button to replace next occurence
            Fl_Button *replace_cancel;      //cancel replace operation

            Fl_Text_Editor *text_editor;        //main are for text editing
            char search [256];
    };

    

}



