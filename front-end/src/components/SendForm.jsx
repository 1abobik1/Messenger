import React, { useState, useEffect } from 'react';
import '../css/form.css';

const SendForm = ({ active, setActive, socket, receiverId }) => {
    const [message, setMessage] = useState('');
    const [messages, setMessages] = useState([]);

  useEffect(() => {
    socket.onmessage = (event) => {
      const data = JSON.parse(event.data);
      if (data.command === "private_msg") {
        setMessages(prevMessages => [...prevMessages, { content: data.message, timestamp: data.sent_at }]);
      }
    };

    return () => {
      socket.onmessage = null;
    };
  }, [socket, receiverId]);

  function getCurrentTime() {
        const date = new Date();
        const hours = date.getHours().toString().padStart(2, '0');
        const minutes = date.getMinutes().toString().padStart(2, '0');
        return `${hours}:${minutes}`;
    }

    function sendMessage() {
        if (message.trim() !== '') {
            const newMessage = { content: message, timestamp: getCurrentTime() };
            socket.send(JSON.stringify({ "command": "private_msg", "receiver_id": receiverId, "message": message }));
            setMessages([...messages, newMessage]);
            setMessage('');
        }
    }

    return (
        <div
            className={active ? 'flex flex-col flex-auto h-screen form active pl-4' : 'flex flex-col flex-auto h-screen form pl-4'}>
            <div className="flex flex-cnpm startol flex-auto flex-shrink-0  bg-gray-100 h-full p-4">
                <div className="sendhund">
                    <div className="flex flex-col h-full">
                        {messages.map((msg, index) => (
                            <div key={index} className="mt-2 text-gray-600"> {msg.content} {msg.timestamp}</div>
                        ))}
                        <div className="grid grid-cols-12 gap-y-2">
                        </div>
                    </div>
                    <div className="flex flex-row items-center h-16 rounded-xl bg-white w-full px-4 input">
                        <div className="flex-grow ml-4">
                            <div className="relative w-full">
                                <textarea
                                    className="flex w-full border rounded-xl focus:outline-none focus:border-indigo-300 pl-4 h-10 messageInput"
                                    onChange={(event) => setMessage(event.target.value)}
                                    value={message}
                                    id="messageInput" />
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
