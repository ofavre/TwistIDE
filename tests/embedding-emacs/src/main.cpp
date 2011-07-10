#include <iostream>
#include <sstream>

#include <gtkmm.h>
#include <gtkmm/socket.h>



class Emacs : public Gtk::Window
{
	public:
		Emacs()
		{
			add(mSocket);

			set_default_size(512, 960);
			set_visible(true);
		}

		~Emacs()
		{
		}

		void makeEmacs()
		{
			Gdk::NativeWindow xid = mSocket.get_id();
			std::stringstream cmd;
			cmd << "emacs --parent-id " << xid << " &";
			std::cout << cmd.str() << std::endl;
			system(cmd.str().c_str());
		}

	protected:
		Gtk::Socket mSocket;
};

int main(int argc, char** argv)
{
	Gtk::Main kit(argc, argv);

	Emacs e;
	e.makeEmacs();

	kit.run(e);

	return 0;
}
