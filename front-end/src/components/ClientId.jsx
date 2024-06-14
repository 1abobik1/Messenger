import React, {useEffect, useState} from 'react';
import SendForm from "./SendForm";
import {Link, useOutletContext} from "react-router-dom";
import LoadingSpinner from "./LoadingSpinner";
import useAuth from "../auth/useAuth";

const ClientId = () => {
  const {menuActive, id, socket} = useOutletContext();
  const {request} = useAuth();
  const receiverId = Number(id);
  const [receiver, setReceiver] = useState(null); // {id, username}
  const [notFound, setNotFound] = useState(false);

  // The chat is opened by URL, so the peer's name comes from the API, not from the menu state.
  useEffect(() => {
    let cancelled = false;
    setReceiver(null);
    setNotFound(false);
    request(`/api/users/${receiverId}`)
      .then(user => !cancelled && setReceiver(user))
      .catch(() => !cancelled && setNotFound(true));
    return () => {
      cancelled = true;
    };
  }, [receiverId, request]);

  if (notFound) {
    return (
      <div className='flex flex-col w-screen h-screen items-center justify-center'>
        <p className='font-bold text-xl'>User not found</p>
        <Link to='/client' className='text-blue-400 mt-2'>Back</Link>
      </div>
    );
  }

  return (
    <div className='w-screen'>
      {socket && receiver ? (
        <SendForm active={menuActive} socket={socket} receiver={receiver}/>
      ) : (
        <div className='flex h-screen justify-center w-full'><LoadingSpinner/></div>
      )}
    </div>
  );
};

export default ClientId;
