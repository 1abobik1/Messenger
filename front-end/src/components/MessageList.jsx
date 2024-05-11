import React from 'react';

const MessageList = ({yourMessages, otherMessages}) => {

  function extractHoursAndMinutes(timestamp) {
    const date = new Date(timestamp);

    const hours = date.getHours().toString().padStart(2,"0");
    const minutes = date.getMinutes().toString().padStart(2,"0");

    return {hours, minutes};
  }

  const combineAndSortMessages = () => {
    const allMessages = [...yourMessages, ...otherMessages];
    return allMessages.sort((a, b) => new Date(a.timestamp) - new Date(b.timestamp));
  };

  const sortedMessages = combineAndSortMessages();

  return (
    <div className="flex flex-col space-y-4 p-4">
      {sortedMessages.map((msg, index) => (
        <div
          key={index}
          className={`flex ${
            msg.isYours ? 'justify-end' : 'justify-start'
          }`}
        >
          <div
            className={`max-w-xs rounded-lg p-2 ${
              msg.isYours
                ? 'bg-blue-500 text-white'
                : 'bg-gray-300 text-gray-800'
            }`}
          >
            <p>{msg.content}</p>
            <span
              className="text-xs">{extractHoursAndMinutes(msg.timestamp).hours + ':' + extractHoursAndMinutes(msg.timestamp).minutes}</span>
          </div>
        </div>
      ))}
    </div>
  );
};

export default MessageList;
