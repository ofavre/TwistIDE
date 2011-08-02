#include <iostream>



#include "GVim.hpp"



int main(int argc, char** argv)
{
    Glib::thread_init();
    gdk_threads_init();
    Gtk::Main kit(argc, argv);

    GVim w;
    // Pre Gtk mainloop initialization
    // It's still time to quit with no window
    if (!w.startServer())
    {
        std::cout << "Unable to start server, aborting!" << std::endl;
        return 1;
    }

    gdk_threads_enter();
    kit.run(w);
    gdk_threads_leave();

    return 0;
}
