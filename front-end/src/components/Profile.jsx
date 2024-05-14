import React, {useEffect, useRef, useState} from 'react';
import {useNavigate} from "react-router-dom";
import useAuth from "../auth/useAuth";

const Profile = () => {
  const userEmail = localStorage.getItem('userEmail');
  const [isOpen, setIsOpen] = useState(false);
  const dropdownRef = useRef(null);
  const navigate = useNavigate();
  const {signout} = useAuth();

  const toggleDropdown = () => {
    setIsOpen(!isOpen);
  };
  useEffect(() => {
    if (isOpen) {
      function handleClickOutside(event) {
        if (dropdownRef.current && !dropdownRef.current.contains(event.target)) {
          setIsOpen(false);
        }
      }
      document.addEventListener("mousedown", handleClickOutside);
      return () => {
        document.removeEventListener("mousedown", handleClickOutside);
      };
    }
  }, [isOpen]);
  const logout = () => {
    localStorage.removeItem('userEmail');
    localStorage.removeItem('userId');
    signout(() => navigate('/', {replace: true}))
  };

  return (
    <div className="relative" ref={dropdownRef}>
      <button
        onClick={toggleDropdown}
        className="h-12 w-12 bg-purple-400 rounded-full flex items-center justify-center overflow-hidden"
      >
        {userEmail ? userEmail[0].toUpperCase() : ''}
      </button>
      {isOpen && (
        <div className="absolute right-0 mt-2 py-2 w-48 bg-white rounded-lg shadow-xl">
          <div className="block px-4 py-2 text-sm text-gray-700 hover:bg-gray-100">
            {userEmail}
          </div>
          <div className="block px-4 py-2 text-sm">
            <button
              onClick={logout}
              className="text-white bg-red-600 hover:bg-red-700 focus:outline-none focus:ring-2 focus:ring-red-500 focus:ring-opacity-50 rounded-md shadow-sm px-4 py-2"
            >
              Logout
            </button>
          </div>
        </div>
      )}
    </div>
  );
};

export default Profile;
