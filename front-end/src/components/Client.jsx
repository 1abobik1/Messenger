import React, {useState} from 'react';
import BurgerMenu from "./BurgerMenu";
import WhoToWrite from "./WhoToWrite";
import {Outlet, useParams} from "react-router-dom";
import Profile from "./Profile";
import useAuth from "../auth/useAuth";
import useChatSocket from "../hooks/useChatSocket";

const Client = () => {
  const [menuActive, setMenuActive] = useState(true);
  const {id} = useParams();
  const {token} = useAuth();
  const socket = useChatSocket(token);

  return (
    <div className="flex h-screen antialiased text-gray-800 w-screen">
      <div className="flex flex-row h-full w-full overflow-x-hidden bg-gray-100">
        <button className="burger m-5 h-1 w-1 z-10" onClick={() => setMenuActive(!menuActive)} aria-label="Toggle menu">
          <svg xmlns="http://www.w3.org/2000/svg" x="0px" y="0px" width="30" height="30" viewBox="0 0 50 50">
            <path
              d="M 0 9 L 0 11 L 50 11 L 50 9 Z M 0 24 L 0 26 L 50 26 L 50 24 Z M 0 39 L 0 41 L 50 41 L 50 39 Z"></path>
          </svg>
        </button>
        <div className='absolute z-20 right-0 m-2'><Profile/></div>
        <BurgerMenu active={menuActive}/>
        {id === undefined
          ? <WhoToWrite active={menuActive}/>
          : <Outlet context={{menuActive, id, socket}}/>}
      </div>
    </div>
  );
};

export default Client;
