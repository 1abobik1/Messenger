import React, {useEffect, useState, useCallback} from 'react';
import '../css/form.css';
import MessageList from "./MessageList";
import Receiver from "./Receiver";
import useAuth from "../auth/useAuth";

const MAX_MESSAGE_LENGTH = 4000;

const SendForm = ({active, socket, receiver}) => {
  const receiverId = receiver.id;
  const [message, setMessage] = useState('');
  const [messages, setMessages] = useState([]); // [{id, from, to, body, sent_at}], oldest first
  const [error, setError] = useState('');
  const {user, request} = useAuth();
  const myId = user.id;

  // Adds messages that are not on screen yet; the server may deliver the same message twice (history + socket).
  const addMessages = useCallback((incoming) => {
    setMessages(prev => {
      const known = new Set(prev.map(msg => msg.id));
      const fresh = incoming.filter(msg => !known.has(msg.id));
      return fresh.length ? [...prev, ...fresh].sort((a, b) => a.id - b.id) : prev;
    });
  }, []);

  useEffect(() => {
    let cancelled = false;
    setMessages([]);
    request(`/api/messages/${receiverId}?limit=50`)
      .then(history => {
        if (!cancelled) {
          addMessages(history);
        }
      })
      .catch(err => setError(err.message));
    return () => {
      cancelled = true;
    };
  }, [receiverId, request, addMessages]);

  useEffect(() => {
    socket.onmessage = (event) => {
      const data = JSON.parse(event.data);
      if (data.type === 'message') {
        const msg = data.message;
        // Only messages of this conversation: a message from someone else must not appear here.
        const inThisChat = (msg.from === receiverId && msg.to === myId) || (msg.from === myId && msg.to === receiverId);
        if (inThisChat) {
          addMessages([msg]);
        }
      } else if (data.type === 'error') {
        setError(data.code === 'rate_limited'
          ? `Too many messages. Try again in ${data.retry_after} s.`
          : data.message);
      }
    };
    return () => {
      socket.onmessage = null;
    };
  }, [socket, receiverId, myId, addMessages]);

  function sendMessage() {
    if (message.trim() !== '') {
      // Shown when the server echoes it back with its id and time.
      socket.send(JSON.stringify({type: 'message', to: receiverId, body: message}));
      setMessage('');
      setError('');
    }
  }

  const handleKeyPress = (event) => {
    if (event.key === 'Enter' && event.ctrlKey) {
      sendMessage();
    }
  };

  return (
    <div
      className={active ? 'flex flex-col flex-auto h-screen form active pl-4' : 'flex flex-col flex-auto h-screen form pl-4'}>
      <div className="flex flex-auto flex-shrink-0 bg-gray-100 h-full p-4">
        <div className="sendhund">
          <Receiver receiver={receiver}/>
          <div className="flex flex-col h-full overflow-y-scroll">
            <MessageList messages={messages} myId={myId}/>
          </div>
          {error && <div className="text-sm text-red-600 px-4 py-1">{error}</div>}
          <div className="flex flex-row items-center h-16 rounded-xl bg-white w-full px-4 input">
            <div className="flex-grow ml-4">
              <div className="relative w-full">
                                <textarea
                                  className="flex w-full border rounded-xl focus:outline-none focus:border-indigo-300 pl-4 h-10 messageInput"
                                  onChange={(event) => setMessage(event.target.value)}
                                  onKeyUp={handleKeyPress}
                                  value={message}
                                  id="messageInput"
                                  maxLength={MAX_MESSAGE_LENGTH}
                                  autoFocus={true}
                                  placeholder={`Message`}/>
              </div>
            </div>
            <div className="ml-4">
              <button
                className="flex items-center justify-center bg-indigo-500 hover:bg-indigo-600 rounded-xl text-white px-4 py-1 flex-shrink-0 send"
                onClick={sendMessage}
                disabled={!message.trim()}
              >
                <span>Send</span>
                <span className="ml-2">
                                    <svg className="w-4 h-4 transform rotate-45 -mt-px"
                                         fill="none"
                                         stroke="currentColor"
                                         viewBox="0 0 24 24"
                                         xmlns="http://www.w3.org/2000/svg">
                                        <path strokeLinecap="round"
                                              strokeLinejoin="round"
                                              strokeWidth="2"
                                              d="M12 19l9 2-9-18-9 18 9-2zm0 0v-8"></path>
                                    </svg>
                                </span>
              </button>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
};

export default SendForm;
