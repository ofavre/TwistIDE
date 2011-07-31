//#include <ctime>



#include "GVim.hpp"



int main(int argc, char** argv)
{
    Gtk::Main kit(argc, argv);

    GVim w;

    kit.run(w);

    return 0;
}
