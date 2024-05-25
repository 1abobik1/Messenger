import React, {useEffect, useState, useRef} from 'react';
import BurgerMenu from "./BurgerMenu";
import WhoToWrite from "./WhoToWrite";
import {Outlet, useParams} from "react-router-dom";
import {SocketContext} from "./SocketContext";
import Profile from "./Profile";

const Client = () => {
  const [menuActive, setMenuActive] = useState(true);
  const socketRef = useRef(null);
  const [isSocketReady, setIsSocketReady] = useState(false);
  const [selectedUserName, setSelectedUserName] = useState('');
  const {id} = useParams();

  useEffect(() => {
    if (socketRef.current) {
      socketRef.current.close();
    }

    socketRef.current = new WebSocket('ws://localhost:9000/');
    socketRef.current.onopen = () => {
      setIsSocketReady(true);
    };
    socketRef.current.onclose = () => {
      setIsSocketReady(false);
    };

    return () => {
      if (socketRef.current) {
        socketRef.current.close();
      }
    };
  }, []);


  return (
    <SocketContext.Provider value={socketRef.current}>
      <div className="flex h-screen antialiased text-gray-800 w-screen">
        <div className="flex flex-row h-full w-full overflow-x-hidden bg-gray-100">
          <button className="burger m-5 h-1 w-1 z-10" onClick={() => setMenuActive(!menuActive)}>
            <svg xmlns="http://www.w3.org/2000/svg" x="0px" y="0px" width="30" height="30" viewBox="0 0 50 50">
              <path
                d="M 0 9 L 0 11 L 50 11 L 50 9 Z M 0 24 L 0 26 L 50 26 L 50 24 Z M 0 39 L 0 41 L 50 41 L 50 39 Z"></path>
            </svg>
          </button>
          <div className='absolute z-20 right-0 m-2'><Profile/></div>
          <BurgerMenu active={menuActive} setActive={setMenuActive} setSelectedUserName={setSelectedUserName}/>
          {id === undefined ?
            <WhoToWrite active={menuActive} setActive={setMenuActive}/>
            : <Outlet
              context={{menuActive, setMenuActive, id, socket: socketRef.current, isSocketReady, selectedUserName}}/>
          }
        </div>
      </div>
    </SocketContext.Provider>
  );
};

export default Client;
