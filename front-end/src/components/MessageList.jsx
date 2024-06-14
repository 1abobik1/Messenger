import React from 'react';

function formatTime(isoString) {
  const date = new Date(isoString);
  return `${String(date.getHours()).padStart(2, '0')}:${String(date.getMinutes()).padStart(2, '0')}`;
}

// messages: [{id, from, to, body, sent_at}] in chronological order.
const MessageList = ({messages, myId}) => {
  return (
    <div className="flex flex-col space-y-4 p-4">
      {messages.map((msg) => {
        const isYours = msg.from === myId;
        return (
          <div key={msg.id} className={`flex ${isYours ? 'justify-end' : 'justify-start'}`}>
            <div
              className={`max-w-xs rounded-lg p-2 ${
                isYours ? 'bg-blue-500 text-white' : 'bg-gray-300 text-gray-800'
              }`}
            >
              <p className="whitespace-pre-wrap break-words">{msg.body}</p>
              <span className="text-xs">{formatTime(msg.sent_at)}</span>
            </div>
          </div>
        );
      })}
    </div>
  );
};

export default MessageList;
