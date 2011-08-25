
/* testChannelConnect.cpp */
/* Author:  Matej Sekoranja Date: 2011.8.24 */


#include <iostream>
#include <sstream>
#include <pv/CDRMonitor.h>
#include <epicsExit.h>
#include <pv/clientContextImpl.h>
#include <pv/clientFactory.h>

using namespace epics::pvData;
using namespace epics::pvAccess;

#define N_CHANNELS 10000

static Event g_event;

class ChannelRequesterImpl : public ChannelRequester
{
public:
    ChannelRequesterImpl() : count(0) {}
private:
    int count;
    
    virtual String getRequesterName()
    {
        return "ChannelRequesterImpl";
    };

    virtual void message(String message,MessageType messageType)
    {
        std::cout << "[" << getRequesterName() << "] message(" << message << ", " << messageTypeName[messageType] << ")" << std::endl;
    }

    virtual void channelCreated(const epics::pvData::Status& status, Channel::shared_pointer const & channel)
    {
        if (!status.isSuccess())
        {
            std::cout << "channelCreated(" << status.toString() << ", "
                      << (channel ? channel->getChannelName() : "(0)") << ")" << std::endl;
        }
    }

    // always called from the same thread
    virtual void channelStateChange(Channel::shared_pointer const & c, Channel::ConnectionState connectionState)
    {
        if (connectionState == Channel::CONNECTED)
        {
            cout << c->getChannelName() << " CONNECTED: " << (count+1) << endl;
            if (++count == N_CHANNELS)
                g_event.signal();
        }
        else if (connectionState == Channel::DISCONNECTED)
        {
            --count;
            cout << c->getChannelName() << " DISCONNECTED: " << count << endl;
        }
        else
            cout << c->getChannelName() << " " << Channel::ConnectionStateNames[connectionState] << endl;
        
    }
};



int main(int argc,char *argv[])
{
    {
        ClientFactory::start();
        ChannelProvider::shared_pointer provider = getChannelAccess()->getProvider("pvAccess");
        ChannelRequester::shared_pointer channelRequester(new ChannelRequesterImpl());
        
        Channel::shared_pointer channels[N_CHANNELS];
        char buf[16];
        for (int i = 0; i < N_CHANNELS; i++)
        {
            sprintf(buf, "record%d", (i+1));
            channels[i] = provider->createChannel(buf, channelRequester);
        }
        
        g_event.wait();
        
        cout << "connected to all" << endl;
        
        ClientFactory::stop();
    }
    
    epicsThreadSleep ( 1.0 ); 
    std::cout << "-----------------------------------------------------------------------" << std::endl;
    epicsExitCallAtExits();
    CDRMonitor::get().show(stdout);
    return(0);
}
