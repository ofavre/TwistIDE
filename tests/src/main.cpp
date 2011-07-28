#include <string>
#include <iostream>
#include <sstream>
#include <ctime>
#include <exception>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <pthread.h>
#include <sigc++/sigc++.h>

#include <gtkmm.h>
#include <gtkmm/socket.h>



class VimSocketInterfaceCommunicator
{
    public:
        VimSocketInterfaceCommunicator(int socket, sockaddr_in addr)
        :   mSocket(socket)
        ,   mSckaddr(addr)
        {
            std::cout << "Vim port: " << mSckaddr.sin_port << std::endl;
        }

        ~VimSocketInterfaceCommunicator()
        {
            close(); // safe to call twice
        }

        void readloop_async()
        {
            pthread_t thread;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            if (pthread_create(&thread, &attr, &VimSocketInterfaceCommunicator::readloop, reinterpret_cast<void*>(this)) != 0)
            {
                perror("Cannot start client socket reading thread");
                throw std::exception();
            }
            std::cout << "Client socket reading thread started" << std::endl;
            pthread_attr_destroy(&attr);
        }

        static void* readloop(void* pthat)
        {
            reinterpret_cast<VimSocketInterfaceCommunicator*>(pthat)->readloop();
            std::cout << "Client socket reading thread finished" << std::endl;
            return NULL;
        }

        void readloop()
        {
            std::cout << "Inside readloop" << std::endl;

            char buffer[1051];
            bzero(buffer, 1051);
            while (true)
            {
                int n = read(mSocket, buffer, 1050);
                if (n < 0)
                {
                    perror("ERROR reading from socket");
                    break;
                }
                else if (n == 0)
                {
                    std::cout << "Nothing more to read, exitting." << std::endl;
                    if (mSocket != -1)
                        ::close(mSocket);
                    mSocket = -1;
                    break;
                }
                buffer[n] = '\0';
                printf("Here is the message (%d): %s\n", n, buffer);
                //TODO: Analyze answers and events
            }

            std::cout << "Exitting readloop" << std::endl;
            close(); // safe to call twice
            return;
        }

        void close()
        {
            if (mSocket != -1)
            {
                ::close(mSocket);
                mSocket = -1;
            }
        }

    private:
        int         mSocket;
        sockaddr_in mSckaddr;
        //TODO: Add signals to respond to events
};



class VimSocketInterfaceServer
{
    public:
        VimSocketInterfaceServer()
        :   mSocket(-1)
        ,   mPort(0)
        ,   mListening(false)
        {
            memset(&mSckaddr, 0, sizeof(mSckaddr));
        }

        sigc::signal<void,VimSocketInterfaceCommunicator*> signal_newClient() const
        {
            return mNewClientSignal;
        }

        void set_port(unsigned short port)
        {
            mPort = port;
        }

        unsigned short get_port() const
        {
            return mPort;
        }

        unsigned short get_realport() const
        {
            return ntohs(mSckaddr.sin_port);
        }

        bool create()
        {
            mSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (mSocket == -1)
            {
                perror("Cannot open a listening TCP socket");
                return false;
            }
            return true;
        }

        bool bind()
        {
            memset(&mSckaddr, 0, sizeof(mSckaddr));
            mSckaddr.sin_family = AF_INET;
            mSckaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            mSckaddr.sin_port = htons((unsigned short)mPort);
            if (::bind(mSocket, (struct sockaddr *)&mSckaddr, sizeof(mSckaddr)) == -1)
            {
                perror("bind() error");
                return false;
            }
            return true;
        }

        bool listen()
        {
            if (::listen(mSocket, 1) == -1)
            {
                perror("listen() error");
                mListening = false;
                return false;
            }
            mListening = true;
            return true;
        }

        bool accept()
        {
            sockaddr_in addr;
            socklen_t len = sizeof(addr);
            std::cout << "Accepting..." << std::endl;
            int sckConn = ::accept(mSocket, (sockaddr*)&addr, &len);
            if (sckConn < 0)
            {
                perror("Cannot accept connection");
                return false;
            }

            VimSocketInterfaceCommunicator* conn = new VimSocketInterfaceCommunicator(sckConn, addr);
            if (mNewClientSignal.empty()) {
                // Proper closing
                conn->close();
                delete conn;
            } else {
                // gives ownership
                mNewClientSignal.emit(conn);
            }
            return true;
        }

        void accept_loop()
        {
            while (mSocket != -1)
            {
                if (!accept())
                    break;
            }
        }

    private:
        static void* accept_loop(void* pthat)
        {
            reinterpret_cast<VimSocketInterfaceServer*>(pthat)->accept_loop();
            std::cout << "Accept thread finished" << std::endl;
            return NULL;
        }

    public:
        bool accept_loop_async()
        {
            pthread_t thread;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            if (pthread_create(&thread, &attr, &VimSocketInterfaceServer::accept_loop, reinterpret_cast<void*>(this)) != 0)
            {
                perror("Cannot start accept thread");
                return false;
            }
            std::cout << "Accept thread started" << std::endl;
            pthread_attr_destroy(&attr);
            return true;
        }

        bool listening() const
        {
            return mListening;
        }

        void close()
        {
            ::close(mSocket);
            mSocket = -1;
            mListening = false;
        }

        bool created() const
        {
            return mSocket != -1;
        }

    private:
        int             mSocket;
        sockaddr_in     mSckaddr;
        unsigned short  mPort;
        bool            mListening;
        sigc::signal<void,VimSocketInterfaceCommunicator*>
                        mNewClientSignal;
};



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

            mServer.set_port(1234);
            mServer.signal_newClient().connect(sigc::mem_fun(*this, &GVim::on_vimsocket_newclient));

            // Enters Gtk's mainloop
            show_all();
        }

        virtual ~GVim()
        {
            mServer.close();
        }

        void startServer()
        {
            if (mServer.created()) return; // already started

            mServer.create();
            mServer.bind();
            mServer.listen();
            std::cout << "Real server port: " << mServer.get_realport() << std::endl;
        }

        void on_vimsocket_newclient(VimSocketInterfaceCommunicator* conn)
        {
            mpVim = conn;
            //TODO: Bind to signals
            // mpVim->signal_...().connect(sigc::mem_fun(*this, &GVim::...));
            //TODO: When receiving 0:startupDone=0, call 1:create!1
            //TODO: When receiving 0:disconnect=0, call mpVim->close();
            mpVim->readloop_async();
        }

        void makeGVim()
        {
            startServer();

            Gdk::NativeWindow xid = mSocket.get_id();
            std::stringstream cmd;
            // Don't use ' &' or vim can be still initializing while the rest of this program will run (vim server not started yet)
            cmd << "gvim --servername TwistIDE -nb:127.0.0.1:" << mServer.get_realport() << " --socketid " << xid << " &";
            std::cout << cmd.str() << std::endl;

            int rtn = system(cmd.str().c_str());
            if (rtn < 0 || !WIFEXITED(rtn) || WEXITSTATUS(rtn) != 0)
            {
                std::cerr << "Cannot run gvim!" << std::endl;
            }

            // Only accept one client
            mServer.accept();
            // Then quit server
            mServer.close();
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

        VimSocketInterfaceServer
                        mServer;
        VimSocketInterfaceCommunicator*
                        mpVim;
};

int main(int argc, char** argv)
{
    Gtk::Main kit(argc, argv);

    GVim w;

    kit.run(w);

    return 0;
}
