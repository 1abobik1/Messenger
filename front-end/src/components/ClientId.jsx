import React from 'react';
import SendForm from "./SendForm";
import {useOutletContext} from "react-router-dom";

const ClientId = () => {
    const { menuActive, setMenuActive, id, socket } = useOutletContext();
    const receiverId = parseInt(id);
    return (
        <SendForm active={menuActive} setActive={setMenuActive} socket={socket} receiverId={receiverId} />
    );
};

export default ClientId;
