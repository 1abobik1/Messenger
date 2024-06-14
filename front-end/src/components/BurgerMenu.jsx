import React, {useCallback, useEffect, useState} from 'react';
import '../css/burger.css';
import useAuth from '../auth/useAuth';
import {FaPlus, FaMinus, FaCheck} from 'react-icons/fa';
import {useNavigate} from 'react-router-dom';

const BurgerMenu = ({active}) => {
  const [searchQuery, setSearchQuery] = useState('');
  const [searchResult, setSearchResult] = useState(null); // {id, username}
  const [errorMessage, setErrorMessage] = useState('');
  const [searching, setSearching] = useState(false);
  const [friends, setFriends] = useState([]); // [{id, username}]
  const [showFriends, setShowFriends] = useState(true);
  const navigate = useNavigate();
  const {request} = useAuth();

  const loadFriends = useCallback(async () => {
    try {
      setFriends(await request('/api/friends'));
    } catch (error) {
      console.error(error);
    }
  }, [request]);

  useEffect(() => {
    loadFriends();
  }, [loadFriends]);

  const handleSearch = async () => {
    const query = searchQuery.trim();
    if (!query) {
      return;
    }
    setSearching(true);
    try {
      setSearchResult(await request(`/api/users/search?email=${encodeURIComponent(query)}`));
      setErrorMessage('');
    } catch (error) {
      setSearchResult(null);
      setErrorMessage(error.status === 404 ? 'No such user exists' : error.message);
    } finally {
      setSearching(false);
    }
  };

  const handleAddFriend = async (friendId) => {
    try {
      await request('/api/friends', {method: 'POST', body: {friend_id: friendId}});
      await loadFriends();
    } catch (error) {
      setErrorMessage(error.message);
    }
  };

  const isFriend = (userId) => friends.some((friend) => friend.id === userId);

  const toggleShowFriends = () => {
    setShowFriends((prevState) => !prevState);
  };

  const openChat = (userId, event) => {
    event.stopPropagation();
    navigate(`/client/${userId}`);
  };

  return (
    <div className={active ? 'menu active h-screen' : 'menu h-screen'}>
      <div className="flex justify-between h-screen flex-col py-8 pl-6 pr-6 w-64 bg-white flex-shrink-0">
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
              onChange={(e) => setSearchQuery(e.target.value)}
              className="border border-gray-200 rounded p-2"
            />
            <button
              onClick={handleSearch}
              className="bg-indigo-500 text-white font-semibold py-2 mt-2 rounded hover:bg-indigo-400"
            >
              {searching ? 'Searching...' : 'Search'}
            </button>
          </div>
          {/* Search result */}
          <div className="flex flex-col mt-8">
            <div className="flex flex-row items-center justify-between text-xs">
              <span className={`font-bold ${errorMessage ? 'no-users-found' : 'invisible'}`}>
                {errorMessage || 'No users found'}
              </span>
            </div>
            <div className="flex flex-col space-y-1 mt-4 -mx-2 min-h-10 overflow-y-auto">
              {searchResult && (
                <div
                  className="flex items-center justify-between hover:bg-gray-100 rounded-xl p-2 cursor-pointer"
                  onClick={(event) => openChat(searchResult.id, event)}
                >
                  <div className="flex items-center">
                    <div className="flex items-center justify-center h-8 w-8 bg-indigo-200 rounded-full">
                      {searchResult.username.charAt(0)}
                    </div>
                    <div className="ml-2 text-sm font-semibold">
                      {searchResult.username} (id-{searchResult.id})
                    </div>
                  </div>
                  <button
                    onClick={(event) => {
                      event.stopPropagation();
                      if (!isFriend(searchResult.id)) {
                        handleAddFriend(searchResult.id);
                      }
                    }}
                    className={
                      isFriend(searchResult.id)
                        ? 'ml-2 text-green-500 rounded-full p-1'
                        : 'ml-2 text-green-500 hover:bg-green-200 rounded-full p-1'
                    }
                  >
                    {isFriend(searchResult.id) ? <FaCheck /> : <FaPlus />}
                  </button>
                </div>
              )}
            </div>
          </div>
          {/* List of friends */}
          <div className="flex flex-col mt-8">
            <div className="flex flex-row items-center justify-between text-xs">
              <span className="font-bold text-lg text-gray-800">Friends</span>
              <button onClick={toggleShowFriends} className="focus:outline-none">
                {showFriends ? <FaMinus /> : <FaPlus />}
              </button>
            </div>
            {showFriends && (
              <div className="flex flex-col space-y-1 mt-4 -mx-2 overflow-y-auto max-h-100">
                {friends.map((friend) => (
                  <div
                    className="flex items-center justify-between hover:bg-gray-100 rounded-xl p-2 cursor-pointer"
                    onClick={(event) => openChat(friend.id, event)}
                    key={friend.id}
                  >
                    <div className="flex items-center">
                      <div className="flex items-center justify-center h-8 w-8 bg-indigo-200 rounded-full">
                        {friend.username.charAt(0)}
                      </div>
                      <div className="ml-2 text-sm font-semibold">
                        {friend.username} (id-{friend.id})
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
