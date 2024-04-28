import React, { useState } from 'react';
import '../css/burger.css';

const BurgerMenu = ({ active, setActive }) => {
    const [searchQuery, setSearchQuery] = useState('');
    const [searchResult, setSearchResult] = useState([]);
    const [errorMessage, setErrorMessage] = useState('');
    const [searching, setSearching] = useState(false);

    const handleSearch = async () => {
        try {
            setSearching(true);
            const response = await fetch('http://localhost:9000/client/SearchUser', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({ email: searchQuery })
            });
            if (response.ok) {
                const data = await response.json();
                if (!data.error) {
                    setSearchResult([data.username]); 
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

    const handleDisplayUsers = async () => {
        try {
            setSearching(true);
            const response = await fetch('http://localhost:9000/client/DisplayUsers', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({})
            });
            if (response.ok) {
                const data = await response.json();
                setSearchResult(data || []);
                setErrorMessage('');
            } else {
                throw new Error('Failed to fetch random friends');
            }
        } catch (error) {
            console.error(error);
            setSearchResult([]);
            setErrorMessage(error.message || 'Failed to fetch random friends');
        } finally {
            setSearching(false);
        }
    };

    return (
        <div className={active ? 'menu active h-screen' : 'menu h-screen'}>
            <div className="flex flex-col py-8 pl-6 pr-2 w-64 bg-white flex-shrink-0 ">
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
                </div>
                {/* List of users */}
                <div className="flex flex-col mt-8">
                    <div className="flex flex-row items-center justify-between text-xs">
                        <span className="font-bold">{searchResult.length > 0 ? `Users (${searchResult.length})` : 'No users found'}</span>
                    </div>
                    <div className="flex flex-col space-y-1 mt-4 -mx-2 min-h-10 overflow-y-auto">
                        {searchResult.map((user, index) => (
                            <button
                                key={index}
                                className="flex flex-row items-center hover:bg-gray-100 rounded-xl p-2"
                            >
                                <div className="flex items-center justify-center h-8 w-8 bg-indigo-200 rounded-full">
                                    {user.charAt(0)}
                                </div>
                                <div className="ml-2 text-sm font-semibold">{user}</div>
                            </button>
                        ))}
                    </div>
                </div>
                {/* Find random friends button */}
                <div className="flex justify-center mt-4">
                    <button onClick={handleDisplayUsers} className="bg-green-500 text-white font-semibold py-2 mt-2 rounded">
                        {searching ? 'Fetching...' : 'Find Random Friends'}
                    </button>
                </div>
            </div>
        </div>
    );
};

export default BurgerMenu;
