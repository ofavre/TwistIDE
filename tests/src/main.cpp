#include <iostream>
#include <sstream>

#include <gtkmm.h>
#include <gtkmm/socket.h>

#include <QWidget>
#include <QX11EmbedWidget>
#include <KParts/Part>
#include <KApplication>
#include <kde_terminal_interface.h>
#include <KPluginFactory>
#include <KPluginLoader>

class Konsole : public Gtk::Window
{
	public:
		Konsole()
		{
			add(mSocket);

			set_default_size(960, 512);
			set_visible(true);
		}

		virtual ~Konsole()
		{
			if (mKonsolePart) delete mKonsolePart;
		}

		void makeKonsole()
		{
			Gdk::NativeWindow xid = mSocket.get_id();
			std::cout << "Parent-id: " << xid << std::endl;

			mSocketQ.embedInto(xid);

			KPluginFactory* factory = KPluginLoader("konsolepart").factory();
			if (!factory)
				factory = KPluginLoader("libkonsolepart").factory(); // deprecated name
			mKonsolePart = factory ? (factory->create<KParts::Part>(&mSocketQ)) : 0;

			if (mKonsolePart == NULL) {
				std::cerr << "Unable to create a Konsole::Part" << std::endl;
				exit(EXIT_FAILURE);
			}

			mSocketQ.show();
		}

	protected:
		Gtk::Socket     mSocket;
		QX11EmbedWidget mSocketQ;
		KParts::Part*   mKonsolePart;
};

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	Gtk::Main kit(argc, argv);

	Konsole k;
	k.makeKonsole();

	kit.run(k);

	return 0;
}
