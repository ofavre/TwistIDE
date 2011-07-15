#include <iostream>
#include <sstream>
#include <ctime>

#include <gtkmm.h>
#include <gtkmm/socket.h>

class GVim : public Gtk::Window
{
    public:
        GVim()
        {
            set_title("TwistIDE - GVim embedding test");
            mSocket.signal_realize().connect(sigc::mem_fun(*this, &GVim::on_mSocket_realize));
            mSocket.signal_grab_focus().connect(sigc::mem_fun(*this, &GVim::on_mSocket_grab_focus));
            mSocket.signal_plug_removed().connect(sigc::mem_fun(*this, &GVim::on_mSocket_plug_removed));
            mSocket.signal_plug_added().connect(sigc::mem_fun(*this, &GVim::on_mSocket_plug_added));

            mBox.pack_start(mSocket, true, true);

            add(mBox);
            set_default_size(640, 480);
            set_visible(true);
            show_all();
        }

        virtual ~GVim()
        {
        }

        void makeGVim()
        {
            Gdk::NativeWindow xid = mSocket.get_id();
            std::stringstream cmd;
            // Don't use ' &' or vim can be still initializing while the rest of this program will run (vim server not started yet)
            cmd << "gvim --servername TwistIDE --socketid " << xid;
            std::cout << cmd.str() << std::endl;

            int rtn = system(cmd.str().c_str());
            if (rtn < 0 || !WIFEXITED(rtn) || WEXITSTATUS(rtn) != 0)
            {
                std::cerr << "Cannot run gvim!" << std::endl;
            }
        }

        void on_mSocket_realize()
        {
            makeGVim();
        }

        void on_mSocket_plug_added()
        {
            // GVim started!
            mSocket.child_focus(Gtk::DIR_TAB_FORWARD);
        }

        bool on_mSocket_plug_removed()
        {
            // GVim exitted, quitting
            Gtk::Main::instance()->quit();
            return true;
        }

        void on_mSocket_grab_focus()
        {
            int rtn = system("gvim --servername TwistIDE --remote-send 'iWelcome to TwistIDE!'");
            if (rtn < 0 || !WIFEXITED(rtn) || WEXITSTATUS(rtn) != 0) {
                std::cerr << "Could not send commands to gvim!" << std::endl;
            }
        }

    protected:
        Gtk::VBox       mBox;
        Gtk::Socket     mSocket;
};

int main(int argc, char** argv)
{
    Gtk::Main kit(argc, argv);

    GVim w;

    kit.run(w);

    return 0;
}
