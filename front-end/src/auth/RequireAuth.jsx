import React from 'react';
import {Navigate, useLocation} from "react-router-dom";
import useAuth from "./useAuth";

const RequireAuth = ({children}) => {
  const location = useLocation();
  const {user} = useAuth();

  if (!user){
    return <Navigate to='/' state={{from: location}} replace={true}/>
  }
  return children;
};

export default RequireAuth;
