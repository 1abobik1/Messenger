import React, { useEffect, useState, useCallback } from 'react';
import '../css/form.css';
import MessageList from "./MessageList";

const SendForm = ({ active, setActive, socket, receiverId }) => {
    const [message, setMessage] = useState('');
    const [allMessages, setAllMessages] = useState([]);
    const [yourMessages, setYourMessages] = useState([]);
    const [otherMessages, setOtherMessages] = useState([]);
    const yourUserId = localStorage.getItem('userId');

    function parseDate(dateString) {
        const [datePart] = dateString.split('.');
        const date = new Date(datePart);
        return Math.floor(date.getTime());
    }

    const fetchAllMessages = useCallback(async () => {
        try {
            const requestBody = {
                sender_id: parseInt(yourUserId),
                receiver_id: receiverId
            };

            const response = await fetch('http://localhost:9000/client/PrintClientsMessages', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(requestBody)
            });

            if (response.ok) {
                const data = await response.json();
                setAllMessages(data);
            }
        } catch (error) {
            console.error(error);
        }
    }, [yourUserId, receiverId]);


    useEffect(() => {
        if (receiverId && yourUserId) {
            fetchAllMessages();
        }
    }, [fetchAllMessages, receiverId, yourUserId]);

    useEffect(() => {
        socket.onmessage = (event) => {
            const data = JSON.parse(event.data);
            setOtherMessages(prevMessages => [...prevMessages, {
                content: data.message,
                timestamp: parseDate(data.sent_at),
                isYours: false
            }]);
        };

        return () => {
            socket.onmessage = null;
        };
    }, [socket, receiverId]);

    function getCurrentTime() {
        return Date.now();
    }

    function sendMessage() {
        if (message.trim() !== '') {
            const newMessage = { content: message, timestamp: getCurrentTime(), isYours: true };
            socket.send(JSON.stringify({ "command": "private_msg", "receiver_id": receiverId, "message": message }));
            setYourMessages([...yourMessages, newMessage]);
            setMessage('');
        }
    }

    const handleKeyPress = (event) => {
        if (event.key === 'Enter' && event.ctrlKey) {
            sendMessage();
        }
    }

    useEffect(() => {
        // Группировка сообщений на основе того, чей это ID (отправителя или получателя)
        const yourMsgs = allMessages.filter(msg => msg.sender_id.toString() === yourUserId);
        const otherMsgs = allMessages.filter(msg => msg.receiver_id.toString() === yourUserId);

        // Форматирование сообщений для отображения в интерфейсе
        const formattedYourMsgs = yourMsgs.map(msg => ({
            content: msg.message_text,
            timestamp: parseDate(msg.sent_at),
            isYours: true
        }));

        const formattedOtherMsgs = otherMsgs.map(msg => ({
            content: msg.message_text,
            timestamp: parseDate(msg.sent_at),
            isYours: false
        }));

        // Сортировка сообщений по времени отправки
        const combinedMessages = [...formattedYourMsgs, ...formattedOtherMsgs];
        combinedMessages.sort((a, b) => a.timestamp - b.timestamp);

        setYourMessages(combinedMessages.filter(msg => msg.isYours));
        setOtherMessages(combinedMessages.filter(msg => !msg.isYours));
    }, [allMessages, yourUserId]);


  return (
    <div className={active ? 'flex flex-col flex-auto h-screen form active pl-4' : 'flex flex-col flex-auto h-screen form pl-4'}>
      <div className="flex flex-cnpm startol flex-auto flex-shrink-0  bg-gray-100 h-full p-4">
        <div className="sendhund">
          <div className="flex flex-col h-full overflow-y-scroll">
            <MessageList yourMessages={yourMessages} otherMessages={otherMessages}/>
          </div>
          <div className="flex flex-row items-center h-16 rounded-xl bg-white w-full px-4 input">
            <div className="flex-grow ml-4">
              <div className="relative w-full">
                                <textarea
                                  className="flex w-full border rounded-xl focus:outline-none focus:border-indigo-300 pl-4 h-10 messageInput"
                                  onChange={(event) => setMessage(event.target.value)}
                                  onKeyUp={handleKeyPress}
                                  value={message}
                                  id="messageInput"
                                  maxLength={3500}
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
