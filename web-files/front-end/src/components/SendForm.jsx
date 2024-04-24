import React from 'react';
import '../css/form.css'

const SendForm = ({active, setActive}) => {
  const socket = new WebSocket("ws://localhost:9000/");

  socket.onmessage = function(event) {
    const message = JSON.parse(event.data);
    document.getElementById("messages").innerHTML += `<p>${message.user_id_from}: ${message.message}</p>`;

  };

  function sendMessage() {
    const messageInput = document.getElementById("messageInput");
    const message = messageInput.value;
    socket.send(JSON.stringify({ command: "public_msg", message: message }));
    messageInput.value = "";
  }

  return (
      <div className={active ? 'flex flex-col flex-auto h-screen form active pl-4' : 'flex flex-col flex-auto h-screen form pl-4'}>
        <div className="flex flex-cnpm startol flex-auto flex-shrink-0  bg-gray-100 h-full p-4">
          <div className="flex flex-col h-full overflow-x-auto mb-4">
            <div className="flex flex-col h-full">
              <div className="grid grid-cols-12 gap-y-2">
              </div>
            </div>
            <div className="flex flex-row items-center h-16 rounded-xl bg-white w-full px-4 input">
              <div className="flex-grow ml-4">
                <div className="relative w-full">
                  <textarea
                         className="flex w-full border rounded-xl focus:outline-none focus:border-indigo-300 pl-4 h-10" id="messageInput"/>

                </div>
              </div>
              <div className="ml-4">
                <button className="flex items-center justify-center bg-indigo-500 hover:bg-indigo-600 rounded-xl text-white px-4 py-1 flex-shrink-0 send" onClick={sendMessage}>
                  <span>Send</span>
                  <span className="ml-2">
                                <svg className="w-4 h-4 transform rotate-45 -mt-px"
                                     fill="none"
                                     stroke="currentColor"
                                     viewBox="0 0 24 24"
                                     xmlns="http://www.w3.org/2000/svg">
                                    <path stroke-linecap="round"
                                          stroke-linejoin="round"
                                          stroke-width="2"
                                          d="M12 19l9 2-9-18-9 18 9-2zm0 0v-8"></path>
                                </svg>
                            </span>
                </button>
              </div>
            </div>
          </div>
        </div>
      </div>
  );
};

export default SendForm;
