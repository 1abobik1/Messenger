import React from 'react';
import '../css/whotowrite.css'

const WhoToWrite = ({active, setActive}) => {
  return (
    <div className={active ? 'flex flex-wrap justify-center w-screen message' : 'flex flex-wrap justify-center w-screen message active'}>
        <span className='text'>Choose who to write to</span>
    </div>
  );
};

export default WhoToWrite;
