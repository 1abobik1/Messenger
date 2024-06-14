import React from 'react';

const Receiver = ({receiver}) => {
  return (
    <div className="bg-gray-200 w-full rounded-lg">
      <p className="p-2 font-bold">
        {receiver.username} (id {receiver.id})
      </p>
    </div>
  );
};

export default Receiver;
