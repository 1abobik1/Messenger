import React, {createContext, useState} from 'react';

export const AuthContext = createContext(null);

export const AuthProvider = ({children}) => {
  const [user, setUser] = useState(JSON.parse(localStorage.getItem('userData')));

  const signin = (newUser, cb) => {
    localStorage.setItem('userData', JSON.stringify(newUser));
    setUser(newUser);
    cb();
  };

  const signout = (cb) => {
    localStorage.removeItem('userData');
    setUser(null);
    cb();
  };

  const value = {user, signin, signout};

  return (
    <AuthContext.Provider value={value}>
      {children}
    </AuthContext.Provider>
  );
};
