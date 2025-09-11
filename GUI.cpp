#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>

int main() {
    class Editor_Window : public Fl_Double_Window 
    {
        public:
            Editor_Window(int w, int h, const char *title);
            ~Editor_Window();

            Fl_Window *replace_dlg;
            Fl_Input *replace_find;
            Fl_Input *replace_with;
            Fl_Button *replace_all;
            Fl_Return_Button *replace_next;
            Fl_Button *replace_cancel;

            Fl_Text_Editor *text_editor;
            char search [256];
    }
};


