#include <string>
#include <iostream>
#include <sstream>
#include <ctime>
#include <stdexcept>
#include <cstdarg>
#include <typeinfo>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <pthread.h>
#include <sigc++/sigc++.h>

#include <gtkmm.h>
#include <gtkmm/socket.h>



class VimValue
{
    public:
        enum Type
        {
            STRING          = 1,
            NUMBER          = 2,
            BOOLEAN         = 3,
            OFFSET          = 4,
            LINE_NUMBER     = 5,
            COLUMN_NUMBER   = 6,
            PATHNAME        = 7,
            COLOR           = 8
        };
        static const std::string TypeToString(Type type)
        {
            switch (type)
            {
                case STRING:
                    return "STRING";
                case NUMBER:
                    return "NUMBER";
                case BOOLEAN:
                    return "BOOLEAN";
                case OFFSET:
                    return "OFFSET";
                case LINE_NUMBER:
                    return "LINE_NUMBER";
                case COLUMN_NUMBER:
                    return "COLUMN_NUMBER";
                case PATHNAME:
                    return "PATHNAME";
                case COLOR:
                    return "COLOR";
                default:
                    return "UNEXISTING_TYPE";
            }
        }

    public:
        VimValue(Type type, std::string str)
        {
            if (type != STRING && type != PATHNAME && type != COLOR)
                throw std::invalid_argument("Invalid VimValue type");
            mType = type;
            mString = str;
        }

        VimValue(Type type, long nbr)
        {
            if (type != NUMBER && type != OFFSET && type != LINE_NUMBER && type != COLUMN_NUMBER)
                throw std::invalid_argument("Invalid VimValue type");
            mType = type;
            mNumber = nbr;
        }

        VimValue(Type type, bool bol)
        {
            if (type != BOOLEAN)
                throw std::invalid_argument("Invalid VimValue type");
            mType = type;
            mBoolean = bol;
        }

        static VimValue newString(const std::string& value)
        {
            return VimValue(STRING, value);
        }

        static VimValue newNumber(long value)
        {
            return VimValue(NUMBER, value);
        }

        static VimValue newBoolean(bool value)
        {
            return VimValue(BOOLEAN, value);
        }

        static VimValue newOffset(long value)
        {
            return VimValue(OFFSET, value);
        }

        static VimValue newLineNumber(long value)
        {
            return VimValue(LINE_NUMBER, value);
        }

        static VimValue newColumnNumber(long value)
        {
            return VimValue(COLUMN_NUMBER, value);
        }

        static VimValue newPathName(const std::string& value)
        {
            return VimValue(PATHNAME, value);
        }

        static VimValue newColor(const std::string& value)
        {
            return VimValue(COLOR, value);
        }

        static VimValue parse(Type type, std::istream& input)
        {
            if (type == NUMBER || type == OFFSET || type == LINE_NUMBER || type == COLUMN_NUMBER)
            {
                long n;
                input >> n;
                return VimValue(type, n);
            }
            else if (type == BOOLEAN)
            {
                std::string s;
                input >> s;
                return VimValue(type, s == "T");
            }
            else if (type == COLOR)
            {
                std::string s;
                input >> s;
                return VimValue(type, s);
            }
            else if (type == STRING || type == PATHNAME)
            {
                char c;
                input.get(c);
                if (c != '"')
                    throw std::invalid_argument("Invalid input for parsing type STRING or PATHNAME");
                std::string s;
                while (input.good())
                {
                    if (c == '\\')
                    {
                        input.get(c);
                        if (c == '"')
                            s.push_back('"');
                        else if (c == 'n')
                            s.push_back('\n');
                        else if (c == 'r')
                            s.push_back('\r');
                        else if (c == 'r')
                            s.push_back('\t');
                        else if (c == '\\')
                            s.push_back('\\');
                        // ADDITIONS
                        else if (c == '0')
                            s.push_back('\0');
                        else
                            s.push_back(c);
                    }
                    else
                    {
                        input.get(c);
                        if (c == '\\')
                            continue;
                        else if (c == '"')
                            break;
                        else
                            s.push_back(c);
                    }
                }
                // Read space separation, if any
                if (input.good())
                {
                    input.get(c);
                    // Put it back if the read character (if any) is not a space
                    if (input.gcount() == 1 && c != ' ')
                        // Denotes an error...
                        input.unget();
                }
                return VimValue(type, s);
            }
            else
            {
                throw std::invalid_argument("Unknown or unparsable type");
            }
        }

        Type getType() const
        {
            return mType;
        }

        std::string toString() const
        {
            switch (mType)
            {
                case STRING:
                case PATHNAME:
                case COLOR:
                    return mString;
                case NUMBER:
                case OFFSET:
                case LINE_NUMBER:
                case COLUMN_NUMBER:
                    {
                        std::stringstream tostr;
                        tostr << mNumber;
                        return tostr.str();
                    }
                    break;
                case BOOLEAN:
                    if (mBoolean)
                        return "T";
                    else
                        return "F";
                default:
                    throw std::invalid_argument("Unknown or un-toString-able type");
            }
        }

        std::string getSomeString() const
        {
            return mString;
        }

        long getSomeNumeric() const
        {
            return mNumber;
        }

        bool getSomeBoolean() const
        {
            return mBoolean;
        }

        std::string getString() const
        {
            return mString;
        }

        long getNumber() const
        {
            return mNumber;
        }

        bool getBoolean() const
        {
            return mBoolean;
        }

        long getOffset() const
        {
            return mNumber;
        }

        long getLineNumber() const
        {
            return mNumber;
        }

        long getColumnNumber() const
        {
            return mNumber;
        }

        std::string getPathName() const
        {
            return mString;
        }

        std::string getColor() const
        {
            return mString;
        }

    private:
        Type        mType;
        std::string mString;
        long        mNumber;
        bool        mBoolean;
};



class VimSocketInterfaceCommunicator;
class VimEvent
{
    public:
        VimEvent(const std::string& name)
        :   mName(name)
        {
        }

        VimEvent(const std::string& name, int paramCount, VimValue::Type param1Type, ...)
        :   mName(name)
        {
            mParameterTypes.reserve(paramCount);
            va_list args;
            va_start(args, param1Type);
            mParameterTypes.push_back(param1Type);
            for ( ; --paramCount > 0 ; )
            {
                VimValue::Type type = static_cast<VimValue::Type>(va_arg(args, int)); // VimValue::Type is promoted to int
                mParameterTypes.push_back(type);
            }
            va_end(args);
        }

        const std::string& getName()
        {
            return mName;
        }

        const std::vector<VimValue::Type>& getParameterTypes()
        {
            return mParameterTypes;
        }

        void emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& parameters)
        {
            if (parameters.size() != mParameterTypes.size())
                throw std::invalid_argument("Bad parameter count");
            std::vector<VimValue::Type>::iterator types = mParameterTypes.begin();
            std::vector<VimValue>::const_iterator values = parameters.begin();
            for (int i = mParameterTypes.size() ; i > 0 ; i--, types++, values++)
            {
                if (values->getType() != *types)
                    throw std::invalid_argument("Bad parameter type");
            }
            do_emit(vim, bufID, seqno, parameters);
        }

        virtual void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& typeCheckedParameters) = 0;

        virtual sigc::signal_base get_signalbase() = 0;

    private:
        const std::string           mName;
        std::vector<VimValue::Type> mParameterTypes;
};

class VimEventBalloonEval : public VimEvent
{
    friend class VimEventManager;
    private:
        VimEventBalloonEval()
        :   VimEvent(VimEventBalloonEval::getName(), 3, VimValue::OFFSET, VimValue::NUMBER, VimValue::STRING)
        {
        }

    public:
        static const std::string getName()
        {
            return "balloonEval";
        }

        void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, seqno, parameters[0].getOffset(), parameters[1].getNumber(), parameters[2].getString());
        }

        sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long,long,std::string> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long,long,std::string>  mSignal;
};

class VimEventBalloonText : public VimEvent
{
    friend class VimEventManager;
    private:
        VimEventBalloonText()
        :   VimEvent(VimEventBalloonText::getName(), 1, VimValue::STRING)
        {
        }

    public:
        static const std::string getName()
        {
            return "balloonText";
        }

        void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, seqno, parameters[0].getString());
        }

        sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,std::string> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,std::string>    mSignal;
};

class VimEventButtonRelease : public VimEvent
{
    friend class VimEventManager;
    private:
        VimEventButtonRelease()
        :   VimEvent(VimEventButtonRelease::getName(), 1, VimValue::NUMBER, VimValue::LINE_NUMBER, VimValue::COLUMN_NUMBER)
        {
        }

    public:
        static const std::string getName()
        {
            return "buttonRelease";
        }

        void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, seqno, parameters[0].getNumber(), parameters[1].getLineNumber(), parameters[2].getColumnNumber());
        }

        sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long,long,long> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long,long,long> mSignal;
};

class VimEventDisconnect : public VimEvent
{
    friend class VimEventManager;
    private:
        VimEventDisconnect()
        :   VimEvent(VimEventDisconnect::getName())
        {
        }

    public:
        static const std::string getName()
        {
            return "disconnect";
        }

        void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, seqno);
        }

        sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long>    mSignal;
};

class VimEventFileOpened: public VimEvent
{
    friend class VimEventManager;
    private:
        VimEventFileOpened()
        :   VimEvent(VimEventFileOpened::getName(), 3, VimValue::PATHNAME, VimValue::BOOLEAN, VimValue::BOOLEAN)
        {
        }

    public:
        static const std::string getName()
        {
            return "fileOpened";
        }

        void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, seqno, parameters[0].getPathName(), parameters[1].getBoolean(), parameters[2].getBoolean());
        }

        sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,std::string,bool,bool> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,std::string,bool,bool>  mSignal;
};

class VimEventGeometry: public VimEvent
{
    friend class VimEventManager;
    private:
        VimEventGeometry()
        :   VimEvent(VimEventGeometry::getName(), 4, VimValue::COLUMN_NUMBER, VimValue::LINE_NUMBER, VimValue::COLUMN_NUMBER, VimValue::LINE_NUMBER)
        {
        }

    public:
        static const std::string getName()
        {
            return "geometry";
        }

        void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, seqno, parameters[0].getColumnNumber(), parameters[1].getLineNumber(), parameters[2].getColumnNumber(), parameters[3].getLineNumber());
        }

        sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long,long,long,long> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long,long,long,long>  mSignal;
};

class VimEventInsert: public VimEvent
{
    friend class VimEventManager;
    private:
        VimEventInsert()
        :   VimEvent(VimEventInsert::getName(), 2, VimValue::OFFSET, VimValue::STRING)
        {
        }

    public:
        static const std::string getName()
        {
            return "insert";
        }

        void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, seqno, parameters[0].getOffset(), parameters[1].getString());
        }

        sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long,std::string> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long,std::string>   mSignal;
};


class VimEventKeyCommand: public VimEvent
{
    friend class VimEventManager;
    private:
        VimEventKeyCommand()
        :   VimEvent(VimEventKeyCommand::getName(), 1, VimValue::STRING)
        {
        }

    public:
        static const std::string getName()
        {
            return "keyCommand";
        }

        void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, seqno, parameters[0].getString());
        }

        sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,std::string> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,std::string>    mSignal;
};

class VimEventKeyAtPos: public VimEvent
{
    friend class VimEventManager;
    private:
        VimEventKeyAtPos()
        :   VimEvent(VimEventKeyAtPos::getName(), 3, VimValue::STRING, VimValue::LINE_NUMBER, VimValue::COLUMN_NUMBER)
        {
        }

    public:
        static const std::string getName()
        {
            return "keyAtPos";
        }

        void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, seqno, parameters[0].getString(), parameters[1].getLineNumber(), parameters[2].getColumnNumber());
        }

        sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,std::string,long,long> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,std::string,long,long>  mSignal;
};

class VimEventKilled: public VimEvent
{
    friend class VimEventManager;
    private:
        VimEventKilled()
        :   VimEvent(VimEventKilled::getName())
        {
        }

    public:
        static const std::string getName()
        {
            return "killed";
        }

        void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, seqno);
        }

        sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long>    mSignal;
};

class VimEventNewDotAndMark: public VimEvent
{
    friend class VimEventManager;
    private:
        VimEventNewDotAndMark()
        :   VimEvent(VimEventNewDotAndMark::getName(), 2, VimValue::OFFSET, VimValue::OFFSET)
        {
        }

    public:
        static const std::string getName()
        {
            return "newDotAndMark";
        }

        void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, seqno, parameters[0].getOffset(), parameters[1].getOffset());
        }

        sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long,long> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long,long>  mSignal;
};

class VimEventRemove: public VimEvent
{
    friend class VimEventManager;
    private:
        VimEventRemove()
        :   VimEvent(VimEventRemove::getName(), 2, VimValue::OFFSET, VimValue::NUMBER)
        {
        }

    public:
        static const std::string getName()
        {
            return "remove";
        }

        void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, seqno, parameters[0].getOffset(), parameters[1].getNumber());
        }

        sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long,long> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,long,long>  mSignal;
};

class VimEventSave: public VimEvent
{
    friend class VimEventManager;
    private:
        VimEventSave()
        :   VimEvent(VimEventSave::getName())
        {
        }

    public:
        static const std::string getName()
        {
            return "save";
        }

        void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, seqno);
        }

        sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long>    mSignal;
};

class VimEventStartupDone: public VimEvent
{
    friend class VimEventManager;
    private:
        VimEventStartupDone()
        :   VimEvent(VimEventStartupDone::getName())
        {
        }

    public:
        static const std::string getName()
        {
            return "startupDone";
        }

        void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, seqno);
        }

        sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long>    mSignal;
};

class VimEventUnmodified: public VimEvent
{
    friend class VimEventManager;
    private:
        VimEventUnmodified()
        :   VimEvent(VimEventUnmodified::getName())
        {
        }

    public:
        static const std::string getName()
        {
            return "unmodified";
        }

        void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, seqno);
        }

        sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long>    mSignal;
};

class VimEventVersion: public VimEvent
{
    friend class VimEventManager;
    private:
        VimEventVersion()
        :   VimEvent(VimEventVersion::getName(), 1, VimValue::STRING)
        {
        }

    public:
        static const std::string getName()
        {
            return "version";
        }

        void do_emit(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, const std::vector<VimValue>& parameters)
        {
            mSignal.emit(vim, bufID, seqno, parameters[0].getString());
        }

        sigc::signal_base get_signalbase()
        {
            return mSignal;
        }

        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,std::string> get_signal()
        {
            return mSignal;
        }

    private:
        sigc::signal<void,VimSocketInterfaceCommunicator&,long,long,std::string>    mSignal;
};



class VimEventManager
{
    public:
        VimEventManager()
        {
        }

        template <class E>
        E* addEvent()
        {
            const std::type_info& type = typeid(E);
            std::map<std::string, VimEvent*>::iterator pos;
            pos = mEventsByTypeName.find(type.name());
            if (pos != mEventsByTypeName.end())
                return dynamic_cast<E*>(pos->second);
            E* evt = new E();
            mEventsByTypeName.insert(std::pair<std::string,VimEvent*>(type.name(), evt));
            mEventsByName.insert(std::pair<std::string,VimEvent*>(evt->getName(), evt));
            return evt;
        }

        template <class E>
        E* getEvent()
        {
            std::map<std::string, VimEvent*>::iterator pos;
            pos = mEventsByTypeName.find(typeid(E).name());
            if (pos != mEventsByTypeName.end())
                return dynamic_cast<E*>(pos->second);
            return NULL;
        }

        VimEvent* getEvent(const std::string& name)
        {
            std::map<std::string, VimEvent*>::iterator pos;
            pos = mEventsByName.find(name);
            if (pos != mEventsByName.end())
                return pos->second;
            return NULL;
        }

        sigc::signal<void, VimSocketInterfaceCommunicator&, std::string> get_signal_special_auth()
        {
            return mSpecialAuthSignal;
        }

        void emit_special_auth(VimSocketInterfaceCommunicator& vim, std::string password)
        {
            mSpecialAuthSignal.emit(vim, password);
        }

    private:
        std::map<std::string, VimEvent*>                            mEventsByName;
        std::map<std::string, VimEvent*>                            mEventsByTypeName;
        sigc::signal<void, VimSocketInterfaceCommunicator&, std::string>  mSpecialAuthSignal;
};



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

        void setEventManager(VimEventManager* manager)
        {
            mEventManager = manager;
        }

        void readloop_async()
        {
            pthread_t thread;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            if (pthread_create(&thread, &attr, &VimSocketInterfaceCommunicator::readloop, reinterpret_cast<void*>(this)) != 0)
            {
                perror("Cannot start client socket reading thread");
                throw std::runtime_error("pthread_create() failed");
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

            std::stringstream line;
            char buffer[1051];
            bzero(buffer, 1051);
            while (mSocket != -1)
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
                char* start = buffer;
                char* nl = strchr(start, '\n');
                for ( ; nl != NULL ; nl = strchr(start, '\n'))
                {
                    *nl ='\0';
                    std::cout << "Adding \"" << start << "\"" << std::endl;
                    line << start;
                    analyze_answer(line);
                    line.clear(); // clear eof()
                    start = nl+1;
                };
                std::cout << "Adding unfinished \"" << start << "\"" << std::endl;
                line << start;
            }

            std::cout << "Exitting readloop" << std::endl;
            close(); // safe to call twice
            return;
        }

        void analyze_answer(std::stringstream& line)
        {
            std::cout << "Analyzing : \"" << line.str() << "\"" << std::endl;
            // Read first number: seqno or bufID
            char c;
            long n = 0;
            long bufID = -1;
            long seqno = -1;
            while (line.good())
            {
                line.get(c);
                if (c >= '0' && c <= '9')
                {
                    n = n*10 + c-'0';
                }
                else
                {
                    break;
                }
            }
            if (c == ' ')
            {
                seqno = n;
                std::cout << "seqno = " << seqno << std::endl;
                // Following are space separated words for return values
                //TODO: Have some model to emit an answer to the right function call
                //      Based on seqno.
            }
            else if (c == ':')
            {
                bufID = n;
                std::cout << "bufID = " << bufID << std::endl;
                std::string evtName;
                while (line.good())
                {
                    line.get(c);
                    if (c == '=')
                    {
                        break;
                    }
                    else
                    {
                        evtName.push_back(c);
                    }
                }
                std::cout << "Event = " << evtName << std::endl;
                line >> seqno;
                std::cout << "seqno = " << seqno << std::endl;
                // Following are space separated words for parameters values
                if (mEventManager == NULL)
                {
                    std::cout << "End of the adventure, no VimEventManager set." << std::endl;
                }
                else
                {
                    VimEvent* evt = mEventManager->getEvent(evtName);
                    if (evt == NULL)
                    {
                        std::cout << "UNKNOWN EVENT!" << std::endl;
                    }
                    else
                    {
                        std::cout << "Found event : " << evt->getName() << std::endl;
                        const std::vector<VimValue::Type>& paramTypes = evt->getParameterTypes();
                        std::vector<VimValue> params;
                        params.reserve(paramTypes.size());
                        for (std::vector<VimValue::Type>::const_iterator type = paramTypes.begin() ; type < paramTypes.end() ; type++)
                        {
                            // Skip space separators if not already read
                            while (line.good() && line.peek() == ' ') line.get(c);
                            // Should be data for the current parameter
                            if (!line.good())
                                throw std::invalid_argument("Not enough data for to read all parameters");
                            // Parse next parameter
                            VimValue param = VimValue::parse(*type, line);
                            params.push_back(param);
                            std::cout << "Param : " << VimValue::TypeToString(param.getType()) << " = \"" << param.toString() << "\"" << std::endl;
                        }
                        // Emit the event
                        evt->emit(*this, bufID, seqno, params);
                    }
                }
            }
            else
            {
                line.unget();
                std::string w;
                line >> w;
                std::cout << "Special command \"" << w << "\"" << std::endl;
                if (w == "AUTH")
                {
                    line >> w;
                    std::cout << "Password : \"" << w << "\"" << std::endl;
                    if (mEventManager == NULL)
                    {
                        std::cout << "End of the adventure, no VimEventManager set." << std::endl;
                    }
                    else
                    {
                        mEventManager->emit_special_auth(*this, w);
                    }
                }
                else if (w == "ACCEPT")
                {
                    // Unused
                }
                else if (w == "REJECT")
                {
                    // Unused
                }
                else
                {
                    std::cout << "ERROR !" << std::endl;
                }
            }
            // Eat the stream upto the end
            while (line.good())
            {
                std::string s;
                line >> s;
                std::cout << "UNREAD Word : \"" << s << "\"" << std::endl;
            }
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
        int                 mSocket;
        sockaddr_in         mSckaddr;
        VimEventManager*    mEventManager;
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
            setupSignals();

            set_title("TwistIDE - GVim embedding test");
            mSocket.signal_realize().connect(sigc::mem_fun(*this, &GVim::on_mSocket_realize));
            mSocket.signal_grab_focus().connect(sigc::mem_fun(*this, &GVim::on_mSocket_grab_focus));
            mSocket.signal_plug_removed().connect(sigc::mem_fun(*this, &GVim::on_mSocket_plug_removed));
            mSocket.signal_plug_added().connect(sigc::mem_fun(*this, &GVim::on_mSocket_plug_added));

            mBox.pack_start(mSocket, true, true);

            add(mBox);
            set_default_size(640, 480);
            set_position(Gtk::WIN_POS_CENTER_ALWAYS);
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

        void setupSignals()
        {
            mEventManager.get_signal_special_auth().connect(sigc::mem_fun(*this, &GVim::on_vimclient_special_auth));
            mEventManager.addEvent<VimEventBalloonEval>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_balloonEval));
            mEventManager.addEvent<VimEventBalloonText>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_balloonText));
            mEventManager.addEvent<VimEventButtonRelease>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_buttonRelease));
            mEventManager.addEvent<VimEventDisconnect>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_disconnect));
            mEventManager.addEvent<VimEventFileOpened>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_fileOpened));
            mEventManager.addEvent<VimEventGeometry>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_geometry));
            mEventManager.addEvent<VimEventInsert>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_insert));
            mEventManager.addEvent<VimEventKeyCommand>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_keyCommand));
            mEventManager.addEvent<VimEventKeyAtPos>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_keyAtPos));
            mEventManager.addEvent<VimEventKilled>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_killed));
            mEventManager.addEvent<VimEventNewDotAndMark>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_newDotAndMark));
            mEventManager.addEvent<VimEventRemove>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_remove));
            mEventManager.addEvent<VimEventSave>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_save));
            mEventManager.addEvent<VimEventStartupDone>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_startupDone));
            mEventManager.addEvent<VimEventUnmodified>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_unmodified));
            mEventManager.addEvent<VimEventVersion>()->get_signal().connect(sigc::mem_fun(*this, &GVim::on_vimclient_version));
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
            mpVim->setEventManager(&mEventManager);
            mpVim->readloop_async();
        }

        void on_vimclient_special_auth(VimSocketInterfaceCommunicator& vim, std::string password)
        {
            std::cout << "Vim authenticating with password \"" << password << "\"" << std::endl;
        }

        void on_vimclient_balloonEval(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, long offset, long len, std::string type)
        {
            std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::balloonEval(offset="<<offset<<", len="<<len<<", type="<<type<<")" << std::endl;
        }

        void on_vimclient_balloonText(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, std::string text)
        {
            std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::balloonText(text="<<text<<")" << std::endl;
        }

        void on_vimclient_buttonRelease(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, long button, long line, long col)
        {
            std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::buttonRelease(button="<<button<<", line="<<line<<", col="<<col<<")" << std::endl;
        }

        void on_vimclient_disconnect(VimSocketInterfaceCommunicator& vim, long bufID, long seqno)
        {
            std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::disconnect()" << std::endl;
            std::cout << "Vim is disconnecting!" << std::endl;
            vim.close();
            if (mpVim == &vim)
                mpVim = NULL;
        }

        void on_vimclient_fileOpened(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, std::string pathname, bool open, bool modified)
        {
            std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::fileOpened(pathname="<<pathname<<", open="<<open<<", modified="<<modified<<")" << std::endl;
        }

        void on_vimclient_geometry(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, long cols, long rows, long x, long y)
        {
            std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::geometry(cols="<<cols<<", rows="<<rows<<", x="<<x<<", y="<<y<<")" << std::endl;
        }

        void on_vimclient_insert(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, long offset, std::string text)
        {
            std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::insert(offset="<<offset<<", text="<<text<<")" << std::endl;
        }

        void on_vimclient_keyCommand(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, std::string keyName)
        {
            std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::keyCommand(keyName="<<keyName<<")" << std::endl;
        }

        void on_vimclient_keyAtPos(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, std::string keyName, long lnum, long lcol)
        {
            std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::keyAtPos(keyName="<<keyName<<", lnum="<<lnum<<", lcol="<<lcol<<")" << std::endl;
        }

        void on_vimclient_killed(VimSocketInterfaceCommunicator& vim, long bufID, long seqno)
        {
            std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::killed()" << std::endl;
        }

        void on_vimclient_newDotAndMark(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, long offset1, long offset2)
        {
            std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::newDotAndMark(offset1="<<offset1<<", offset2="<<offset2<<")" << std::endl;
        }

        void on_vimclient_remove(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, long offset, long length)
        {
            std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::remove(offset="<<offset<<", length="<<length<<")" << std::endl;
        }

        void on_vimclient_save(VimSocketInterfaceCommunicator& vim, long bufID, long seqno)
        {
            std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::save()" << std::endl;
        }

        void on_vimclient_startupDone(VimSocketInterfaceCommunicator& vim, long bufID, long seqno)
        {
            std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::startupDone()" << std::endl;
            //TODO: call 1:create!1
        }

        void on_vimclient_unmodified(VimSocketInterfaceCommunicator& vim, long bufID, long seqno)
        {
            std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::unmodified()" << std::endl;
        }

        void on_vimclient_version(VimSocketInterfaceCommunicator& vim, long bufID, long seqno, std::string version)
        {
            std::cout << "Vim(bufID="<<bufID<<",seqno="<<seqno<<")::version(version="<<version<<")" << std::endl;
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

        VimEventManager mEventManager;
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
