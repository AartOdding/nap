#pragma once

// Local Includes
#include "websocketutils.h"
#include "websocketconnection.h"
#include "websocketmessage.h"

// External Includes
#include <nap/device.h>
#include <memory.h>
#include <future>
#include <nap/signalslot.h>
#include <mutex>
#include <atomic>

namespace nap
{
	class IWebSocketClient;
	class WebSocketClientWrapper;

	/**
	 * Web-socket client endpoint.
	 */
	class NAPAPI WebSocketClientEndPoint : public Device
	{
		friend class IWebSocketClient;
		RTTI_ENABLE(Device)
	public:
		virtual ~WebSocketClientEndPoint();

		/**
		* Initialize this object after de-serialization
		* @param errorState contains the error message when initialization fails
		*/
		virtual bool init(utility::ErrorState& errorState) override;

		virtual void stop() override;

		virtual bool start(nap::utility::ErrorState& error) override;

		/**
		 * Sends a message to the specified connection
		 * @param connection the server connection
		 * @param message the message to send
		 * @param code message type
		 * @param error contains the error if sending fails
		 * @return if message was send successfully
		 */
		bool send(const WebSocketConnection& connection, const std::string& message, EWebSocketOPCode code, nap::utility::ErrorState& error);

		/**
		 * Sends a message using the given payload and opcode to the specified connection
		 * @param connection the server connection
		 * @param payload the message buffer
		 * @param length total number of bytes
		 * @param code message type
		 * @param error contains the error if sending fails
		 * @return if message was send successfully
		 */
		bool send(const WebSocketConnection& connection, void const* payload, int length, EWebSocketOPCode code, nap::utility::ErrorState& error);


		bool mAllowFailure = true;												///< Property: 'AllowFailure' if the client connection to the server is allowed to fail on start
		bool mLogConnectionUpdates = true;										///< Property: "LogConnectionUpdates" if client / server connection information is logged to the console.
		EWebSocketLogLevel mLibraryLogLevel = EWebSocketLogLevel::Warning;		///< Property: "LibraryLogLevel" library related equal to or higher than requested are logged.

	private:
		uint32 mLogLevel = 0;													///< Converted library log level
		uint32 mAccessLogLevel = 0;												///< Log client / server connection data
		bool mRunning = false;													///< If the client connection to the server is open						
		wspp::ClientEndPoint mEndPoint;											///< Websocketpp client end point
		std::future<void> mClientTask;											///< The client server thread
		std::vector<std::unique_ptr<WebSocketClientWrapper>> mClients;			///< All unique client connections

		/**
		 * Runs the end point in a background thread until stopped.
		 */
		void run();

		/**
		 * Connects a client to a server. The connection is managed by this endpoint.
		 * @param uri server uri.
		 * @return the newly created web-socket connection
		 */
		bool registerClient(IWebSocketClient& client, utility::ErrorState& error);

		// THIS IS AN INIT WORKAROUND: TODO: FIX ORDER OF DESTRUCTION
		void onClientDestroyed(const IWebSocketClient& client);
		nap::Slot<const IWebSocketClient&> mClientDestroyed = { this, &WebSocketClientEndPoint::onClientDestroyed };

		void removeClient(const IWebSocketClient& client);
	};


	//////////////////////////////////////////////////////////////////////////
	// WebSocketClientWrapper
	//////////////////////////////////////////////////////////////////////////

	/**
	 * Used internally by the web-socket client end-point to wrap a client resource and a connection.
	 * Connection information and messages are received from various threads. This objects
	 * ensures that new information is forwarded to the right client without locking any resources.
	 */
	class NAPAPI WebSocketClientWrapper final
	{
		friend class WebSocketClientEndPoint;
	public:
		// Destructor
		~WebSocketClientWrapper();

	private:
		/**
		 * Called when a new connection is made
		 */
		void onConnectionOpened(wspp::ConnectionHandle connection);

		/**
		 * Called when a collection is closed
		 */
		void onConnectionClosed(wspp::ConnectionHandle connection);

		/**
		 * Called when a failed connection attempt is made
		 */
		void onConnectionFailed(wspp::ConnectionHandle connection);

		/**
		 * Called when a new message is received
		 */
		void onMessageReceived(wspp::ConnectionHandle connection, wspp::MessagePtr msg);

		/**
		 * Only client end point is able to construct this object.
		 */
		WebSocketClientWrapper(IWebSocketClient& client, wspp::ClientEndPoint& endPoint, wspp::ConnectionPtr connection);

		/**
		 * Disconnects the web-socket client, ensuring no callbacks are triggered when
		 * the connection is closed.
		 * @param error contains the error if the operation fails.
		 * @return if the client disconnected correct.y.
		 */
		bool disconnect(nap::utility::ErrorState& error);

		IWebSocketClient* mResource = nullptr;
		wspp::ClientEndPoint* mEndPoint = nullptr;
		wspp::ConnectionHandle mHandle;

		std::atomic<bool> mOpen = { false };
	};
}
