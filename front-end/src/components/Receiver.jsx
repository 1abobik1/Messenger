import React from 'react';

const Receiver = ({selectedUserName, receiverId}) => {
  return (
    <div className='bg-gray-200 w-full rounded-lg'>
      <p className='p-2 font-bold'>{selectedUserName}{` (id - ${receiverId})`}</p>
    </div>
  );
};

export default Receiver;
