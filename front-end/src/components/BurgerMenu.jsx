import React, {useState} from 'react';
import '../css/burger.css';
import useAuth from "../auth/useAuth";
import {FaPlus, FaMinus, FaCheck} from 'react-icons/fa';
import {useNavigate} from "react-router-dom";
import {allowedRoutes} from "./URLGuard";

const BurgerMenu = ({active, setActive}) => {
  const [searchQuery, setSearchQuery] = useState('');
  const [searchResult, setSearchResult] = useState({user_by_email: null, user_id: null});
  const [errorMessage, setErrorMessage] = useState('');
  const [searching, setSearching] = useState(false);
  const [searchPerformed, setSearchPerformed] = useState(false);
  const [selectedUser, setSelectedUser] = useState(null);
  const [friends, setFriends] = useState([]);
  const userEmail = localStorage.getItem('userEmail');
  const [addedFriends, setAddedFriends] = useState([]);
  const [showFriends, setShowFriends] = useState(true);
  const navigate = useNavigate();

  const handleSearch = async () => {
    try {
      if (!searchQuery.trim()) {
        return;
      }
      setSearchPerformed(true);
      setSearching(true);
      const response = await fetch('http://localhost:9000/client/SearchUser', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({query: searchQuery})
      });
      if (response.ok) {
        const data = await response.json();
        if (!data.error) {
          if (data.user_by_email) {
            setSearchResult({user_by_email: data.user_by_email, user_id: data.user_id});
          }
          setErrorMessage('');
        } else {
          throw new Error(data.error, 'Failed to search users');
        }
      } else {
        throw new Error('Failed to search users');
      }
    } catch (error) {
      console.error(error);
      setSearchResult({user_by_email: null, user_id: null});
      setErrorMessage(error.message + ' No such user exists');
    } finally {
      setSearching(false);
    }
  };

  const handleSearchFriends = async () => {
    try {
      const response = await fetch('http://localhost:9000/client/SearchUserFriends', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({user_email: userEmail})
      });
      if (response.ok) {
        const data = await response.json();
        if (!data.error) {
          setFriends(data.friends);
        }
      }
    } catch (error) {
      console.error(error);
    }
  };
  const handleAddFriend = async (e) => {
    e.preventDefault();
    try {
      if (addedFriends.includes(searchResult.user_id)) {
        return;
      }
      const response = await fetch('http://localhost:9000/client/AddFriends', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({user_email: userEmail, new_friend_id: searchResult.user_id})
      });
      if (response.ok) {
        setAddedFriends(prevFriends => [...prevFriends, searchResult.user_id]);
      }
    } catch (error) {
      console.error(error);
    }
  };

  const handleFriendsClick = () => {
    handleSearchFriends();
  };

  const toggleShowFriends = () => {
    setShowFriends(prevState => !prevState);
  };
  const handleClientIdClick = (userId) => {
    allowedRoutes.add(`/client/${userId}`);
    navigate(`${userId}`);
  };
  return (
    <div className={active ? 'menu active h-screen' : 'menu h-screen'}>
      <div className="flex justify-between h-screen flex-col py-8 pl-6 pr-2 w-64 bg-white flex-shrink-0">
        <div>
          <div className="flex flex-row items-center justify-center h-12 w-full">
            <div className="ml-2 font-bold text-2xl">BeBroVChat</div>
          </div>
          {/* Search input */}
          <div className="flex flex-col mt-4">
            <input
              type="text"
              placeholder="search by email"
              value={searchQuery}
              onChange={e => setSearchQuery(e.target.value)}
              className="border border-gray-200 rounded p-2"
            />
            <button onClick={handleSearch} className="bg-indigo-500 text-white font-semibold py-2 mt-2 rounded">
              {searching ? 'Searching...' : 'Search'}
            </button>
            <button onClick={handleFriendsClick} className="bg-blue-600 text-white font-semibold py-1 mt-1 rounded">
              Friends
            </button>
          </div>
          {/* List of users */}
          <div className="flex flex-col mt-8">
            <div className="flex flex-row items-center justify-between text-xs">
                            <span
                              className={`font-bold ${searchPerformed && searchResult.user_by_email === null ? 'no-users-found' : 'invisible'}`}
                            >
                                {searchResult.length > 0 ? `Users (${searchResult.length})` : 'No users found'}
                            </span>
            </div>
            <div className="flex flex-col space-y-1 mt-4 -mx-2 min-h-10 overflow-y-auto">
              {searchResult.user_by_email && (
                <div className="flex items-center justify-between hover:bg-gray-100 rounded-xl p-2 cursor-pointer"
                     onClick={() => handleClientIdClick(searchResult.user_id)}>
                  <div className="flex items-center">
                    <div className="flex items-center justify-center h-8 w-8 bg-indigo-200 rounded-full">
                      {searchResult.user_by_email.charAt(0)}
                    </div>
                    <div className="ml-2 text-sm font-semibold">
                      {searchResult.user_by_email} (id-{searchResult.user_id})
                    </div>
                  </div>
                  <button
                    onClick={
                      friends.some(friend => friend.friend_id === searchResult.user_id)
                        ? () => {}
                        : handleAddFriend
                    }
                    className={
                      friends.some(friend => friend.friend_id === searchResult.user_id)
                        ? "ml-2 text-green-500 rounded-full p-1"
                        : "ml-2 text-green-500 hover:bg-green-200 rounded-full p-1"
                    }
                  >
                    {friends.some(friend => friend.friend_id === searchResult.user_id)
                      ? <FaCheck/>
                      : <FaPlus/>}
                  </button>

                </div>
              )}
            </div>
          </div>
          {/* List of friends */}
          <div className="flex flex-col mt-8">
            <div className="flex flex-row items-center justify-between text-xs">
              <span className="font-bold text-lg text-gray-800">Friends</span>
              {/* ������ ��� ������������ ����������� ������ ������ */}
              <button onClick={toggleShowFriends} className="focus:outline-none">
                {showFriends ? <FaMinus/> : <FaPlus/>}
              </button>
            </div>
            {/* �������� ����������� ������ ������ */}
            {showFriends && (
              <div className="flex flex-col space-y-1 mt-4 -mx-2 overflow-y-auto max-h-100">
                {friends.map((friend, index) => (
                  <div className="flex items-center justify-between hover:bg-gray-100 rounded-xl p-2 cursor-pointer"
                       onClick={() => handleClientIdClick(friend.friend_id)} key={index}>
                    <div className="flex items-center">
                      <div className="flex items-center justify-center h-8 w-8 bg-indigo-200 rounded-full">
                        {friend.friend_name.charAt(0)}
                      </div>
                      <div className="ml-2 text-sm font-semibold">
                        {friend.friend_name} (id-{friend.friend_id})
                      </div>
                    </div>
                  </div>
                ))}
              </div>
            )}
          </div>
        </div>
      </div>
    </div>
  );
};

export default BurgerMenu;
