/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
#ifndef PV_SHAREDSTATE_H
#define PV_SHAREDSTATE_H

#include <string>
#include <list>

#include <shareLib.h>
#include <pv/sharedPtr.h>
#include <pv/noDefaultMethods.h>
#include <pv/bitSet.h>

#include <pva/server.h>

namespace epics{namespace pvData{
class Structure;
class PVStructure;
class BitSet;
class Status;
}} // epics::pvData
namespace epics{namespace pvAccess{
class ChannelProvider;
class Channel;
class ChannelRequester;
class ChannelBaseRequester;
class GetFieldRequester;
}} // epics::pvAccess

namespace pvas {

struct SharedChannel;
struct SharedMonitorFIFO;
struct SharedPut;
struct SharedRPC;

struct Operation;

/** @addtogroup pvas
 * @{
 */

/** A Shared State Process Variable (PV)
 *
 * "Shared" in the sense that all clients/subscribers interact with the
 * same PVStructure.
 *
 * @warning For the purposes of locking, this class is an Operation (see @ref provider_roles_requester_locking).
 *          eg. no locks may be held when calling post(), open(), close(), or connect().
 *
 * This class contains a cached PVStructure, which is updated by post(),
 * also a list of subscribing clients and in-progress network Operations.
 *
 * On construction a SharedPV is in a "disconnected" state.
 * It has no associated PVStructure (or Structure).  No type.
 * A type is associated via the open() method.
 * After it has been open()'d.  Calls to post() may be made.
 * Calling close() will close all currently opened client channels.
 *
 * Client channels, and operations on them, may be initiated at any time (via connect()).
 * However, operations will not be fully created until open() is called.
 *
 * @note A SharedPV does not have a name.  Name(s) are associated with a SharedPV
 *       By a Provider (StaticProvider, DynamicProvider, or any epics::pvAccess::ChannelProvider).
 *       These channel names may be seen via connect()
 *
 * @see @ref pvas_sharedptr
 */
class epicsShareClass SharedPV
        : public pvas::StaticProvider::ChannelBuilder
{
    friend struct SharedChannel;
    friend struct SharedMonitorFIFO;
    friend struct SharedPut;
    friend struct SharedRPC;
public:
    POINTER_DEFINITIONS(SharedPV);
    /** Callbacks associated with a SharedPV.
     *
    * @note For the purposes of locking, this class is an Requester (see @ref provider_roles_requester_locking)
     */
    struct epicsShareClass Handler {
        POINTER_DEFINITIONS(Handler);
        virtual ~Handler() {}
        virtual void onFirstConnect(const SharedPV::shared_pointer& pv) {}
        //! Called when the last client disconnects.  May close()
        virtual void onLastDisconnect(const SharedPV::shared_pointer& pv) {}
        //! Client requests Put
        virtual void onPut(const SharedPV::shared_pointer& pv, Operation& op) {}
        //! Client requests RPC
        virtual void onRPC(const SharedPV::shared_pointer& pv, Operation& op) {}
    };

    /** Allocate a new PV in the closed state.
     * @param handler Our callbacks.  May be NULL.  Stored internally as a shared_ptr<>
     * @post In the closed state
     */
    static shared_pointer build(const std::tr1::shared_ptr<Handler>& handler);
    //! A SharedPV which fails all Put and RPC operations.
    static shared_pointer buildReadOnly();
    //! A SharedPV which accepts all Put operations, and fails all RPC operations.
    static shared_pointer buildMailbox();
private:
    explicit SharedPV(const std::tr1::shared_ptr<Handler>& handler);
public:
    virtual ~SharedPV();

    //! Replace Handler given with ctor
    void setHandler(const std::tr1::shared_ptr<Handler>& handler);
    Handler::shared_pointer getHandler() const;

    //! test open-ness.  cf. open() and close()
    bool isOpen() const;

    //! Shorthand for @code open(value, pvd::BitSet().set(0)) @endcode
    void open(const epics::pvData::PVStructure& value);

    //! Begin allowing clients to connect.
    //! @param value The initial value of this PV.  (any pending Get operation will complete this this)
    //! @param valid Only these marked fields are considered to have non-default values.
    //! @throws std::logic_error if not in the closed state.
    //! @post In the opened state
    //! @note Provider locking rules apply (@see provider_roles_requester_locking).
    void open(const epics::pvData::PVStructure& value, const epics::pvData::BitSet& valid);

    //! Shorthand for @code open(*pvd::getPVDataCreate()->createPVStructure(type), pvd::BitSet().set(0)) @endcode
    void open(const epics::pvData::StructureConstPtr& type);

    //! Force any clients to disconnect, and prevent re-connection
    //! @param destroy Indicate whether this close() is permanent for clients.
    //!                If destroy=false, the internal client list is retained, and these clients will see a subsequent open().
    //!                If destory=true, the internal client list is cleared.
    //! @post In the closed state
    //! @note Provider locking rules apply (@see provider_roles_requester_locking).
    virtual void close(bool destroy=false);

    //! Create a new container which may be used to prepare to call post().
    //! This container will be owned exclusively by the caller.
    std::tr1::shared_ptr<epics::pvData::PVStructure> build();

    //! Update the cached PVStructure in this SharedPV.
    //! Only those fields marked as changed will be copied in.
    //! Makes a light-weight copy.
    //! @pre isOpen()==true
    //! @throws std::logic_error if !isOpen()
    //! @note Provider locking rules apply (@see provider_roles_requester_locking).
    void post(const epics::pvData::PVStructure& value,
              const epics::pvData::BitSet& changed);

    //! Update arguments with current value, which is the initial value from open() with accumulated post() calls.
    void fetch(epics::pvData::PVStructure& value, epics::pvData::BitSet& valid);

    //! may call Handler::onFirstConnect()
    //! @note Provider locking rules apply (@see provider_roles_requester_locking).
    virtual std::tr1::shared_ptr<epics::pvAccess::Channel> connect(
            const std::tr1::shared_ptr<epics::pvAccess::ChannelProvider>& provider,
            const std::string& channelName,
            const std::tr1::shared_ptr<epics::pvAccess::ChannelRequester>& requester);

    void setDebug(int lvl);
    int isDebug() const;

private:
    static size_t num_instances;

    weak_pointer internal_self; // const after build()

    mutable epicsMutex mutex;

    std::tr1::shared_ptr<SharedPV::Handler> handler;

    typedef std::list<SharedPut*> puts_t;
    typedef std::list<SharedRPC*> rpcs_t;
    typedef std::list<SharedMonitorFIFO*> monitors_t;
    typedef std::list<std::tr1::weak_ptr<epics::pvAccess::GetFieldRequester> > getfields_t;
    typedef std::list<SharedChannel*> channels_t;

    std::tr1::shared_ptr<const epics::pvData::Structure> type;

    puts_t puts;
    rpcs_t rpcs;
    monitors_t monitors;
    getfields_t getfields;
    channels_t channels;

    std::tr1::shared_ptr<epics::pvData::PVStructure> current;
    //! mask of fields which are considered to have non-default values.
    //! Used for initial Monitor update and Get operations.
    epics::pvData::BitSet valid;

    int debugLvl;

    EPICS_NOT_COPYABLE(SharedPV)
};

//! An in-progress network operation (Put or RPC).
//! Use value(), changed() to see input data, and
//! call complete() when done handling.
struct epicsShareClass Operation {
    POINTER_DEFINITIONS(Operation);
    struct Impl;
private:
    std::tr1::shared_ptr<Impl> impl;

    friend struct SharedPut;
    friend struct SharedRPC;
    explicit Operation(const std::tr1::shared_ptr<Impl> impl);
public:
    Operation() {} //!< create empty op for later assignment

    //! pvRequest blob, may be used to modify handling.
    const epics::pvData::PVStructure& pvRequest() const;
    const epics::pvData::PVStructure& value() const; //!< Input data
    //! Applies to value().  Which fields of input data are actual valid.  Others should not be used.
    const epics::pvData::BitSet& changed() const;
    //! The name of the channel through which this request was made (eg. for logging purposes).
    std::string channelName() const;

    void complete(); //!< shorthand for successful completion w/o data (Put or RPC with void return)
    //! Complete with success or error w/o data.
    void complete(const epics::pvData::Status& sts);
    //! Sucessful completion with data (RPC only)
    void complete(const epics::pvData::PVStructure& value,
                  const epics::pvData::BitSet& changed);

    //! Send info message to client.  Does not complete().
    void info(const std::string&);
    //! Send warning message to client.  Does not complete().
    void warn(const std::string&);

    int isDebug() const;

    // escape hatch.  never NULL
    std::tr1::shared_ptr<epics::pvAccess::Channel> getChannel();
    // escape hatch.  may be NULL
    std::tr1::shared_ptr<epics::pvAccess::ChannelBaseRequester> getRequester();

    bool valid() const;

#if __cplusplus>=201103L
    explicit operator bool() const { return valid(); }
#else
private:
    typedef bool (Operation::*bool_type)() const;
public:
    operator bool_type() const { return valid() ? &Operation::valid : 0; }
#endif
};

} // namespace pvas

//! @}

#endif // PV_SHAREDSTATE_H
