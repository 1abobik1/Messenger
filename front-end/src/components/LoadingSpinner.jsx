import React from 'react';
import '../css/spinner.css'

const LoadingSpinner = ({active, setActive}) => {
  return (
    <div className={`h-screen flex justify-center items-center spinner ${active ? '' : 'active'}`}>
      <div className="animate-spin rounded-full h-16 w-16 border-b-4 border-blue-600"></div>
    </div>
  );
};

export default LoadingSpinner;
