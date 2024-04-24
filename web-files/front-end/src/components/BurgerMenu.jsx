import React from 'react';
import '../css/burger.css'

const BurgerMenu = ({active, setActive}) => {
  return (
    <div className={active ? 'menu active h-screen': 'menu h-screen'}>
      <div className="flex flex-col py-8 pl-6 pr-2 w-64 bg-white flex-shrink-0 ">
        <div className="flex flex-row items-center justify-center h-12 w-full">
          <div className="ml-2 font-bold text-2xl">Chat</div>
        </div>
        <div className="flex flex-col items-center bg-indigo-100 border border-gray-200 mt-4 w-full py-6 px-4 rounded-lg">
          <div className="text-sm font-semibold mt-2">MaName</div>
        </div>
        <div className="flex flex-col mt-8">
          <div className="flex flex-row items-center justify-between text-xs">
            <span className="font-bold">Conversations</span>
          </div>
          <div className="flex flex-col space-y-1 mt-4 -mx-2 min-h-10 overflow-y-auto">
            <button className="flex flex-row items-center hover:bg-gray-100 rounded-xl p-2">
              <div className="flex items-center justify-center h-8 w-8 bg-indigo-200 rounded-full">
                H
              </div>
              <div className="ml-2 text-sm font-semibold">Henry Boyd</div>
            </button>
            <button className="flex flex-row items-center hover:bg-gray-100 rounded-xl p-2">
              <div className="flex items-center justify-center h-8 w-8 bg-gray-200 rounded-full">
                M
              </div>
              <div className="ml-2 text-sm font-semibold">Marta Curtis</div>
              <div className="flex items-center justify-center ml-auto text-xs text-white bg-red-500 h-4 w-4 rounded leading-none">
                2
              </div>
            </button>
            <button className="flex flex-row items-center hover:bg-gray-100 rounded-xl p-2">
              <div className="flex items-center justify-center h-8 w-8 bg-orange-200 rounded-full">
                P
              </div>
              <div className="ml-2 text-sm font-semibold">Philip Tucker</div>
            </button>
            <button className="flex flex-row items-center hover:bg-gray-100 rounded-xl p-2">
              <div className="flex items-center justify-center h-8 w-8 bg-pink-200 rounded-full">
                C
              </div>
              <div className="ml-2 text-sm font-semibold">Christine Reid</div>
            </button>
            <button className="flex flex-row items-center hover:bg-gray-100 rounded-xl p-2">
              <div className="flex items-center justify-center h-8 w-8 bg-purple-200 rounded-full">
                J
              </div>
              <div className="ml-2 text-sm font-semibold">Jerry Guzman</div>
            </button>
          </div>
        </div>
      </div>
    </div>
  );
};

export default BurgerMenu;
