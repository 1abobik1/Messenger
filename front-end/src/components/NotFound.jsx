import React from 'react';
import {Link} from "react-router-dom";

const NotFound = () => {
  return (
    <div className='flex flex-wrap bg-center flex-col w-screen h-screen content-center justify-center'>
      <h1 className='font-bold text-3xl'>Not found</h1>
      <Link to='/' className='text-blue-400 mt-2'>To home</Link>
    </div>
  );
};

export default NotFound;
