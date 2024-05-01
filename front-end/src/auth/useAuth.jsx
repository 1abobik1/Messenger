import {useContext} from 'react';
import {AuthContext} from "./AuthProvider";

const UseAuth = () => {
  return useContext(AuthContext);
};

export default UseAuth;
