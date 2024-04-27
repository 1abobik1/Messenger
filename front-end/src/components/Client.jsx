import React, { useState, useEffect } from 'react';
import BurgerMenu from "./BurgerMenu";
import SendForm from "./SendForm";

const Client = () => {
    const [menuActive, setMenuActive] = useState(false);
    const [socket, setSocket] = useState(null);

    useEffect(() => {
        const ws = new WebSocket("ws://localhost:9000/");
        setSocket(ws);

        return () => {
            ws.close();
        };
    }, []);

    return (
        <div className="flex h-screen antialiased text-gray-800 w-screen">
            <div className="flex flex-row h-full w-full overflow-x-hidden bg-gray-100">
                <button className="burger m-5 h-1 w-1 z-10" onClick={() => setMenuActive(!menuActive)}>
                    <svg xmlns="http://www.w3.org/2000/svg" x="0px" y="0px" width="30" height="30" viewBox="0 0 50 50">
                        <path d="M 0 9 L 0 11 L 50 11 L 50 9 Z M 0 24 L 0 26 L 50 26 L 50 24 Z M 0 39 L 0 41 L 50 41 L 50 39 Z"></path>
                    </svg>
                </button>
                <BurgerMenu active={menuActive} setActive={setMenuActive} />
                {socket && <SendForm active={menuActive} setActive={setMenuActive} socket={socket} />}
            </div>
        </div>
    );
};

export default Client;