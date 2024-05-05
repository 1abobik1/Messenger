import React, {useState} from 'react';
import '../css/burger.css';
import useAuth from "../auth/useAuth";
import {Link, useNavigate} from "react-router-dom";

const BurgerMenu = ({active, setActive}) => {
  const [searchQuery, setSearchQuery] = useState('');
  const [searchResult, setSearchResult] = useState(['krasava', 'legenda krasava', 'aye', 'lox_pidor', 'legenda', 'legenda', 'legenda', 'legenda']);
  const [errorMessage, setErrorMessage] = useState('');
  const [searching, setSearching] = useState(false);
  const [searchPerformed, setSearchPerformed] = useState(false);
  const {signout} = useAuth();
  const navigate = useNavigate();
  const filteredUsers = searchResult.filter(user => {
    return user.toLowerCase().includes(searchQuery.toLowerCase())
  })

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
            setSearchResult([data.user_by_email]);
          } else {
            setSearchResult(data.user_by_name);
          }
          setErrorMessage('');
        } else {
          throw new Error(data.error || 'Failed to search users');
        }
      } else {
        throw new Error('Failed to search users');
      }
    } catch (error) {
      console.error(error);
      setSearchResult([]);
      setErrorMessage(error.message || 'No such user exists');
    } finally {
      setSearching(false);
    }
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
              placeholder="search by email or name"
              value={searchQuery}
              onChange={e => setSearchQuery(e.target.value)}
              className="border border-gray-200 rounded p-2"
            />
            <button onClick={handleSearch} className="bg-indigo-500 text-white font-semibold py-2 mt-2 rounded">
              {searching ? 'Searching...' : 'Search'}
            </button>
            <button className="bg-green-500 text-white font-semibold py-2 mt-2 rounded">
              Search Random Friend
            </button>

          </div>
          {/* List of users */}
          <div className="flex flex-col mt-8">
            <div className="flex flex-row items-center justify-between text-xs">
              <span
                className={`font-bold ${searchPerformed && searchResult.length === 0 ? 'no-users-found' : 'invisible'}`}>{searchResult.length > 0 ? `Users (${searchResult.length})` : 'No users found'}</span>
            </div>
            <div className="flex flex-col space-y-1 mt-4 mx-2 max-h-72 overflow-y-auto scrollbar">
              {filteredUsers.map((user, index) => (
                <Link key={index} to={`/client/${index}`}>
                  <button className="flex flex-row items-center hover:bg-gray-100 rounded-xl p-2">
                    <div className="flex items-center justify-center h-8 w-8 bg-indigo-200 rounded-full">
                      {user.charAt(0)}
                    </div>
                    <div className="ml-2 text-sm font-semibold">{user}</div>
                  </button>
                </Link>
              ))}
            </div>
          </div>
        </div>
        <div className="flex justify-center mb-2">
          <button className="bg-red-500 text-white font-semibold p-2 mt-2 rounded"
                  onClick={() => signout(() => navigate('/', {replace: true}))}>
            LogOut
          </button>
        </div>
      </div>
    </div>
  );
};

export default BurgerMenu;
