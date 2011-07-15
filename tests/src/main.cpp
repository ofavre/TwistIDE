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
            mSocket.signal_realize().connect(sigc::mem_fun(*this, &GVim::on_mSocket_realize));
            mSocket.signal_plug_removed().connect(sigc::mem_fun(*this, &GVim::on_mSocket_plug_removed));
            mSocket.signal_plug_added().connect(sigc::mem_fun(*this, &GVim::on_mSocket_plug_added));

            mBox.pack_start(mSocket, true, true);
            mSocket.set_size_request(100,100);

            add(mBox);
            set_default_size(960, 512);
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
            cmd << "gvim --socketid " << xid << " &";
            std::cout << cmd.str() << std::endl;

            int rtn = system(cmd.str().c_str());
            if (rtn != 0)
            {
                perror("Cannot run gvim!");
            }
        }

        void on_mSocket_realize()
        {
            makeGVim();
        }

        void on_mSocket_plug_added()
        {
            std::cout << "GVim started!" << std::endl;
            mSocket.get_plug_window()->focus(0); // TODO FIX
        }

        bool on_mSocket_plug_removed()
        {
            std::cout << "GVim exitted, quitting" << std::endl;
            Gtk::Main::instance()->quit();
            return true;
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
