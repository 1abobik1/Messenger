import React from 'react';
import SendForm from "./SendForm";
import {useOutletContext} from "react-router-dom";
import LoadingSpinner from "./LoadingSpinner";

const ClientId = () => {
  const {menuActive, setMenuActive, id, socket, isSocketReady, selectedUserName} = useOutletContext();
  const receiverId = parseInt(id);

  return (
    <div className='w-screen'>
      {isSocketReady ? (
        socket ? <SendForm
          active={menuActive}
          setActive={setMenuActive}
          socket={socket}
          receiverId={receiverId}
          selectedUserName={selectedUserName}
        /> : <div>Error: Socket not available</div>
      ) : (
        <div className='flex h-screen justify-center w-full'><LoadingSpinner/></div>
      )}
    </div>
  );
};

export default ClientId;
